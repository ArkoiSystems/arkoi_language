#pragma once

#include <functional>
#include <string>

#include "arkoi_language/ast/nodes.hpp"
#include "arkoi_language/ast/visitor.hpp"

namespace arkoi::ast {
/**
 * @brief Visitor for printing the Abstract Syntax Tree (AST) in a human-readable format
 */
class ASTPrinter final : public Visitor {
public:
    /**
     * @brief Constructs an ASTPrinter using a provided output stream that is written to
     *
     * @param output The output stream to which the AST will be printed
     */
    explicit ASTPrinter(std::stringstream& output) :
        _output(output) { }

public:
    /**
     * @brief Prints the AST of a program to the output stream
     *
     * @param program The program whose AST is to be printed
     *
     * @return An output stream containing the printed AST
     */
    [[nodiscard]] static std::stringstream print(Program& program);

    /**
     * @brief Helper function to print a child node with proper indentation
     *
     * @param callback A callback function to print the child node or do other related
     *                 things
     *
     * @param is_last Whether the child node is the last one in a potential list
     */
    void print_child(const std::function<void()>& callback, bool is_last);

    /**
     * @brief Visits a Program node and prints it in a human-readable format
     *
     * @param node The Program node to visit
     */
    void visit(Program& node) override;

    /**
     * @brief Visits a Function node and prints it in a human-readable format
     *
     * @param node The Function node to visit
     */
    void visit(Function& node) override;

    /**
     * @brief Visits a Block node and prints it in a human-readable format
     *
     * @param node The Block node to visit
     */
    void visit(Block& node) override;

    /**
     * @brief Visits a Parameter node and prints it in a human-readable format
     *
     * @param node The Parameter node to visit
     */
    void visit(Parameter& node) override;

    /**
     * @brief Visits an Immediate node and prints it in a human-readable format
     *
     * @param node The Immediate node to visit
     */
    void visit(Immediate& node) override;

    /**
     * @brief Visits a Variable node and prints it in a human-readable format
     *
     * @param node The Variable node to visit
     */
    void visit(Variable& node) override;

    /**
     * @brief Visits a Return node and prints it in a human-readable format
     *
     * @param node The Return node to visit
     */
    void visit(Return& node) override;

    /**
     * @brief Visits an Identifier node and prints it in a human-readable format
     *
     * @param node The Identifier node to visit
     */
    void visit(Identifier& node) override;

    /**
     * @brief Visits a Binary node and prints it in a human-readable format
     *
     * @param node The Binary node to visit
     */
    void visit(Binary& node) override;

    /**
     * @brief Visits a Cast node and prints it in a human-readable format
     *
     * @param node The Cast node to visit
     */
    void visit(Cast& node) override;

    /**
     * @brief Visits an Assign node and prints it in a human-readable format
     *
     * @param node The Assign node to visit
     */
    void visit(Assign& node) override;

    /**
     * @brief Visits a Call node and prints it in a human-readable format
     *
     * @param node The Call node to visit
     */
    void visit(Call& node) override;

    /**
     * @brief Visits an If node and prints it in a human-readable format
     *
     * @param node The If node to visit
     */
    void visit(If& node) override;

    /**
     * @brief Returns the stream with the final formatted output
     *
     * @return The output stream containing the data
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
