#pragma once

#include "arkoi_language/ast/nodes.hpp"
#include "arkoi_language/sem/type.hpp"

namespace arkoi::sem {
/**
 * @brief Visitor that performs type resolution and inference on the AST.
 *
 * `TypeResolver` traverses the AST to determine the semantic type of every
 * expression and statement. It handles implicit type conversions, validates
 * operand types for operators, and ensures that function return values match
 * their declarations.
 *
 * @see ast::Visitor, Type, NameResolver
 */
class TypeResolver final : ast::Visitor {
private:
    /**
     * @brief Constructs a `TypeResolver`.

     * Private to enforce usage through the static @ref resolve entry point.
     */
    TypeResolver() = default;

public:
    /**
     * @brief Performs type resolution on an entire AST program.

     * @param node The root `ast::Program` node to resolve.
     * @return A `TypeResolver` instance containing the result state.
     */
    [[nodiscard]] static TypeResolver resolve(ast::Program& node);

    /**
     * @brief Indicates whether any type resolution errors were encountered.

     * @return True if one or more typing errors occurred, false otherwise.
     */
    [[nodiscard]] auto has_failed() const { return _failed; }

private:
    /**
     * @brief Resolves types in the global program scope.
     *
     * @param node The `Program` node to visit.
     */
    void visit(ast::Program& node) override;

    /**
     * @brief First-pass visitor for function prototypes.
     *
     * @param node The `Function` node to visit.
     */
    void visit_as_prototype(ast::Function& node);

    /**
     * @brief Resolves types within a function body.
     *
     * @param node The `Function` node to visit.
     */
    void visit(ast::Function& node) override;

    /**
     * @brief Resolves types within a nested block.
     *
     * @param node The `Block` node to visit.
     */
    void visit(ast::Block& node) override;

    /**
     * @brief Resolves types for function parameters.
     *
     * @param node The `Parameter` node to visit.
     */
    void visit(ast::Parameter& node) override;

    /**
     * @brief Resolves the type of a literal expression.
     *
     * @param node The `Immediate` node to visit.
     */
    void visit(ast::Immediate& node) override;

    /**
     * @brief Resolves types for integer literals.
     *
     * @param node The `Immediate` node to visit.
     */
    void visit_integer(ast::Immediate& node);

    /**
     * @brief Resolves types for floating-point literals.
     *
     * @param node The `Immediate` node to visit.
     */
    void visit_floating(ast::Immediate& node);

    /**
     * @brief Resolves types for boolean literals.
     *
     * @param node The `Immediate` node to visit.
     */
    void visit_boolean(ast::Immediate& node);

    /**
     * @brief Resolves types within a variable declaration.
     *
     * @param node The `Variable` node to visit.
     */
    void visit(ast::Variable& node) override;

    /**
     * @brief Resolves types within a return statement.
     *
     * @param node The `Return` node to visit.
     */
    void visit(ast::Return& node) override;

    /**
     * @brief Resolves the type of an identifier reference.
     *
     * @param node The `Identifier` node to visit.
     */
    void visit(ast::Identifier& node) override;

    /**
     * @brief Resolves types for a binary operation, including promotion.
     *
     * @param node The `Binary` node to visit.
     */
    void visit(ast::Binary& node) override;

    /**
     * @brief Resolves types for an explicit type cast.
     *
     * @param node The `Cast` node to visit.
     */
    void visit(ast::Cast& node) override;

    /**
     * @brief Resolves types for an assignment statement.
     *
     * @param node The `Assign` node to visit.
     */
    void visit(ast::Assign& node) override;

    /**
     * @brief Resolves types for a function call, checking arguments.
     *
     * @param node The `Call` node to visit.
     */
    void visit(ast::Call& node) override;

    /**
     * @brief Resolves types within an if-else statement.
     *
     * @param node The `If` node to visit.
     */
    void visit(ast::If& node) override;

    /**
     * @brief Determines the common result type for an operation between two types.
     *
     * @param left_type The type of the left operand.
     * @param right_type The type of the right operand.
     * @return The promoted resulting `Type`.
     */
    static Type _arithmetic_conversion(const Type& left_type, const Type& right_type);

    /**
     * @brief Checks if a source type can be implicitly converted to a target type.
     *
     * @param from The source `Type`.
     * @param destination The target `Type`.
     * @return True if implicit conversion is legal.
     */
    static bool _can_implicit_convert(const Type& from, const Type& destination);

    /**
     * @brief Wraps an AST node in an explicit `ast::Cast` node.
     *
     * @param node The expression `Node` to wrap.
     * @param from The current type of the node.
     * @param to The target type of the cast.
     * @return A unique pointer to the new `ast::Cast` node.
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
