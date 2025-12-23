#pragma once

#include <unordered_set>

#include "arkoi_language/opt/pass.hpp"

namespace arkoi::opt {
/**
 * @brief Optimization pass that removes instructions whose results are never used.
 *
 * `DeadCodeElimination` (DCE) identifies 'dead' assignments—variables that are
 * written to but never subsequently read before the end of their lifetime.
 * It also removes instructions with no side effects that do not contribute
 * to the program's output.
 *
 * This implementation uses a simple usage-tracking mechanism across the function.
 *
 * @see Pass, il::InstructionBase::defs, il::InstructionBase::uses
 */
class DeadCodeElimination final : public Pass {
public:
    /**
     * @brief No-op for module entry.
     */
    bool enter_module([[maybe_unused]] il::Module& module) override { return false; }

    /**
     * @brief No-op for module exit.
     */
    bool exit_module([[maybe_unused]] il::Module& module) override { return false; }

    /**
     * @brief Pre-calculates the set of used operands for the entire function.
     *
     * Traverses all instructions in the function to determine which variables
     * are read at least once.
     *
     * @param function The `il::Function` being optimized.
     * @return False (this step only gathers information).
     */
    bool enter_function(il::Function& function) override;

    /**
     * @brief No-op for function exit.
     */
    bool exit_function([[maybe_unused]] il::Function& function) override { return false; }

    /**
     * @brief Removes dead instructions from a basic block.
     *
     * An instruction is removed if it defines a variable not in the
     * pre-calculated `_used` set, and the instruction has no side effects.
     *
     * @param block The `il::BasicBlock` to optimize.
     * @return True if any instructions were removed, false otherwise.
     */
    bool on_block(il::BasicBlock& block) override;

private:
    std::unordered_set<il::Operand> _used;
};
} // namespace arkoi::opt

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
