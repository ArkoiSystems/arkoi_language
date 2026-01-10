#include "arkoi_language/x86_64/register_allocation.hpp"

#include <array>
#include <ranges>

using namespace arkoi::x86_64;

void RegisterAllocater::run() {
    _renumber();
    _build();
    _simplify();
}

void RegisterAllocater::_renumber() {
    _analysis = il::DataflowAnalysis<il::InstructionLivenessAnalysis>();
    _analysis.run(_function);
}

void RegisterAllocater::_build() {
    _graph = InterferenceGraph<il::Variable>();

    for (const auto& in_operands : std::views::values(_analysis.in())) {
        for (const auto& operand : in_operands) {
            const auto* op_variable = std::get_if<il::Variable>(&operand);
            if (!op_variable) continue;
            _graph.add_node(*op_variable);
        }
    }

    for (const auto& [instruction, out_operands] : _analysis.out()) {
        for (const auto& definition : instruction->defs()) {
            const auto* def_variable = std::get_if<il::Variable>(&definition);
            if (!def_variable) continue;

            for (const auto& operand : out_operands) {
                const auto* op_variable = std::get_if<il::Variable>(&operand);
                if (!op_variable) continue;

                _graph.add_edge(*def_variable, *op_variable);
            }
        }
    }
}

void RegisterAllocater::_simplify() {
    std::stack<il::Variable> stack;

    auto work_list = _graph.nodes();

    // Remove precolored variables.
    for (const auto& variable : std::views::keys(_assigned)) {
        work_list.erase(variable);
    }

    while (!work_list.empty()) {
        auto found = false;

        for (const auto& node : work_list) {
            const auto interferences = _graph.interferences(node);

            const auto is_floating = std::holds_alternative<sem::Floating>(node.type());
            if (is_floating && interferences.size() >= FLOATING_REGISTERS.size()) continue;
            if (!is_floating && interferences.size() >= INTEGER_CALLEE_SAVED.size()) continue;

            stack.push(node);
            work_list.erase(node);
            found = true;
            break;
        }

        if (found) continue;

        const auto& first = *work_list.begin();
        _spilled.insert(first);
        work_list.erase(first);
    }

    while (!stack.empty()) {
        auto node = stack.top();
        stack.pop();

        std::unordered_set<Register::Base> colors;
        for (const auto& interference : _graph.interferences(node)) {
            const auto found = _assigned.find(interference);
            if (found == _assigned.end()) continue;
            colors.insert(found->second);
        }

        auto found = false;
        if (std::holds_alternative<sem::Floating>(node.type())) {
            for (const auto base : FLOATING_REGISTERS) {
                if (colors.contains(base)) continue;
                _assigned[node] = base;
                found = true;
                break;
            }
        } else {
            for (const auto base : INTEGER_CALLEE_SAVED) {
                if (colors.contains(base)) continue;
                _assigned[node] = base;
                found = true;
                break;
            }
        }

        if (found) continue;

        _spilled.insert(node);
    }
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
