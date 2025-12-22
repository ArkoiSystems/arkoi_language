#pragma once

#include "arkoi_language/il/dataflow.hpp"

namespace arkoi::il {
/**
 * @brief Dataflow analysis for computing liveness at the basic block level.
 *
 * This analysis determines which operands are 'live' at the entry and exit of each
 * basic block. An operand is live if its current value may be read in the future
 * before it is overwritten.
 *
 * Direction: Backward
 * Granularity: Block
 *
 * @see DataflowPass
 */
class BlockLivenessAnalysis final :
    public DataflowPass<Operand, DataflowDirection::Backward, DataflowGranularity::Block> {
public:
    BlockLivenessAnalysis() = default;

    /**
     * @brief Merges liveness states from successor blocks.
     *
     * In a backward analysis, the merge operation typically performs a union
     * of the live sets from all successors.
     *
     * @param successors The liveness sets from all successor basic blocks.
     * @return The combined liveness state.
     */
    [[nodiscard]] State merge(const std::vector<State>& successors) override;

    /**
     * @brief Initializes the liveness state for a specific basic block.
     *
     * @param function The function containing the block.
     * @param current The basic block to initialize.
     * @return An empty or conservative initial liveness state.
     */
    [[nodiscard]] State initialize(Function& function, BasicBlock& current) override;

    /**
     * @brief Applies the backward transfer function to a basic block.
     *
     * The transfer function for liveness is: `In[B] = Use[B] U (Out[B] - Def[B])`.
     *
     * @param current The basic block being processed.
     * @param state The liveness state at the exit of the block.
     * @return The liveness state at the entry of the block.
     */
    [[nodiscard]] State transfer(BasicBlock& current, const State& state) override;
};

/**
 * @brief Dataflow analysis for computing liveness at the individual instruction level.
 *
 * This provides finer granularity than `BlockLivenessAnalysis`, determining
 * liveness at every program point between instructions. This is crucial for
 * interference graph construction during register allocation.
 *
 * Direction: Backward
 * Granularity: Instruction
 *
 * @see DataflowPass, BlockLivenessAnalysis, x86_64::RegisterAllocater
 */
class InstructionLivenessAnalysis final :
    public DataflowPass<Operand, DataflowDirection::Backward, DataflowGranularity::Instruction> {
public:
    InstructionLivenessAnalysis() = default;

    /**
     * @brief Merges liveness states from the succeeding program point.
     *
     * @param successors The liveness states from successor points.
     * @return The merged liveness state.
     */
    [[nodiscard]] State merge(const std::vector<State>& successors) override;

    /**
     * @brief Initializes the liveness state for an instruction point.
     *
     * @param function The function containing the instruction.
     * @param instruction The instruction to initialize.
     * @return The initial liveness state.
     */
    [[nodiscard]] State initialize(Function& function, Instruction& instruction) override;

    /**
     * @brief Applies the backward transfer function to a single instruction.
     *
     * `In[I] = Use[I] U (Out[I] - Def[I])`.
     *
     * @param current The instruction being processed.
     * @param state The liveness state after the instruction.
     * @return The liveness state before the instruction.
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
