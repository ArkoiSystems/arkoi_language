#include "arkoi_language/il/ssa.hpp"

#include "arkoi_language/utils/utils.hpp"

using namespace arkoi::il;
using namespace arkoi;

SSAPromoter::SSAPromoter(Function& function) :
    _function(function) {
    _frontiers = DominatorTree::compute_frontiers(_function);

    _candidates = _collect_candidates();
    for (const auto& candidate : _candidates) {
        _stacks[candidate].push(0);
        _counters[candidate] = 0;
    }

    auto immediates = DominatorTree::compute_immediates(_function);
    for (auto& [block, immediate] : immediates) {
        if (!immediate) continue;
        _children[immediate].push_back(block);
    }
}

void SSAPromoter::promote() {
    if (_candidates.empty()) return;

    for (const auto& candidate : _candidates) {
        _place_phi_nodes(candidate);
    }

    std::unordered_set<BasicBlock*> visited{ };
    _rename(_function.entry(), visited);
}

std::set<std::string> SSAPromoter::_collect_candidates() const {
    std::set<std::string> candidates{ };
    for (auto& block : _function) {
        for (auto& instruction : block) {
            const auto* alloca = std::get_if<Alloca>(&instruction);
            if (!alloca) continue;

            const auto& name = alloca->result().name();
            if (candidates.contains(name)) {
                throw std::runtime_error("SSA conversion failed: multiple definitions of variable.");
            }

            candidates.insert(name);
        }
    }
    return candidates;
}

void SSAPromoter::_place_phi_nodes(const std::string& candidate) const {
    std::unordered_set<BasicBlock*> definition_blocks{ };

    std::optional<sem::Type> type{ };
    for (auto& block : _function) {
        for (auto& instruction : block) {
            if (const auto* alloca = std::get_if<Alloca>(&instruction)) {
                if (alloca->result().name() != candidate) continue;
                definition_blocks.insert(&block);

                // This will only be set once, because `_collect_candidates` makes sure
                // that a name only has one alloca.
                type = alloca->result().type();
            }

            if (const auto* store = std::get_if<Store>(&instruction)) {
                if (store->result().name() != candidate) continue;
                definition_blocks.insert(&block);
            }
        }
    }

    const Variable variable{ candidate, *type };

    std::deque worklist(definition_blocks.begin(), definition_blocks.end());
    std::unordered_set<BasicBlock*> inserted_blocks{ };

    while (!worklist.empty()) {
        auto* block = worklist.front();
        worklist.pop_front();

        if (!_frontiers.contains(block)) continue;

        for (auto* frontier : _frontiers.at(block)) {
            if (inserted_blocks.contains(frontier)) continue;
            inserted_blocks.insert(frontier);

            frontier->instructions().insert(
                frontier->instructions().begin(),
                Phi(variable, { }, std::nullopt)
            );

            if (!definition_blocks.contains(frontier)) {
                worklist.push_back(frontier);
            }
        }
    }
}

void SSAPromoter::_rename(BasicBlock* block, std::unordered_set<BasicBlock*>& visited) {
    if (visited.contains(block)) return;
    visited.insert(block);

    std::unordered_map<std::string, size_t> pushed_count;
    for (auto it = block->instructions().begin(); it != block->instructions().end(); ++it) {
        auto& instruction = *it;

        if (auto* phi = std::get_if<Phi>(&instruction)) {
            const auto& name = phi->result().name();
            if (!_candidates.contains(name)) continue;

            const auto new_version = _counters[name]++;
            phi->result().set_version(new_version);

            _stacks[name].push(new_version);
            pushed_count[name]++;
        } else if (auto* store = std::get_if<Store>(&instruction)) {
            const auto& name = store->result().name();
            if (!_candidates.contains(name)) continue;

            const auto new_version = _counters[name]++;
            const auto result = Variable{ name, store->result().type(), new_version };

            _stacks[name].push(new_version);
            pushed_count[name]++;

            instruction = Assign(result, store->source(), store->span());
        } else if (const auto* load = std::get_if<Load>(&instruction)) {
            const auto& name = load->source().name();
            if (!_candidates.contains(name)) continue;

            const auto version = _stacks[name].top();
            const auto source = Variable{ name, load->source().type(), version };

            instruction = Assign(load->result(), source, load->span());
        } else if (const auto* alloca = std::get_if<Alloca>(&*it)) {
            const auto& name = alloca->result().name();
            if (!_candidates.contains(name)) continue;

            it = --block->instructions().erase(it);
        }
    }

    auto handle_successor = [&](BasicBlock* successor) {
        if (!successor) return;

        for (auto& instruction : successor->instructions()) {
            if (auto* phi = std::get_if<Phi>(&instruction)) {
                const auto& name = phi->result().name();
                if (!_candidates.contains(name)) continue;

                const auto version = _stacks[name].top();
                const auto source = Variable{ name, phi->result().type(), version };

                phi->incoming().insert_or_assign(block->label(), source);
            }
        }
    };

    handle_successor(block->next());
    handle_successor(block->branch());

    if (_children.contains(block)) {
        for (auto* child : _children.at(block)) {
            _rename(child, visited);
        }
    }

    for (auto& [name, count] : pushed_count) {
        for (size_t index = 0; index < count; ++index) {
            _stacks[name].pop();
        }
    }
}

PhiLowerer::PhiLowerer(Function& function) :
    _function(function) { }

void PhiLowerer::lower() const {
    for (auto& block : _function) {
        std::vector<Phi*> phis;

        for (auto& instruction : block.instructions()) {
            auto* phi = std::get_if<Phi>(&instruction);
            if (!phi) continue;

            phis.push_back(phi);
        }

        if (phis.empty()) continue;

        for (auto* predecessor : block.predecessors()) {
            for (const auto& phi : phis) {
                const auto& result = phi->result();

                auto source_result = phi->incoming().find(predecessor->label());
                if (source_result == phi->incoming().end()) continue;

                const auto& source = source_result->second;

                auto rit = predecessor->instructions().rbegin();
                for (; rit != predecessor->instructions().rend(); ++rit) {
                    const auto& instruction = *rit;
                    if (!(std::get_if<Goto>(&instruction) || std::get_if<If>(&instruction)
                          || std::get_if<Return>(&instruction))) {
                        break;
                    }
                }

                predecessor->instructions().insert(
                    rit.base(),
                    Assign(result, source, std::nullopt)
                );
            }
        }

        auto& instructions = block.instructions();
        std::erase_if(
            instructions,
            [](auto& instruction) {
                return std::holds_alternative<Phi>(instruction);
            }
        );
    }
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
