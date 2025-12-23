#pragma once

#include <memory>
#include <unordered_map>
#include <utility>

#include "arkoi_language/sem/symbol.hpp"

namespace arkoi::sem {
/**
 * @brief Manages symbols within a specific lexical scope.
 *
 * `SymbolTable` implements a hierarchical symbol mapping. It stores symbols
 * defined in the current scope and maintains a pointer to a parent scope
 * to allow for name resolution in outer scopes (lexical nesting).
 *
 * @see Symbol, Function, Variable
 */
class SymbolTable {
public:
    /**
     * @brief Constructs a `SymbolTable` with an optional parent table.
     *
     * @param parent A shared pointer to the enclosing scope's symbol table.
     */
    explicit SymbolTable(std::shared_ptr<SymbolTable> parent = nullptr) :
        _parent(std::move(parent)) { }

    /**
     * @brief Creates and inserts a new symbol into the current scope.
     *
     * @tparam Type The concrete symbol type (e.g., `Variable`, `Function`).
     * @tparam Args Argument types for the symbol's constructor.
     * @param name The name identifier for the symbol.
     * @param args Arguments to pass to the symbol constructor.
     * @return A shared pointer to the newly created `Symbol`.
     * @throws IdentifierAlreadyTaken if @p name already exists in the current scope.
     */
    template <typename Type, typename... Args>
    std::shared_ptr<Symbol>& insert(const std::string& name, Args&&... args);

    /**
     * @brief Resolves a symbol by name, searching current and parent scopes.
     *
     * This method implements lexical scoping rules.
     *
     * @tparam Types Optional filter for allowed symbol types.
     * @param name The name identifier to search for.
     * @return A reference to the shared pointer of the found `Symbol`.
     * @throws IdentifierNotFound if the symbol is not found in any accessible scope.
     */
    template <typename... Types>
    [[nodiscard]] std::shared_ptr<Symbol>& lookup(const std::string& name);

private:
    std::unordered_map<std::string, std::shared_ptr<Symbol>> _symbols{ };
    std::shared_ptr<SymbolTable> _parent;
};

/**
 * @brief Exception thrown when attempting to redefine an identifier in the same scope.
 */
class IdentifierAlreadyTaken final : public std::runtime_error {
public:
    /**
     * @brief Constructs an `IdentifierAlreadyTaken` exception.
     *
     * @param name The name of the conflicting identifier.
     */
    explicit IdentifierAlreadyTaken(const std::string& name) :
        std::runtime_error("The identifier " + name + " is already taken.") { }
};

/**
 * @brief Exception thrown when an identifier cannot be resolved.
 */
class IdentifierNotFound final : public std::runtime_error {
public:
    /**
     * @brief Constructs an `IdentifierNotFound` exception.
     *
     * @param name The name of the missing identifier.
     */
    explicit IdentifierNotFound(const std::string& name) :
        std::runtime_error("The identifier " + name + " was not found.") { }
};

#include "../../../src/arkoi_language/sem/symbol_table.tpp"
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
