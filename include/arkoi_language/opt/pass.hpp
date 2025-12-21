#pragma once

#include <memory>
#include <vector>

#include "arkoi_language/il/cfg.hpp"

namespace arkoi::opt {
/**
 * @brief Base class for optimization passes
 */
class Pass {
public:
    virtual ~Pass() = default;

    /**
     * @brief Called when entering a module
     *
     * @param module The module being entered
     *
     * @return True if the module was modified, false otherwise
     */
    virtual bool enter_module(il::Module& module) = 0;

    /**
     * @brief Called when exiting a module
     *
     * @param module The module being exited
     *
     * @return True if the module was modified, false otherwise
     */
    virtual bool exit_module(il::Module& module) = 0;

    /**
     * @brief Called when entering a function
     *
     * @param function The function being entered
     *
     * @return True if the function was modified, false otherwise
     */
    virtual bool enter_function(il::Function& function) = 0;

    /**
     * @brief Called when exiting a function
     *
     * @param function The function being exited
     *
     * @return True if the function was modified, false otherwise
     */
    virtual bool exit_function(il::Function& function) = 0;

    /**
     * @brief Called for each basic block in a function
     *
     * @param block The basic block being processed
     *
     * @return True if the block was modified, false otherwise
     */
    virtual bool on_block(il::BasicBlock& block) = 0;
};

/**
 * @brief Manager for running a series of optimization passes
 */
class PassManager {
public:
    /**
     * @brief Runs all registered passes on a module
     *
     * @param module The module to optimize
     */
    void run(il::Module& module) const;

    /**
     * @brief Adds a new pass to the manager
     *
     * @tparam Type The type of the pass
     * @tparam Args The types of arguments for the pass constructor
     * @param args The arguments for the pass constructor
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
