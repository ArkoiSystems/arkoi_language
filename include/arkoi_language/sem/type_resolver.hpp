#pragma once

#include "arkoi_language/ast/nodes.hpp"
#include "arkoi_language/sem/type.hpp"

namespace arkoi::sem {
/**
 * @brief Performs type resolution on the AST
 */
class TypeResolver final : ast::Visitor {
private:
    /**
     * @brief Construct a TypeResolver
     *
     * Private to enforce usage through the static resolve() entry point
     */
    TypeResolver() = default;

public:
    /**
     * @brief Perform type resolution on a program
     *
     * @param node The program AST to resolve
     *
     * @return A TypeResolver instance containing the resolution result
     */
    [[nodiscard]] static TypeResolver resolve(ast::Program& node);

    /**
     * @brief Check whether type resolution failed
     *
     * @return True if one or more type resolution errors occurred, false otherwise
     */
    [[nodiscard]] auto has_failed() const { return _failed; }

private:
    /**
     * @brief Visits a Program node
     *
     * @param node The Program node to visit
     */
    void visit(ast::Program& node) override;

    /**
     * @brief Visit a Function node as a prototype

     * @param node The Function node to register
     */
    void visit_as_prototype(ast::Function& node);

    /**
     * @brief Visits a Function node
     *
     * @param node The Function node to visit
     */
    void visit(ast::Function& node) override;

    /**
     * @brief Visits a Block node
     *
     * @param node The Block node to visit
     */
    void visit(ast::Block& node) override;

    /**
     * @brief Visits a Parameter node
     *
     * @param node The Parameter node to visit
     */
    void visit(ast::Parameter& node) override;

    /**
     * @brief Visits an Immediate node
     *
     * @param node The Immediate node to visit
     */
    void visit(ast::Immediate& node) override;

    /**
     * @brief Visits an Integer Immediate node
     *
     * @param node The Integer Immediate node to visit
     */
    void visit_integer(ast::Immediate& node);

    /**
     * @brief Visits a Floating immediate node
     *
     * @param node The Floating Immediate node to visit
     */
    void visit_floating(ast::Immediate& node);

    /**
     * @brief Visits a Boolean Immediate node
     *
     * @param node The Boolean Immediate node to visit
     */
    void visit_boolean(ast::Immediate& node);

    /**
     * @brief Visits a Variable node
     *
     * @param node The Variable node to visit
     */
    void visit(ast::Variable& node) override;

    /**
     * @brief Visits a Return node
     *
     * @param node The Return node to visit
     */
    void visit(ast::Return& node) override;

    /**
     * @brief Visits an Identifier node
     *
     * @param node The Identifier node to visit
     */
    void visit(ast::Identifier& node) override;

    /**
     * @brief Visits a Binary node
     *
     * @param node The Binary node to visit
     */
    void visit(ast::Binary& node) override;

    /**
     * @brief Visits a Cast node
     *
     * @param node The Cast node to visit
     */
    void visit(ast::Cast& node) override;

    /**
     * @brief Visits an Assign node
     *
     * @param node The Assign node to visit
     */

    void visit(ast::Assign& node) override;
    /**
     * @brief Visits a Call node
     *
     * @param node The Call node to visit
     */
    void visit(ast::Call& node) override;

    /**
     * @brief Visits an If node
     *
     * @param node The If node to visit
     */
    void visit(ast::If& node) override;

private:
    /**
     * @brief Perform arithmetic type conversion
     *
     * Determines the common result type of an arithmetic operation
     * between two operand types
     *
     * @param left_type The type of the left operand
     * @param right_type The type of the right operand
     *
     * @return The resulting type after arithmetic conversion
     */
    static Type _arithmetic_conversion(const Type& left_type, const Type& right_type);

    /**
     * @brief Check whether an implicit type conversion is allowed
     *
     * Determines whether a value of one type can be implicitly
     * converted to another type without an explicit cast
     *
     * @param from The source type
     * @param destination The target type
     *
     * @return True if the conversion is allowed implicitly, false otherwise
     */
    static bool _can_implicit_convert(const Type& from, const Type& destination);

    /**
     * @brief Insert an explicit cast node into the AST
     *
     * Wraps the given AST node in a cast expression converting it
     * from one type to another
     *
     * @param node The AST node to cast
     * @param from The original type of the node
     * @param to The target type of the cast
     *
     * @return A new AST node representing the cast expression
     */
    static std::unique_ptr<ast::Node> _cast(std::unique_ptr<ast::Node>& node, const Type& from, const Type& to);

private:
    std::optional<Type> _current_type{ }, _return_type{ };
    bool _failed{ };
};
} // namespace arkoi::sem

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
