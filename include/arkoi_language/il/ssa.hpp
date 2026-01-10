#pragma once

#include <stack>

#include "arkoi_language/il/cfg.hpp"

namespace arkoi::il {
class SSAPromoter {
public:
    explicit SSAPromoter(Function &function);

    void promote();

private:
    [[nodiscard]] std::set<std::string> _collect_candidates() const;

    void _place_phi_nodes(const std::string& candidate) const;

    void _rename(BasicBlock *block, std::unordered_set<BasicBlock*>& visited);

private:
    std::unordered_map<BasicBlock*, std::vector<BasicBlock*>> _children{ };
    std::unordered_map<std::string, std::stack<size_t>> _stacks{ };
    std::unordered_map<std::string, size_t> _counters{ };
    DominatorTree::Frontiers _frontiers{ };
    std::set<std::string> _candidates{ };
    Function& _function;
};

class PhiLowerer {
public:
    explicit PhiLowerer(Function &function);

    void lower() const;

private:
    Function& _function;
};
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
