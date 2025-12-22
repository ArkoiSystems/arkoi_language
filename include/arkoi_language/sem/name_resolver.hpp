#pragma once

#include <stack>

#include "arkoi_language/ast/visitor.hpp"
#include "arkoi_language/front/token.hpp"
#include "arkoi_language/sem/symbol_table.hpp"

namespace arkoi::sem {
/**
 * @brief Performs name resolution on the AST
 */
class NameResolver final : ast::Visitor {
private:
    /**
     * @brief Construct a NameResolver
     *
     * Private to enforce usage through the static resolve() entry point
     */
    NameResolver() = default;

public:
    /**
     * @brief Perform name resolution on a program
     *
     * @param node The program AST to resolve
     *
     * @return A NameResolver instance containing the resolution result
     */
    [[nodiscard]] static NameResolver resolve(ast::Program& node);

    /**
     * @brief Check whether name resolution failed
     *
     * @return True if one or more name resolution errors occurred, false otherwise
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
     * @brief Visits an Identifier node
     *
     * @param node The Identifier node to visit
     */
    void visit(ast::Identifier& node) override;

    /**
     * @brief Visits an Immediate node (empty implementation)
     *
     * @param node The Immediate node to visit
     */
    void visit([[maybe_unused]] ast::Immediate& node) override { }

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

    /**
     * @brief Ensure a symbol does not already exist in the current scope
     *
     * @tparam Type The symbol type to create
     * @tparam Args Constructor argument types
     * @param token The token identifying the symbol
     * @param args Arguments forwarded to the symbol constructor
     *
     * @return The created symbol, or nullptr on failure
     */
    template <typename Type, typename... Args>
    [[nodiscard]] std::shared_ptr<Symbol> _check_non_existence(const front::Token& token, Args&&... args);

    /**
     * @brief Look up an existing symbol in the current scope stack
     *
     * @tparam Types Allowed symbol types
     * @param token The token identifying the symbol
     *
     * @return The resolved symbol, or nullptr on failure
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
