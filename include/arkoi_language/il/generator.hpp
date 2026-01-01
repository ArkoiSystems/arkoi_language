#pragma once

#include "arkoi_language/il/cfg.hpp"
#include "arkoi_language/il/instruction.hpp"
#include "arkoi_language/sem/type.hpp"

namespace arkoi::il {
/**
 * @brief Visitor for generating the Intermediate Language (IL) from an AST.
 *
 * `Generator` traverses the Abstract Syntax Tree and emits a linear Three-Address
 * Code representation organized into a Control Flow Graph. It manages the
 * creation of basic blocks, temporary variables, and stack allocations.
 *
 * @see ast::Visitor, Module, Function, BasicBlock
 */
class Generator final : ast::Visitor {
private:
    Generator() = default;

public:
    /**
     * @brief Transforms an AST program into its equivalent IL representation.
     *
     * This is the main entry point for IL generation.
     *
     * @param node The root `ast::Program` node to convert.
     * @return A `Module` containing the generated functions and code.
     */
    [[nodiscard]] static Module generate(ast::Program& node);

    /**
     * @brief Returns the compilation module being populated.
     *
     * @return A reference to the internal `Module`.
     */
    [[nodiscard]] auto& module() { return _module; }

private:
    /**
     * @brief Processes the global program structure.
     */
    void visit(ast::Program& node) override;

    /**
     * @brief Processes a function definition and its body.
     */
    void visit(ast::Function& node) override;

    /**
     * @brief Processes a block of statements, creating a new lexical scope in the IL.
     */
    void visit(ast::Block& node) override;

    /**
     * @brief Function parameters are handled by `visit(ast::Function)`.
     */
    void visit(ast::Parameter&) override { }

    /**
     * @brief Processes a literal value and stores it in `_current_operand`.
     */
    void visit(ast::Immediate& node) override;

    /**
     * @brief Specialized handler for integer literals.
     */
    void visit_integer(const ast::Immediate& node);

    /**
     * @brief Specialized handler for floating-point literals.
     */
    void visit_floating(const ast::Immediate& node);

    /**
     * @brief Specialized handler for boolean literals.
     */
    void visit_boolean(const ast::Immediate& node);

    /**
     * @brief Processes a variable declaration, emitting `Alloca` if needed.
     */
    void visit(ast::Variable& node) override;

    /**
     * @brief Processes a return statement, emitting an `il::Return` instruction.
     */
    void visit(ast::Return& node) override;

    /**
     * @brief Processes an identifier, resolving it to a variable or memory location.
     */
    void visit(ast::Identifier& node) override;

    /**
     * @brief Processes a binary operation, emitting an `il::Binary` instruction.
     */
    void visit(ast::Binary& node) override;

    /**
     * @brief Processes a type cast, emitting an `il::Cast` instruction.
     */
    void visit(ast::Cast& node) override;

    /**
     * @brief Processes an assignment, emitting a `Store` or `Constant` assignment.
     */
    void visit(ast::Assign& node) override;

    /**
     * @brief Processes a function call, emitting an `il::Call` instruction.
     */
    void visit(ast::Call& node) override;

    /**
     * @brief Processes an if-else statement, creating labels and branches in the CFG.
     */
    void visit(ast::If& node) override;

    /**
     * @brief Processes a while statement, creating labels and branches in the CFG.
     */
    void visit(ast::While& node) override;

    /**
     * @brief Generates a unique symbolic label name (e.g., "L1").
     *
     * @return A new unique label string.
     */
    [[nodiscard]] std::string _make_label_symbol();

    /**
     * @brief Creates a unique temporary variable (prefixed with "$").
     *
     * @param type The semantic type of the temporary.
     * @return A new `Variable` instance.
     */
    [[nodiscard]] Variable _make_temporary(const sem::Type& type);

    /**
     * @brief Allocates a new unique memory location.
     *
     * @param type The semantic type of the value to be stored.
     * @return A new `Memory` instance.
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
