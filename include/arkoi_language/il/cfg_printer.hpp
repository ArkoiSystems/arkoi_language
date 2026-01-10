#pragma once

#include "arkoi_language/il/analyses.hpp"
#include "arkoi_language/il/cfg.hpp"
#include "arkoi_language/il/dataflow.hpp"
#include "arkoi_language/il/il_printer.hpp"
#include "arkoi_language/il/visitor.hpp"

namespace arkoi::il {
/**
 * @brief Visitor for generating a visual representation of the Control Flow Graph (CFG).
 *
 * `CFGPrinter` outputs the graph in Graphviz DOT format. It includes instruction
 * listings within each block and labels edges to show control flow (jumps and falls).
 * It also optionally displays liveness information if `BlockLivenessAnalysis` is run.
 *
 * @see Visitor, Module, Function, BasicBlock, DataflowAnalysis
 */
class CFGPrinter final : Visitor {
public:
    /**
     * @brief Constructs a `CFGPrinter` that writes to the provided output stream.
     *
     * @param output The string stream where the DOT representation will be accumulated.
     */
    explicit CFGPrinter(std::ostream& output) :
        _current_function(nullptr), _output(output), _printer(output) { }

    /**
     * @brief Starts the DOT graph and visits all functions.
     */
    void visit(Module& module) override;

private:
    /**
     * @brief Generates a subgraph for a single function.
     */
    void visit(Function& function) override;

    /**
     * @brief Generates a node in the DOT graph for a basic block.
     */
    void visit(BasicBlock& block) override;

    /**
     * @brief Delegates instruction printing to the internal `ILPrinter`.
     */
    void visit(Return& instruction) override { _printer.visit(instruction); }

    /**
     * @brief Delegates instruction printing to the internal `ILPrinter`.
     */
    void visit(Binary& instruction) override { _printer.visit(instruction); }

    /**
     * @brief Delegates instruction printing to the internal `ILPrinter`.
     */
    void visit(Cast& instruction) override { _printer.visit(instruction); }

    /**
     * @brief Delegates instruction printing to the internal `ILPrinter`.
     */
    void visit(Argument& instruction) override { _printer.visit(instruction); }

    /**
     * @brief Delegates instruction printing to the internal `ILPrinter`.
     */
    void visit(Call& instruction) override { _printer.visit(instruction); }

    /**
     * @brief Delegates instruction printing to the internal `ILPrinter`.
     */
    void visit(Goto& instruction) override { _printer.visit(instruction); }

    /**
     * @brief Delegates instruction printing to the internal `ILPrinter`.
     */
    void visit(If& instruction) override { _printer.visit(instruction); }

    /**
     * @brief Delegates instruction printing to the internal `ILPrinter`.
     */
    void visit(Alloca& instruction) override { _printer.visit(instruction); }

    /**
     * @brief Delegates instruction printing to the internal `ILPrinter`.
     */
    void visit(Store& instruction) override { _printer.visit(instruction); }

    /**
     * @brief Delegates instruction printing to the internal `ILPrinter`.
     */
    void visit(Load& instruction) override { _printer.visit(instruction); }

    /**
     * @brief Delegates instruction printing to the internal `ILPrinter`.
     */
    void visit(Phi& instruction) override { _printer.visit(instruction); }

    /**
     * @brief Delegates instruction printing to the internal `ILPrinter`.
     */
    void visit(Assign& instruction) override { _printer.visit(instruction); }

private:
    DataflowAnalysis<BlockLivenessAnalysis> _liveness{ };
    Function* _current_function;
    std::ostream& _output;
    ILPrinter _printer;
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
