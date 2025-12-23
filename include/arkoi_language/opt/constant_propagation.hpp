#pragma once

#include "arkoi_language/opt/pass.hpp"

namespace arkoi::opt {
/**
 * @brief Optimization pass that propagates constant values through the CFG.
 *
 * `ConstantPropagation` tracks variables that are assigned constant values and
 * replaces later uses of those variables with the constants themselves.
 * This is a local optimization performed within basic blocks.
 *
 * Example:
 * `x = 5`
 * `y = x + 1`
 * becomes:
 * `x = 5`
 * `y = 5 + 1` (which `ConstantFolding` can then turn into `y = 6`)
 *
 * @see Pass, ConstantFolding
 */
class ConstantPropagation final : public Pass {
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
     * @brief No-op for function entry.
     */
    bool enter_function([[maybe_unused]] il::Function& function) override { return false; }

    /**
     * @brief No-op for function exit.
     */
    bool exit_function([[maybe_unused]] il::Function& function) override { return false; }

    /**
     * @brief Performs constant propagation on the instructions within a basic block.
     *
     * Maintains a local map of variables to their known constant values.
     *
     * @param block The `il::BasicBlock` to optimize.
     * @return True if any operands were replaced with constants, false otherwise.
     */
    bool on_block(il::BasicBlock& block) override;

private:
    /**
     * @brief Attempts to propagate constants into the operands of an instruction.
     *
     * @param target The instruction to modify.
     * @return True if the instruction was modified.
     */
    [[nodiscard]] bool _propagate(il::Instruction& target);

    /**
     * @brief Replaces an operand with its known constant value if possible.
     *
     * @param operand The operand to potentially replace.
     * @return True if the operand was replaced with an `il::Immediate`.
     */
    [[nodiscard]] bool _propagate(il::Operand& operand);

private:
    std::unordered_map<il::Operand, il::Immediate> _constants;
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
