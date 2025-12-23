#pragma once

#include <memory>
#include <vector>

#include "arkoi_language/il/cfg.hpp"

namespace arkoi::opt {
/**
 * @brief Base class for Intermediate Language (IL) optimization passes.
 *
 * A `Pass` provides hooks that are called during the traversal of the IL
 * hierarchy. Passes can modify the IL (e.g., folding constants, removing
 * dead code) and must return true if any changes were made.
 *
 * @see PassManager, il::Module, il::Function, il::BasicBlock
 */
class Pass {
public:
    virtual ~Pass() = default;

    /**
     * @brief Hook called when starting the traversal of a module.
     *
     * @param module The `il::Module` being entered.
     * @return True if the pass modified the module, false otherwise.
     */
    virtual bool enter_module(il::Module& module) = 0;

    /**
     * @brief Hook called after traversing all functions in a module.
     *
     * @param module The `il::Module` being exited.
     * @return True if the pass modified the module, false otherwise.
     */
    virtual bool exit_module(il::Module& module) = 0;

    /**
     * @brief Hook called when starting the traversal of a function.
     *
     * @param function The `il::Function` being entered.
     * @return True if the pass modified the function, false otherwise.
     */
    virtual bool enter_function(il::Function& function) = 0;

    /**
     * @brief Hook called after traversing all blocks in a function.
     *
     * @param function The `il::Function` being exited.
     * @return True if the pass modified the function, false otherwise.
     */
    virtual bool exit_function(il::Function& function) = 0;

    /**
     * @brief Hook called for every basic block in the current function.
     *
     * @param block The `il::BasicBlock` being processed.
     * @return True if the pass modified the block, false otherwise.
     */
    virtual bool on_block(il::BasicBlock& block) = 0;
};

/**
 * @brief Orchestrates the execution of multiple optimization passes.
 *
 * `PassManager` stores a sequence of passes and applies them in order
 * to an IL module.
 */
class PassManager {
public:
    /**
     * @brief Runs all registered optimization passes on the module.
     *
     * Passes are executed in the order they were added to the manager.
     *
     * @param module The `il::Module` to optimize.
     */
    void run(il::Module& module) const;

    /**
     * @brief Registers a new optimization pass.
     *
     * @tparam Type The concrete class of the pass (must inherit from `Pass`).
     * @tparam Args Argument types for the pass constructor.
     * @param args Arguments to instantiate the pass.
     */
    template <typename Type, typename... Args>
    void add(Args&&... args);

private:
    std::vector<std::unique_ptr<Pass>> _passes;
};

#include "../../../src/arkoi_language/opt/pass.tpp"
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
