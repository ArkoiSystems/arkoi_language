#include "arkoi_language/x86_64/mapper.hpp"

#include <ranges>

#include "arkoi_language/il/cfg.hpp"
#include "arkoi_language/utils/utils.hpp"
#include "arkoi_language/x86_64/register_allocation.hpp"

using namespace arkoi::x86_64;
using namespace arkoi;

Mapper::Mapper(il::Function& function) :
    _register_allocator(function), _function(function) {
    function.accept(*this);
}

Operand Mapper::operator[](const il::Operand& operand) {
    return std::visit(
        match{
            [&](const il::Variable& variable) -> Operand {
                return _mappings.at(variable);
            },
            [&](const il::Memory& memory) -> Operand {
                return _mappings.at(memory);
            },
            [&](const il::Immediate& immediate) -> Operand {
                return std::visit([](const auto& value) -> Immediate { return value; }, immediate);
            }
        },
        operand
    );
}

size_t Mapper::stack_size() const {
    size_t stack_size = 0;

    for (const auto& local : _locals) {
        const auto size = size_to_bytes(local.type().size());
        stack_size += size;
    }

    return align_size(stack_size);
}

Register Mapper::return_register(const sem::Type& target) {
    return std::visit(
        match{
            [&](const sem::Integral& type) -> Register {
                return { Register::Base::A, type.size() };
            },
            [&](const sem::Floating& type) -> Register {
                return { Register::Base::XMM0, type.size() };
            },
            [&](const sem::Boolean&) -> Register {
                return { Register::Base::A, Size::BYTE };
            }
        },
        target
    );
}

size_t Mapper::align_size(const size_t input) {
    static constexpr size_t STACK_ALIGNMENT = 16;
    return (input + (STACK_ALIGNMENT - 1)) & ~(STACK_ALIGNMENT - 1);
}

void Mapper::visit(il::Module&) {
    throw std::runtime_error("Mapper::visit(Module) should never be called.");
}

void Mapper::visit(il::Function& function) {
    // --- Phase 1: Create the mappings and precoloring ---

    // Iterate over each block and their corresponding instructions
    // to map operands to their mapped x86_64 equivalent.
    for (auto& block : function) {
        block.accept(*this);
    }

    _map_parameters(function.parameters());

    // --- Phase 2: Try to reduce locals by using register allocation ---
    _register_allocator.run();

    for (const auto& [variable, base] : _register_allocator.assigned()) {
        const auto target = Register(base, variable.type().size());
        _add_register(variable, target);
    }

    // --- Phase 3: Use the redzone for the remaining memory operands if applicable ---

    const auto use_redzone = function.is_leaf() && this->stack_size() <= 128;
    const auto stack_reg = use_redzone ? RSP : RBP;

    if (use_redzone) {
        // If we can use the redzone, overwrite all the stack-allocated memory operands
        // to use RSP instead of RBP.
        for (auto& [source, target] : _mappings) {
            auto* variable = std::get_if<il::Variable>(&source);
            if (!variable) continue;

            auto* memory = std::get_if<Memory>(&target);
            if (!memory) continue;

            auto *reg = std::get_if<Register>(&memory->address());
            if (!reg || *reg != RBP) continue;

            memory->set_address(stack_reg);
        }
    }

    // --- Phase 4: Transform the remaining locals to be stored on the stack ---

    int64_t local_offset = 0;
    for (auto& local : _locals) {
        auto size = local.type().size();
        local_offset -= static_cast<int64_t>(size_to_bytes(size));

        _mappings.insert_or_assign(local, Memory(size, stack_reg, local_offset));
    }
}

void Mapper::_map_parameters(const std::vector<il::Variable>& parameters) {
    size_t stack = 0, integer = 0, floating = 0;

    for (auto& parameter : parameters) {
        const auto& type = parameter.type();

        if (std::holds_alternative<sem::Integral>(type) || std::holds_alternative<sem::Boolean>(type)) {
            if (integer < INTEGER_ARGUMENT_REGISTERS.size()) {
                auto reg = Register(INTEGER_ARGUMENT_REGISTERS[integer], type.size());
                _add_register(parameter, reg);
                integer++;
                continue;
            }
        } else if (std::holds_alternative<sem::Floating>(type)) {
            if (floating < SSE_ARGUMENT_REGISTERS.size()) {
                auto reg = Register(SSE_ARGUMENT_REGISTERS[floating], type.size());
                _add_register(parameter, reg);
                floating++;
                continue;
            }
        }

        const auto offset = static_cast<int64_t>(16 + 8 * stack);
        auto memory = Memory(type.size(), RBP, offset);
        _add_memory(parameter, memory);
        stack++;
    }
}

void Mapper::visit(il::BasicBlock& block) {
    for (auto& instruction : block) {
        instruction.accept(*this);
    }
}

void Mapper::visit(il::Binary& instruction) {
    _add_local(instruction.result());
}

void Mapper::visit(il::Cast& instruction) {
    _add_local(instruction.result());
}

void Mapper::visit(il::Return& instruction) {
    const auto* variable = std::get_if<il::Variable>(&instruction.value());
    if (!variable) return;

    const auto result_reg = return_register(variable->type());
    _add_register(*variable, result_reg);
}

void Mapper::visit(il::Argument& argument) {
    auto& [
        integer,
        floating,
        stack,
        stack_size
    ] = _current_call_frame;

    const auto& result = argument.result();
    const auto& type = result.type();

    if (std::holds_alternative<sem::Integral>(type) || std::holds_alternative<sem::Boolean>(type)) {
        if (integer.size() < INTEGER_ARGUMENT_REGISTERS.size()) {
            const auto reg = Register(INTEGER_ARGUMENT_REGISTERS[integer.size()], type.size());
            _add_register(result, reg);
            integer.push_back(&argument);
            return;
        }
    } else if (std::holds_alternative<sem::Floating>(type)) {
        if (floating.size() < SSE_ARGUMENT_REGISTERS.size()) {
            const auto reg = Register(SSE_ARGUMENT_REGISTERS[floating.size()], type.size());
            _add_register(result, reg);
            floating.push_back(&argument);
            return;
        }
    }

    _add_memory(result, Memory(type.size(), RSP));
    stack.push_back(&argument);
    stack_size += 8;
}

void Mapper::visit(il::Call& instruction) {
    // Add the current call frame to the container.
    _call_frames[&instruction] = _current_call_frame;
    // Reset the current call frame to be empty.
    _current_call_frame = { };
}

void Mapper::visit(il::Alloca& instruction) {
    _add_local(instruction.result());
}

void Mapper::visit(il::Load& instruction) {
    _add_local(instruction.result());
}

void Mapper::visit(il::Phi& instruction) {
    _add_local(instruction.result());
}

void Mapper::visit(il::Assign& instruction) {
    _add_local(instruction.result());
}

void Mapper::_add_local(const il::Operand& operand) {
    _locals.insert(operand);
}

void Mapper::_add_register(const il::Variable& variable, const Register& reg) {
    _locals.erase(variable);
    _mappings.insert_or_assign(variable, reg);
    _register_allocator.precolored().insert_or_assign(variable, reg.base());
}

void Mapper::_add_memory(const il::Variable& variable, const Memory& memory) {
    _locals.erase(variable);
    _mappings.insert_or_assign(variable, memory);
}

//==============================================================================
// BSD 3-Clause License
//
// Copyright (c) 2025, Timo Behrend
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//==============================================================================
