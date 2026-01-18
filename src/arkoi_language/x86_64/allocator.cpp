#include "arkoi_language/x86_64/allocator.hpp"

#include <array>

#include "arkoi_language/utils/utils.hpp"

using namespace arkoi::x86_64;
using namespace arkoi;

void PreColorer::run() {
    _function.accept(*this);
}

Register PreColorer::return_register(const sem::Type& target) {
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

void PreColorer::visit(il::Function& function) {
    size_t integer = 0, floating = 0;

    for (auto& parameter : function.parameters()) {
        std::visit(
            match{
                [&](const sem::Floating& type) {
                    if (floating >= SSE_ARGUMENT_REGISTERS.size()) return;

                    const auto reg = Register(SSE_ARGUMENT_REGISTERS[floating++], type.size());
                    _assigned.insert_or_assign(parameter, reg.base());
                },
                [&](const sem::Boolean& type) {
                    if (integer >= INTEGER_ARGUMENT_REGISTERS.size()) return;

                    const auto reg = Register(INTEGER_ARGUMENT_REGISTERS[integer++], type.size());
                    _assigned.insert_or_assign(parameter, reg.base());
                },
                [&](const sem::Integral& type) {
                    if (integer >= INTEGER_ARGUMENT_REGISTERS.size()) return;

                    const auto reg = Register(INTEGER_ARGUMENT_REGISTERS[integer++], type.size());
                    _assigned.insert_or_assign(parameter, reg.base());
                }
            },
            parameter.type()
        );
    }

    for (auto& block : function) {
        block.accept(*this);
    }
}

void PreColorer::visit(il::BasicBlock& block) {
    for (auto& instruction : block) {
        instruction.accept(*this);
    }
}

void PreColorer::visit(il::Return& instruction) {
    const auto* variable = std::get_if<il::Variable>(&instruction.value());
    if (!variable) return;

    const auto reg = return_register(instruction.value().type());
    _assigned.insert_or_assign(*variable, reg.base());
}

void PreColorer::visit(il::Argument& argument) {
    const auto& result = argument.result();
    const auto& type = result.type();

    if (std::holds_alternative<sem::Integral>(type) || std::holds_alternative<sem::Boolean>(type)) {
        if (_integer < INTEGER_ARGUMENT_REGISTERS.size()) {
            const auto reg = Register(INTEGER_ARGUMENT_REGISTERS[_integer++], type.size());
            _assigned.insert_or_assign(result, reg.base());
        }
    } else if (std::holds_alternative<sem::Floating>(type)) {
        if (_floating < SSE_ARGUMENT_REGISTERS.size()) {
            const auto reg = Register(SSE_ARGUMENT_REGISTERS[_floating++], type.size());
            _assigned.insert_or_assign(result, reg.base());
        }
    }
}

void PreColorer::visit(il::Call&) {
    // Reset the floating and integer counter for the next call.
    _floating = 0;
    _integer = 0;
}

void RegisterAllocator::run() {
    do {
        _renumber();
        _build();
        _simplify();
        _select();

        if (!_spilled.empty()) {
            _rewrite();
            _cleanup();
        }
    } while (!_spilled.empty());
}

void RegisterAllocator::_cleanup() {
    _graph = utils::InterferenceGraph<il::Variable>();
    _assigned.clear();
    _spilled.clear();
    _stack.clear();
}

void RegisterAllocator::_renumber() {
    _liveness_analysis = std::make_shared<il::InstructionLivenessAnalysis>();
    _analysis = il::DataflowAnalysis(_liveness_analysis);
    _analysis.run(_function);
}

void RegisterAllocator::_build() {
    for (auto& block : _function) {
        for (auto& instruction : block) {
            for (const auto& def : instruction.defs()) {
                auto* def_variable = std::get_if<il::Variable>(&def);
                if (!def_variable) continue;
                _graph.add_node(*def_variable);
            }

            for (const auto& use : instruction.uses()) {
                auto* use_variable = std::get_if<il::Variable>(&use);
                if (!use_variable) continue;
                _graph.add_node(*use_variable);
            }
        }
    }

    auto add_clique = [&](auto const& operands) {
        for (size_t i = 0; i < operands.size(); ++i) {
            auto* i_op = std::get_if<il::Variable>(&operands[i]);
            if (!i_op) continue;

            for (size_t j = i + 1; j < operands.size(); ++j) {
                auto* j_op = std::get_if<il::Variable>(&operands[j]);
                if (!j_op) continue;

                _graph.add_edge(*i_op, *j_op);
            }
        }
    };

    for (const auto& [instruction, outs] : _analysis.out()) {
        const auto& defs = instruction->defs();
        const auto& uses = instruction->uses();

        for (const auto& def : defs) {
            auto* def_variable = std::get_if<il::Variable>(&def);
            if (!def_variable) continue;

            for (const auto& out : outs) {
                auto* out_variable = std::get_if<il::Variable>(&out);
                if (!out_variable) continue;
                if (*def_variable == *out_variable) continue;

                _graph.add_edge(*def_variable, *out_variable);
            }
        }

        add_clique(uses);
        add_clique(defs);
    }

    PreColorer pre_colorer(_function);
    pre_colorer.run();

    for (const auto& [variable, color] : pre_colorer.assigned()) {
        _assigned.insert_or_assign(variable, color);
    }
}

void RegisterAllocator::_simplify() {
    auto work_list = _graph.nodes();

    std::unordered_map<il::Variable, size_t> current_degree;
    for (const auto& node : work_list) {
        current_degree[node] = _graph.interferences(node).size();
    }

    for (const auto& variable : std::views::keys(_assigned)) {
        work_list.erase(variable);
    }

    auto compute_k = [&](const il::Variable& variable) {
        if (std::holds_alternative<sem::Floating>(variable.type())) {
            return FLOATING_REGISTERS.size();
        }

        if (_liveness_analysis->live_across_calls().contains(variable)) {
            return INTEGER_CALLEE_SAVED.size();
        }

        return INTEGER_CALLER_SAVED.size();
    };

    // For now this spill cost is only calculated on its degree.
    auto spill_cost = [&](const il::Variable& first, const il::Variable& second) {
        const auto first_size = current_degree[first];
        const auto first_k = compute_k(first);

        const auto second_size = current_degree[second];
        const auto second_k = compute_k(second);

        return (first_size * first_k) < (second_size * second_k);
    };

    auto remove_node = [&](const il::Variable& node) {
        for (const auto& neighbor : _graph.interferences(node)) {
            if (!current_degree.contains(neighbor)) continue;
            current_degree[neighbor]--;
        }
        work_list.erase(node);
    };

    while (!work_list.empty()) {
        auto result = std::ranges::find_if(
            work_list,
            [&](const auto& node) {
                return current_degree[node] < compute_k(node);
            }
        );

        if (result != work_list.end()) {
            _stack.push_back(*result);
            remove_node(*result);
            continue;
        }

        const auto& candidate = *std::max_element(work_list.begin(), work_list.end(), spill_cost);
        _stack.push_back(candidate);
        remove_node(candidate);
    }
}

void RegisterAllocator::_select() {
    while (!_stack.empty()) {
        auto node = _stack.back();
        _stack.pop_back();

        if (_assigned.contains(node)) {
            continue;
        }

        std::unordered_set<Register::Base> taken;
        for (const auto& interference : _graph.interferences(node)) {
            const auto found = _assigned.find(interference);
            if (found == _assigned.end()) continue;

            taken.insert(found->second);
        }

        auto try_assign = [&](const auto& regs) -> bool {
            for (const auto base : regs) {
                if (taken.contains(base)) continue;
                _assigned[node] = base;
                return true;
            }

            return false;
        };

        auto succeeded = false;
        if (std::holds_alternative<sem::Floating>(node.type())) {
            succeeded = try_assign(FLOATING_REGISTERS);
        } else if (_liveness_analysis->is_live_across_calls(node)) {
            succeeded = try_assign(INTEGER_CALLEE_SAVED);
        } else {
            succeeded = try_assign(INTEGER_CALLER_SAVED);
        }

        if (!succeeded) {
            _spilled.insert(node);
        }
    }
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void RegisterAllocator::_rewrite() {
    // TODO: Add spilled variables to the IL.
}

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
