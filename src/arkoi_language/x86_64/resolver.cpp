#include "arkoi_language/x86_64/resolver.hpp"

#include "arkoi_language/il/cfg.hpp"
#include "arkoi_language/utils/utils.hpp"
#include "arkoi_language/x86_64/allocator.hpp"

using namespace arkoi::x86_64;
using namespace arkoi;

void Resolver::run(il::Function& function, const Mapping &mapping) {
    for (auto& [variable, reg_base] : mapping) {
        auto reg = Register(reg_base, variable.type().size());
        _mappings.insert_or_assign(variable, reg);
    }

    function.accept(*this);
}

Operand Resolver::operator[](const il::Operand& operand) const {
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

size_t Resolver::stack_size() const {
    size_t stack_size = 0;

    for (const auto& local : _locals) {
        const auto size = size_to_bytes(local.type().size());
        stack_size += size;
    }

    return align_size(stack_size);
}

size_t Resolver::align_size(const size_t input) {
    static constexpr size_t STACK_ALIGNMENT = 16;
    return (input + (STACK_ALIGNMENT - 1)) & ~(STACK_ALIGNMENT - 1);
}

void Resolver::visit(il::Module&) {
    throw std::runtime_error("Resolver::visit(Module) should never be called.");
}

void Resolver::visit(il::Function& function) {
    // --- Phase 1: Create the mappings ---

    // Iterate over each block and their corresponding instructions
    // to map operands to their mapped x86_64 equivalent.
    for (auto& block : function) {
        block.accept(*this);
    }

    // --- Phase 2: Use the redzone for the remaining memory operands if applicable ---

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

void Resolver::visit(il::BasicBlock& block) {
    for (auto& instruction : block) {
        instruction.accept(*this);
    }
}

void Resolver::visit(il::Binary& instruction) {
    _add_local(instruction.result());
}

void Resolver::visit(il::Cast& instruction) {
    _add_local(instruction.result());
}

void Resolver::visit(il::Argument& argument) {
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
            integer.push_back(&argument);
            return;
        }
    } else if (std::holds_alternative<sem::Floating>(type)) {
        if (floating.size() < SSE_ARGUMENT_REGISTERS.size()) {
            floating.push_back(&argument);
            return;
        }
    }

    _add_memory(result, Memory(type.size(), RSP));
    stack.push_back(&argument);
    stack_size += 8;
}

void Resolver::visit(il::Call& instruction) {
    // Add the current call frame to the container.
    _call_frames[&instruction] = _current_call_frame;
    // Reset the current call frame to be empty.
    _current_call_frame = { };
}

void Resolver::visit(il::Alloca& instruction) {
    _add_local(instruction.result());
}

void Resolver::visit(il::Load& instruction) {
    _add_local(instruction.result());
}

void Resolver::visit(il::Phi& instruction) {
    _add_local(instruction.result());
}

void Resolver::visit(il::Assign& instruction) {
    _add_local(instruction.result());
}

void Resolver::_add_local(const il::Operand& operand) {
    if (_mappings.contains(operand)) return;

    _locals.insert(operand);
}

void Resolver::_add_memory(const il::Variable& variable, const Memory& memory) {
    if (_mappings.contains(variable)) return;

    _mappings.insert_or_assign(variable, memory);
    _locals.erase(variable);
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
