#pragma once

#include "arkoi_language/il/dataflow.hpp"

namespace arkoi::il {
/**
 * @brief Dataflow analysis for computing liveness at the block level
 */
class BlockLivenessAnalysis final :
    public DataflowPass<Operand, DataflowDirection::Backward, DataflowGranularity::Block> {
public:
    BlockLivenessAnalysis() = default;

    /**
     * @brief Merges states from predecessors
     *
     * @param predecessors The states of predecessors to merge
     *
     * @return The merged state
     */
    [[nodiscard]] State merge(const std::vector<State>& predecessors) override;

    /**
     * @brief Initializes the analysis state for a basic block
     *
     * @param function The function containing the block
     * @param current The basic block to initialize
     *
     * @return The initial state
     */
    [[nodiscard]] State initialize(Function& function, BasicBlock& current) override;

    /**
     * @brief Transfer function for a basic block
     *
     * @param current The basic block
     * @param state The current state
     *
     * @return The new state after applying the transfer function
     */
    [[nodiscard]] State transfer(BasicBlock& current, const State& state) override;
};

/**
 * @brief Dataflow analysis for computing liveness at the instruction level
 */
class InstructionLivenessAnalysis final :
    public DataflowPass<Operand, DataflowDirection::Backward, DataflowGranularity::Instruction> {
public:
    InstructionLivenessAnalysis() = default;

    /**
     * @brief Merges states from predecessors
     *
     * @param predecessors The states of predecessors to merge
     *
     * @return The merged state
     */
    [[nodiscard]] State merge(const std::vector<State>& predecessors) override;

    /**
     * @brief Initializes the analysis state for an instruction
     *
     * @param function The function containing the instruction
     * @param instruction The instruction to initialize
     *
     * @return The initial state
     */
    [[nodiscard]] State initialize(Function& function, Instruction& instruction) override;

    /**
     * @brief Transfer function for an instruction
     *
     * @param current The instruction
     * @param state The current state
     *
     * @return The new state after applying the transfer function
     */
    [[nodiscard]] State transfer(Instruction& current, const State& state) override;
};
} // namespace arkoi::il

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
