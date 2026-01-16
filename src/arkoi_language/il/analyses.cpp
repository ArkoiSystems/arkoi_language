#include "arkoi_language/il/analyses.hpp"

#include <ranges>

using namespace arkoi::il;
using namespace arkoi;

using State = BlockLivenessAnalysis::State;

State BlockLivenessAnalysis::merge(const std::vector<State>& predecessors) {
    State result;
    for (const auto& state : predecessors) result.insert(state.begin(), state.end());
    return result;
}

State BlockLivenessAnalysis::initialize(Function&, BasicBlock&) {
    return { };
}

State BlockLivenessAnalysis::transfer(BasicBlock& current, const State& state) {
    State in = state;

    for (auto& instruction : std::ranges::reverse_view(current.instructions())) {
        for (const auto& definition : instruction.defs()) {
            if (std::holds_alternative<Immediate>(definition)) continue;
            in.erase(definition);
        }

        for (const auto& use : instruction.uses()) {
            if (std::holds_alternative<Immediate>(use)) continue;
            in.insert(use);
        }
    }

    return in;
}

State InstructionLivenessAnalysis::merge(const std::vector<State>& predecessors) {
    State result;
    for (const auto& state : predecessors) result.insert(state.begin(), state.end());
    return result;
}

State InstructionLivenessAnalysis::initialize(Function&, Instruction&) {
    return { };
}

State InstructionLivenessAnalysis::transfer(Instruction& current, const State& state) {
    assert(!std::holds_alternative<Phi>(current));

    if (std::holds_alternative<Call>(current)) {
        State in = state;

        for (auto& definition : current.defs()) {
            in.erase(definition);
        }

        _live_across_calls.insert(in.begin(), in.end());
    }

    State in = state;
    for (const auto& definition : current.defs()) {
        if (std::holds_alternative<Immediate>(definition)) continue;
        in.erase(definition);
    }

    for (const auto& use : current.uses()) {
        if (std::holds_alternative<Immediate>(use)) continue;
        in.insert(use);
    }

    return in;
}

bool InstructionLivenessAnalysis::is_live_across_calls(const Operand& operand) const {
    return _live_across_calls.contains(operand);
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
