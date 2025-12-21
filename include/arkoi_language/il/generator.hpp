#pragma once

#include "arkoi_language/il/cfg.hpp"
#include "arkoi_language/il/instruction.hpp"
#include "arkoi_language/sem/type.hpp"

namespace arkoi::il {
/**
 * @brief Visitor for generating Intermediate Language (IL) from an AST
 */
class Generator final : ast::Visitor {
private:
    Generator() = default;

public:
    /**
     * @brief Generates an IL Module from an AST Program
     *
     * @param node The AST Program node
     *
     * @return The generated IL Module
     */
    [[nodiscard]] static Module generate(ast::Program& node);

    /**
     * @brief Visits a Program node
     *
     * @param node The Program node to visit
     */
    void visit(ast::Program& node) override;

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
     * @brief Visits a Parameter node (empty implementation)
     */
    void visit(ast::Parameter&) override { }

    /**
     * @brief Visits an Immediate node
     *
     * @param node The Immediate node to visit
     */
    void visit(ast::Immediate& node) override;

    /**
     * @brief Visits an integer literal
     *
     * @param node The Immediate node
     */
    void visit_integer(const ast::Immediate& node);

    /**
     * @brief Visits a floating-point literal
     *
     * @param node The Immediate node
     */
    void visit_floating(const ast::Immediate& node);

    /**
     * @brief Visits a boolean literal
     *
     * @param node The Immediate node
     */
    void visit_boolean(const ast::Immediate& node);

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

    /**
     * @brief Returns the generated module
     *
     * @return A reference to the module
     */
    [[nodiscard]] auto& module() { return _module; }

private:
    /**
     * @brief Generates a unique label for symbols
     *
     * @return A generated label for symbols
     */
    [[nodiscard]] std::string _make_label_symbol();

    /**
     * @brief Creates a new temporary variable with the given type and
     *        a unique version of the "$" variable.
     *
     * @param type The type of the temporary variable
     *
     * @return The created variable using the given type
     */
    [[nodiscard]] Variable _make_temporary(const sem::Type& type);

    /**
     * @brief Creates a unique new memory location with the given type
     *
     * @param type The type of the memory location
     *
     * @return The created memory using the given type
     */
    [[nodiscard]] Memory _make_memory(const sem::Type& type);

private:
    std::unordered_map<std::shared_ptr<Symbol>, Memory> _allocas{ };
    std::optional<Memory> _return_temp{ };
    size_t _temp_index{ }, _label_index{ };
    Function* _current_function{ };
    BasicBlock* _current_block{ };
    Operand _current_operand{ };
    Module _module;
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
