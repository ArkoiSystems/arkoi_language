#pragma once

#include <stack>

#include "arkoi_language/ast/visitor.hpp"
#include "arkoi_language/front/token.hpp"
#include "arkoi_language/sem/symbol_table.hpp"

namespace arkoi::sem {
/**
 * @brief Visitor that performs name resolution on the Abstract Syntax Tree (AST).
 *
 * `NameResolver` traverses the AST to associate every `Identifier` node with a
 * specific `Symbol` from the hierarchical `SymbolTable`. It identifies errors
 * such as duplicate definitions in the same scope or references to undefined
 * variables/functions.
 *
 * @see ast::Visitor, SymbolTable, Symbol, TypeResolver
 */
class NameResolver final : ast::Visitor {
private:
    /**
     * @brief Constructs a `NameResolver`.

     * Private to enforce usage through the static @ref resolve entry point.
     */
    NameResolver() = default;

public:
    /**
     * @brief Performs name resolution on an entire AST program.

     * @param node The root `ast::Program` node to resolve.
     * @return A `NameResolver` instance containing the result state.
     */
    [[nodiscard]] static NameResolver resolve(ast::Program& node);

    /**
     * @brief Indicates whether any name resolution errors were encountered.

     * @return True if one or more resolution errors occurred, false otherwise.
     */
    [[nodiscard]] auto has_failed() const { return _failed; }

private:
    /**
    * @brief Resolves names in the global program scope.
     *
     * @param node The `ast::Program` node to visit.
     */
    void visit(ast::Program& node) override;

    /**
     * @brief First-pass visitor for function prototypes to allow forward references.

     * @param node The `ast::Function` node to register.
     */
    void visit_as_prototype(ast::Function& node);

    /**
     * @brief Resolves names within a function definition.
     *
     * @param node The `ast::Function` node to visit.
     */
    void visit(ast::Function& node) override;

    /**
     * @brief Resolves names within a nested scope block.
     *
     * @param node The `ast::Block` node to visit.
     */
    void visit(ast::Block& node) override;

    /**
     * @brief Resolves names for function parameters.
     *
     * @param node The `ast::Parameter` node to visit.
     */
    void visit(ast::Parameter& node) override;

    /**
     * @brief Associates an identifier node with its corresponding symbol.
     *
     * @param node The `ast::Identifier` node to visit.
     */
    void visit(ast::Identifier& node) override;

    /**
     * @brief Literals do not contain names to resolve.
     *
     * @param node The `ast::Immediate` node to visit.
     */
    void visit([[maybe_unused]] ast::Immediate& node) override { }

    /**
     * @brief Resolves names within a variable declaration.
     *
     * @param node The `ast::Variable` node to visit.
     */
    void visit(ast::Variable& node) override;

    /**
     * @brief Resolves names within a return statement.
     *
     * @param node The `ast::Return` node to visit.
     */
    void visit(ast::Return& node) override;

    /**
     * @brief Resolves names within a binary operation.
     *
     * @param node The `ast::Binary` node to visit.
     */
    void visit(ast::Binary& node) override;

    /**
     * @brief Resolves names within a type cast.
     *
     * @param node The `ast::Cast` node to visit.
     */
    void visit(ast::Cast& node) override;

    /**
     * @brief Resolves names within an assignment.
     *
     * @param node The `ast::Assign` node to visit.
     */
    void visit(ast::Assign& node) override;

    /**
     * @brief Resolves names within a function call.
     *
     * @param node The `ast::Call` node to visit.
     */
    void visit(ast::Call& node) override;

    /**
     * @brief Resolves names within an if-else statement.
     *
     * @param node The `ast::If` node to visit.
     */
    void visit(ast::If& node) override;

    /**
     * @brief Resolves names within a while statement.
     *
     * @param node The `ast::While` node to visit.
     */
    void visit(ast::While& node) override;

    /**
     * @brief Validates that a name is NOT yet defined in the current scope.
     *
     * @tparam Type The symbol kind to create.
     * @tparam Args Argument types for the symbol constructor.
     * @param token The identifier token.
     * @param args Arguments for the symbol.
     * @return The created `Symbol` shared pointer, or nullptr on failure.
     */
    template <typename Type, typename... Args>
    [[nodiscard]] std::shared_ptr<Symbol> _check_non_existence(const front::Token& token, Args&&... args);

    /**
     * @brief Validates that a name IS defined in an accessible scope.
     *
     * @tparam Types Allowed symbol kinds.
     * @param token The identifier token.
     * @return The found `Symbol` shared pointer, or nullptr on failure.
     */
    template <typename... Types>
    [[nodiscard]] std::shared_ptr<Symbol> _check_existence(const front::Token& token);

private:
    std::stack<std::shared_ptr<SymbolTable>> _scopes{ };
    bool _failed{ };
};

#include "../../../src/arkoi_language/sem/name_resolver.tpp"
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
