#include "arkoi_language/il/ssa.hpp"

#include <deque>

using namespace arkoi::il;
using namespace arkoi;

SSAPromoter::SSAPromoter(Function& function) :
    _function(function) {
    _frontiers = DominatorTree::compute_frontiers(_function);
    _candidates = _collect_candidates();
}

void SSAPromoter::promote() const {
    if (_candidates.empty()) return;

    for (const auto* candidate : _candidates) {
        _place_phi_nodes(candidate);
    }
}

std::vector<Alloca*> SSAPromoter::_collect_candidates() const {
    std::vector<Alloca*> candidates{ };
    for (auto& block : _function) {
        for (auto& instruction : block) {
            auto* alloca = std::get_if<Alloca>(&instruction);
            if (!alloca) continue;

            candidates.push_back(alloca);
        }
    }
    return candidates;
}

void SSAPromoter::_place_phi_nodes(const Alloca* candidate) const {
    std::unordered_set<BasicBlock*> definition_blocks{ };

    for (auto& block : _function) {
        for (auto& instruction : block) {
            if (const auto* store = std::get_if<Store>(&instruction)) {
                if (store->result() != candidate->result()) continue;
                definition_blocks.insert(&block);
            }

            if (const auto* alloca = std::get_if<Alloca>(&instruction)) {
                if (alloca->result() != candidate->result()) continue;
                definition_blocks.insert(&block);
            }
        }
    }

    std::deque<BasicBlock*> worklist{ definition_blocks.begin(), definition_blocks.end() };
    std::unordered_set<BasicBlock*> inserted_blocks{ };

    while (!worklist.empty()) {
        auto* block = worklist.front();
        worklist.pop_front();

        if (!_frontiers.contains(block)) continue;

        for (auto* frontier : _frontiers.at(block)) {
            if (inserted_blocks.contains(frontier)) continue;
            inserted_blocks.insert(frontier);

            const auto name = "$" + candidate->result().name().substr(1);
            const auto type = candidate->result().type();

            frontier->instructions().insert(
                frontier->instructions().begin(),
                Phi({ name, type }, { }, std::nullopt)
            );
        }
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
