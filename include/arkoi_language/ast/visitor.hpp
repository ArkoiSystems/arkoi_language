#pragma once

namespace arkoi::ast {
class Identifier;
class Parameter;
class Immediate;
class Variable;
class Function;
class Program;
class Assign;
class Return;
class Binary;
class While;
class Block;
class Cast;
class Call;
class If;

/**
 * @brief Base class for the Abstract Syntax Tree (AST) visitors.
 *
 * This class implements the Visitor pattern, allowing for operations to be
 * performed on the AST without modifying the node classes themselves.
 * Common implementations include semantic analysis, code generation, and pretty-printing.
 *
 * Each `visit` method corresponds to a specific `Node` subclass.
 *
 * @see Node
 */
class Visitor {
public:
    virtual ~Visitor() = default;

    /**
     * @brief Visits a `Program` node.
     *
     * @param node The `Program` node representing the top-level unit.
     */
    virtual void visit(Program& node) = 0;

    /**
     * @brief Visits a `Function` node.
     *
     * @param node The `Function` node representing a function definition.
     */
    virtual void visit(Function& node) = 0;

    /**
     * @brief Visits a `Block` node.
     *
     * @param node The `Block` node representing a sequence of statements.
     */
    virtual void visit(Block& node) = 0;

    /**
     * @brief Visits a `Parameter` node.
     *
     * @param node The `Parameter` node representing a function parameter.
     */
    virtual void visit(Parameter& node) = 0;

    /**
     * @brief Visits an `Immediate` node.
     *
     * @param node The `Immediate` node representing a literal value (e.g., integer).
     */
    virtual void visit(Immediate& node) = 0;

    /**
     * @brief Visits a `Variable` node.
     *
     * @param node The `Variable` node representing a variable declaration.
     */
    virtual void visit(Variable& node) = 0;

    /**
     * @brief Visits a `Return` node.
     *
     * @param node The `Return` node representing a return statement.
     */
    virtual void visit(Return& node) = 0;

    /**
     * @brief Visits an `Identifier` node.
     *
     * @param node The `Identifier` node representing a name reference.
     */
    virtual void visit(Identifier& node) = 0;

    /**
     * @brief Visits a `Binary` node.
     *
     * @param node The `Binary` node representing a binary operation (e.g., addition).
     */
    virtual void visit(Binary& node) = 0;

    /**
     * @brief Visits a `Cast` node.
     *
     * @param node The `Cast` node representing a type conversion.
     */
    virtual void visit(Cast& node) = 0;

    /**
     * @brief Visits an `Assign` node.
     *
     * @param node The `Assign` node representing an assignment statement.
     */
    virtual void visit(Assign& node) = 0;

    /**
     * @brief Visits a `Call` node.
     *
     * @param node The `Call` node representing a function call.
     */
    virtual void visit(Call& node) = 0;

    /**
     * @brief Visits an `If` node.
     *
     * @param node The `If` node representing an if-else statement.
     */
    virtual void visit(If& node) = 0;

    /**
     * @brief Visits an `While` node.
     *
     * @param node The `While` node representing a while statement.
     */
    virtual void visit(While& node) = 0;
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
