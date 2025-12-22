#pragma once

#include "arkoi_language/opt/pass.hpp"

namespace arkoi::opt {
/**
 * @brief Optimization pass that cleans up and simplifies the Control Flow Graph.
 *
 * `SimplifyCFG` performs several structural optimizations on the CFG:
 * 1. Removes "proxy" blocks (blocks that only contain an unconditional jump).
 * 2. Merges blocks that are sequentially connected without branches.
 * 3. Removes unreachable basic blocks.
 *
 * @see Pass, il::BasicBlock, il::Function
 */
class SimplifyCFG final : public Pass {
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
     * @brief Initializes simplification state for a function.
     */
    bool enter_function(il::Function& function) override;

    /**
     * @brief Performs cleanup and structural changes after individual blocks are processed.
     *
     * This is where block merging and removal actually occur.
     *
     * @param function The `il::Function` being optimized.
     * @return True if the function structure was modified.
     */
    bool exit_function(il::Function& function) override;

    /**
     * @brief Identifies blocks within a function that are candidates for simplification.
     *
     * Categorizes blocks as "simple" (can be merged) or "proxy" (can be bypassed).
     *
     * @param block The `il::BasicBlock` to analyze.
     * @return False (actual modification happens in `exit_function`).
     */
    bool on_block(il::BasicBlock& block) override;

private:
    /**
     * @brief Bypasses a proxy block, connecting its predecessors directly to its successor.
     *
     * @param function The containing function.
     * @param block The proxy block to remove.
     */
    static void _remove_proxy_block(il::Function& function, il::BasicBlock& block);

    /**
     * @brief Determines if a block is a "proxy" (empty or jump-only).
     *
     * @param block The block to check.
     * @return True if the block is a proxy.
     */
    [[nodiscard]] static bool _is_proxy_block(il::BasicBlock& block);

    /**
     * @brief Merges a block into its unique predecessor.
     *
     * @param function The containing function.
     * @param block The block to be merged and deleted.
     */
    static void _merge_block(il::Function& function, il::BasicBlock& block);

    /**
     * @brief Determines if a block can be merged into its predecessor.
     *
     * A block is mergeable if it has exactly one predecessor and that
     * predecessor has exactly one successor (this block).
     *
     * @param block The block to check.
     * @return True if the block is mergeable.
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
