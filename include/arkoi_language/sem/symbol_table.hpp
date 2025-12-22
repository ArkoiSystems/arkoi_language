#pragma once

#include <memory>
#include <unordered_map>
#include <utility>

#include "arkoi_language/sem/symbol.hpp"

namespace arkoi::sem {
/**
 * @brief Represents a symbol table for scope-based symbol management
 */
class SymbolTable {
public:
    /**
     * @brief Constructs a SymbolTable with an optional parent symbol
     *
     * @param parent The parent symbol table (optional)
     */
    explicit SymbolTable(std::shared_ptr<SymbolTable> parent = nullptr) :
        _parent(std::move(parent)) { }

    /**
     * @brief Inserts a new symbol into the table with the given parameters
     *
     * @tparam Type The type of the symbol
     * @tparam Args The types of arguments for the symbol constructor
     * @param name The name of the symbol
     * @param args The arguments for the symbol constructor
     *
     * @return A reference to the newly inserted symbol
     */
    template <typename Type, typename... Args>
    std::shared_ptr<Symbol>& insert(const std::string& name, Args&&... args);

    /**
     * @brief Looks up a symbol by name in the current and parent tables
     *
     * @tparam Types The types of symbols to look for
     * @param name The name of the symbol to lookup
     *
     * @return A reference to the found symbol
     */
    template <typename... Types>
    [[nodiscard]] std::shared_ptr<Symbol>& lookup(const std::string& name);

private:
    std::unordered_map<std::string, std::shared_ptr<Symbol>> _symbols{ };
    std::shared_ptr<SymbolTable> _parent;
};

/**
 * @brief Exception thrown when an identifier is already taken in the current scope
 */
class IdentifierAlreadyTaken final : public std::runtime_error {
public:
    /**
     * @brief Constructs an IdentifierAlreadyTaken exception
     *
     * @param name The name of the identifier
     */
    explicit IdentifierAlreadyTaken(const std::string& name) :
        std::runtime_error("The identifier " + name + " is already taken.") { }
};

/**
 * @brief Exception thrown when an identifier is not found in the current or parent scopes
 */
class IdentifierNotFound final : public std::runtime_error {
public:
    /**
     * @brief Constructs an IdentifierNotFound exception
     *
     * @param name The name of the identifier
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
