#pragma once

#include "arkoi_language/utils/interference_graph.hpp"
#include "arkoi_language/il/instruction.hpp"
#include "arkoi_language/il/analyses.hpp"
#include "arkoi_language/x86_64/operand.hpp"

namespace arkoi::x86_64 {
/**
 * @brief Performs register allocation for an x86-64 function using graph coloring.
 *
 * `RegisterAllocator` maps virtual IL variables to physical machine registers.
 * It uses an `InterferenceGraph` built from liveness analysis to determine
 * which variables can share registers. If the graph cannot be colored with the
 * available registers, some variables are marked as "spilled" to memory.
 *
 * @see InterferenceGraph, il::InstructionLivenessAnalysis, Register
 */
class RegisterAllocator {
public:
    /**
     * @brief Mapping from virtual IL variables to physical register bases.
     */
    using Mapping = std::unordered_map<il::Variable, Register::Base>;

public:
    /**
     * @brief Constructs a `RegisterAllocator`.
     *
     * @param function The `il::Function` whose variables need allocation.
     */
    explicit RegisterAllocator(il::Function& function) :
        _function(function) { }

    /**
     * @brief Executing the entire register allocation.
     */
    void run();

    /**
     * @brief Returns the successful virtual-to-physical register assignments.
     *
     * @return A constant reference to the `Mapping`.
     */
    [[nodiscard]] auto& assigned() { return _assigned; }

    /**
     * @brief Returns the pre-colored virtual-to-physical register assignments.
     *
     * @return A constant reference to the `Mapping`.
     */
    [[nodiscard]] auto& precolored() { return _precolored; }

    /**
     * @brief Returns the variables that could not be assigned a register.
     *
     * Spilled variables must be handled by the code generator by using
     * stack slots instead of registers.
     *
     * @return A constant reference to the vector of spilled `il::Variable` objects.
     */
    [[nodiscard]] auto& spilled() { return _spilled; }

private:
    void _cleanup();

    /**
     * @brief Re-indexes variables to ensure unique identification during allocation.
     */
    void _renumber();

    /**
     * @brief Constructs the interference graph using liveness analysis results.
     */
    void _build();

    /**
     * @brief Attempts to color the interference graph using available registers.
     */
    void _simplify();

    void _select();

    void _rewrite();

private:
    std::shared_ptr<il::InstructionLivenessAnalysis> _liveness_analysis{ };
    il::DataflowAnalysis<il::InstructionLivenessAnalysis> _analysis{ };
    utils::InterferenceGraph<il::Variable> _graph{ };
    Mapping _assigned{ }, _precolored{ };
    std::vector<il::Variable> _stack{ };
    std::set<il::Variable> _spilled{ };
    il::Function& _function;
};
} // namespace arkoi::x86_64

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
