#pragma once

#include <sstream>

#include "arkoi_language/il/visitor.hpp"

namespace arkoi::il {
/**
 * @brief Visitor for printing the Intermediate Language (IL) in a human-readable text format.
 *
 * `ILPrinter` traverses the IL structures and instructions to generate a
 * textual representation suitable for debugging or analysis.
 *
 * @see Visitor, Module, Function, BasicBlock
 */
class ILPrinter final : public Visitor {
public:
    /**
     * @brief Constructs an `ILPrinter` that writes to the provided output stream.
     *
     * @param output The string stream where the IL text will be accumulated.
     */
    explicit ILPrinter(std::ostream& output) :
        _output(output) { }

    /**
     * @brief Prints a representation of the entire module.
     */
    void visit(Module& module) override;

    /**
     * @brief Prints a representation of a function, including signature and blocks.
     */
    void visit(Function& function) override;

    /**
     * @brief Prints a representation of a basic block and its instructions.
     */
    void visit(BasicBlock& block) override;

    /**
     * @brief Prints a `Return` instruction.
     */
    void visit(Return& instruction) override;

    /**
     * @brief Prints a `Binary` operation instruction.
     */
    void visit(Binary& instruction) override;

    /**
     * @brief Prints a `Cast` instruction.
     */
    void visit(Cast& instruction) override;

    /**
     * @brief Prints a representation of a call argument.
     */
    void visit(Argument& argument) override;

    /**
     * @brief Prints a `Call` instruction.
     */
    void visit(Call& instruction) override;

    /**
     * @brief Prints a `Goto` instruction.
     */
    void visit(Goto& instruction) override;

    /**
     * @brief Prints an `If` instruction.
     */
    void visit(If& instruction) override;

    /**
     * @brief Prints an `Alloca` instruction.
     */
    void visit(Alloca& instruction) override;

    /**
     * @brief Prints a `Store` instruction.
     */
    void visit(Store& instruction) override;

    /**
     * @brief Prints a `Load` instruction.
     */
    void visit(Load& instruction) override;

    /**
     * @brief Prints a `Constant` assignment instruction.
     */
    void visit(Constant& instruction) override;

private:
    std::ostream& _output;
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
