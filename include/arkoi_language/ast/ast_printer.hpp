#pragma once

#include <functional>
#include <string>

#include "arkoi_language/ast/nodes.hpp"
#include "arkoi_language/ast/visitor.hpp"

namespace arkoi::ast {
/**
 * @brief Visitor for printing the Abstract Syntax Tree (AST) in a human-readable tree format.
 *
 * This class implements the `Visitor` interface to traverse the AST and generate
 * a visual representation. It uses indentation and box-drawing characters to
 * show the hierarchical structure of the nodes.
 *
 * @see Visitor, Node
 */
class ASTPrinter final : public Visitor {
public:
    /**
     * @brief Constructs an `ASTPrinter` that writes to the provided output stream.
     *
     * @param output The string stream where the tree representation will be accumulated.
     */
    explicit ASTPrinter(std::stringstream& output) :
        _output(output) { }

public:
    /**
     * @brief Entry point for printing the entire AST of a program.
     *
     * This is a static helper method that creates an `ASTPrinter` and starts the
     * visitation process from the root `Program` node.
     *
     * @param program The root `Program` node to be printed.
     * @return A `std::stringstream` containing the full tree representation.
     */
    [[nodiscard]] static std::stringstream print(Program& program);

    /**
     * @brief Helper function to print a child node with appropriate tree indentation.
     *
     * This method manages the `_indentation` state to ensure that child nodes
     * are visually connected to their parent.
     *
     * @param callback A function (usually a lambda) that calls `accept` on the child node.
     * @param is_last Boolean flag indicating if this is the last child in its parent's list.
     */
    void print_child(const std::function<void()>& callback, bool is_last);

    /**
     * @brief Prints a representation of the `Program` node.
     *
     * @param node The `Program` node to visit.
     */
    void visit(Program& node) override;

    /**
     * @brief Prints a representation of the `Function` node.
     *
     * @param node The `Function` node to visit.
     */
    void visit(Function& node) override;

    /**
     * @brief Prints a representation of the `Block` node.
     *
     * @param node The `Block` node to visit.
     */
    void visit(Block& node) override;

    /**
     * @brief Prints a representation of the `Parameter` node.
     *
     * @param node The `Parameter` node to visit.
     */
    void visit(Parameter& node) override;

    /**
     * @brief Prints a representation of the `Immediate` node.
     *
     * @param node The `Immediate` node to visit.
     */
    void visit(Immediate& node) override;

    /**
     * @brief Prints a representation of the `Variable` node.
     *
     * @param node The `Variable` node to visit.
     */
    void visit(Variable& node) override;

    /**
     * @brief Prints a representation of the `Return` node.
     *
     * @param node The `Return` node to visit.
     */
    void visit(Return& node) override;

    /**
     * @brief Prints a representation of the `Identifier` node.
     *
     * @param node The `Identifier` node to visit.
     */
    void visit(Identifier& node) override;

    /**
     * @brief Prints a representation of the `Binary` node.
     *
     * @param node The `Binary` node to visit.
     */
    void visit(Binary& node) override;

    /**
     * @brief Prints a representation of the `Cast` node.
     *
     * @param node The `Cast` node to visit.
     */
    void visit(Cast& node) override;

    /**
     * @brief Prints a representation of the `Assign` node.
     *
     * @param node The `Assign` node to visit.
     */
    void visit(Assign& node) override;

    /**
     * @brief Prints a representation of the `Call` node.
     *
     * @param node The `Call` node to visit.
     */
    void visit(Call& node) override;

    /**
     * @brief Prints a representation of the `If` node.
     *
     * @param node The `If` node to visit.
     */
    void visit(If& node) override;

    /**
     * @brief Returns the output stream containing the formatted tree.
     *
     * @return A constant reference to the internal `std::stringstream`.
     */
    [[nodiscard]] auto& output() const { return _output; }

private:
    std::stringstream& _output;
    std::string _indentation;
};
} // namespace arkoi::ast

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
