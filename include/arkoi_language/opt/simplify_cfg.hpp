#pragma once

#include "arkoi_language/opt/pass.hpp"

namespace arkoi::opt {
class SimplifyCFG final : public Pass {
public:
    /**
     * @brief Called when entering a module
     *
     * This is never getting used by the CFG simplifier
     *
     * @param module The module being entered
     *
     * @return True if the module was modified, false otherwise
     */
    bool enter_module([[maybe_unused]] il::Module& module) override { return false; }

    /**
     * @brief Called when exiting a module
     *
     * This is never getting used by the CFG simplifier
     *
     * @param module The module being exited
     *
     * @return True if the module was modified, false otherwise
     */
    bool exit_module([[maybe_unused]] il::Module& module) override { return false; }

    /**
     * @brief Called when entering a function
     *
     * @param function The function being entered
     *
     * @return True if the function was modified, false otherwise
     */
    bool enter_function(il::Function& function) override;

    /**
     * @brief Called when exiting a function
     *
     * @param function The function being exited
     *
     * @return True if the function was modified, false otherwise
     */
    bool exit_function(il::Function& function) override;

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
     * @brief Removes a proxy block from a function and connects predecessors
     *        with the basic block the proxy is referring to
     *
     * @param function The function in which the block is located in
     *
     * @param block The basic block, which should get removed
     */
    static void _remove_proxy_block(il::Function& function, il::BasicBlock& block);

    /**
     * @brief Checks if a block is a proxy or not
     *
     * A proxy block is a basic block that contains no or only unconditional
     * instructions and therefore is just a proxy link between two blocks.
     *
     * @param block The block to be checked
     *
     * @return Whether the block is a proxy or not
     */
    [[nodiscard]] static bool _is_proxy_block(il::BasicBlock& block);

    /**
     * @brief Merges the given basic block with its predecessor and deletes
     *        it right after
     *
     * @param function The function in which the block is located in
     *
     * @param block The basic block which should get merged with it's predecessor
     */
    static void _merge_block(il::Function& function, il::BasicBlock& block);

    /**
     * @brief Checks if the block is simple or not
     *
     * A simple block is a basic block that has no other branches,
     * only one predecessor and ends with a unconditional goto
     *
     * @param block The block to be checked
     *
     * @return Whether the block is simple or not
     */
    [[nodiscard]] static bool is_simple_block(il::BasicBlock& block);

private:
    std::unordered_set<il::BasicBlock*> _simple_blocks;
    std::unordered_set<il::BasicBlock*> _proxy_blocks;
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
