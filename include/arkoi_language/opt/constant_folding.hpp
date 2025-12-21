#pragma once

#include "arkoi_language/opt/pass.hpp"

namespace arkoi::opt {
class ConstantFolding final : public Pass {
public:
    /**
     * @brief Called when entering a module
     *
     * This is never getting used as the constant folding is a local
     * optimization technique only done on basic blocks
     *
     * @param module The module being entered
     *
     * @return True if the module was modified, false otherwise
     */
    bool enter_module([[maybe_unused]] il::Module& module) override { return false; }

    /**
     * @brief Called when exiting a module
     *
     * This is never getting used as the constant folding is a local
     * optimization technique only done on basic blocks
     *
     * @param module The module being exited
     *
     * @return True if the module was modified, false otherwise
     */
    bool exit_module([[maybe_unused]] il::Module& module) override { return false; }

    /**
     * @brief Called when entering a function
     *
     * This is never getting used as the constant folding is a local
     * optimization technique only done on basic blocks
     *
     * @param function The function being entered
     *
     * @return True if the function was modified, false otherwise
     */
    bool enter_function([[maybe_unused]] il::Function& function) override { return false; }

    /**
     * @brief Called when exiting a function
     *
     * This is never getting used as the constant folding is a local
     * optimization technique only done on basic blocks
     *
     * @param function The function being exited
     *
     * @return True if the function was modified, false otherwise
     */
    bool exit_function([[maybe_unused]] il::Function& function) override { return false; }

    /**
     * @brief Called for each basic block in a function
     *
     * @param block The basic block being processed
     *
     * @return True if the block was modified, false otherwise
     */
    bool on_block(il::BasicBlock& block) override;

private:
    /**
     * @brief This will evaluate a cast instruction to its appropriate
     *        result
     *
     * @param instruction The input cast that should get evaluated
     *
     * @return The result of the cast as immediate operand
     */
    [[nodiscard]] static il::Immediate _cast(il::Cast& instruction);

    /**
     * @brief Evaluates a given cast (based on type and expression) and returns
     *        the result as an immediate
     *
     * @param to The target type to which the expression should be casted
     * @param expression The actual expression to be evaluated
     *
     * @return A resulting immediate of the evaluation
     */
    [[nodiscard]] static il::Immediate _evaluate_cast(const sem::Type& to, auto expression);
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
