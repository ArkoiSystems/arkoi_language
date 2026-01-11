#include "arkoi_language/x86_64/register_allocation.hpp"

#include <array>

using namespace arkoi::x86_64;
using namespace arkoi;

void RegisterAllocater::run() {
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

void RegisterAllocater::_cleanup() {
    _graph = utils::InterferenceGraph<il::Variable>();
    _assigned.clear();
    _spilled.clear();
    _stack.clear();
}

void RegisterAllocater::_renumber() {
    _liveness_analysis = std::make_shared<il::InstructionLivenessAnalysis>();
    _analysis = il::DataflowAnalysis(_liveness_analysis);
    _analysis.run(_function);
}

void RegisterAllocater::_build() {
    for (auto& block : _function) {
        for (auto& instruction : block) {
            const auto& defs = instruction.defs();
            for (const auto& def : defs) {
                auto* def_variable = std::get_if<il::Variable>(&def);
                if (!def_variable) continue;
                _graph.add_node(*def_variable);
            }

            const auto& uses = instruction.uses();
            for (const auto& use : uses) {
                auto* use_variable = std::get_if<il::Variable>(&use);
                if (!use_variable) continue;
                _graph.add_node(*use_variable);
            }
        }
    }

    for (const auto& [instruction, outs] : _analysis.out()) {
        const auto& defs = instruction->defs();

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

        for (size_t i = 0; i < defs.size(); i++) {
            auto* i_def = std::get_if<il::Variable>(&defs[i]);
            if (!i_def) continue;
            for (size_t j = i + 1; j < defs.size(); j++) {
                auto* j_def = std::get_if<il::Variable>(&defs[j]);
                if (!j_def) continue;

                _graph.add_edge(*i_def, *j_def);
            }
        }
    }

    for (const auto& [variable, color] : _precolored) {
        _assigned.insert_or_assign(variable, color);
    }
}

void RegisterAllocater::_simplify() {
    auto work_list = _graph.nodes();
    // TODO: Do this differently.
    auto temp_graph = _graph;

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
        const auto first_size = temp_graph.interferences(first).size();
        const auto first_k = compute_k(first);

        const auto second_size = temp_graph.interferences(second).size();
        const auto second_k = compute_k(second);

        return (first_size * first_k) < (second_size * second_k);
    };

    for (const auto& variable : std::views::keys(_precolored)) {
        work_list.erase(variable);
    }

    while (!work_list.empty()) {
        auto found = false;

        for (auto it = work_list.begin(); it != work_list.end(); ++it) {
            const auto& node = *it;

            const auto interferences = temp_graph.interferences(node);
            if (interferences.size() >= compute_k(node)) continue;

            _stack.push_back(node);
            temp_graph.remove_node(node);
            work_list.erase(it);

            found = true;
            break;
        }

        if (found) continue;

        const auto& candidate = *std::max_element(work_list.begin(), work_list.end(), spill_cost);
        _stack.push_back(candidate);
        temp_graph.remove_node(candidate);
        work_list.erase(candidate);
    }
}

void RegisterAllocater::_select() {
    while (!_stack.empty()) {
        auto node = _stack.back();
        _stack.pop_back();
        if (_assigned.contains(node)) continue;

        std::unordered_set<Register::Base> forbidden;
        for (const auto& interference : _graph.interferences(node)) {
            const auto found = _assigned.find(interference);
            if (found == _assigned.end()) continue;
            forbidden.insert(found->second);
        }

        auto success = false;
        if (std::holds_alternative<sem::Floating>(node.type())) {
            for (const auto base : FLOATING_REGISTERS) {
                if (forbidden.contains(base)) continue;

                _assigned[node] = base;
                success = true;
                break;
            }
        } else if (_liveness_analysis->live_across_calls().contains(node)) {
            for (const auto base : INTEGER_CALLEE_SAVED) {
                if (forbidden.contains(base)) continue;

                _assigned[node] = base;
                success = true;
                break;
            }
        } else {
            for (const auto base : INTEGER_CALLER_SAVED) {
                if (forbidden.contains(base)) continue;

                _assigned[node] = base;
                success = true;
                break;
            }
        }

        if (success) continue;
        _spilled.insert(node);
    }
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void RegisterAllocater::_rewrite() {
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
