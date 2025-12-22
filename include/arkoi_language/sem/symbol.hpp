#pragma once

#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "arkoi_language/sem/type.hpp"

struct Symbol;

namespace arkoi::sem {
class Variable;

/**
 * @brief Represents a function symbol in the semantic analysis
 */
class Function {
public:
    /**
     * @brief Constructs a Function symbol with the given name
     *
     * @param name The name of the function
     */
    explicit Function(std::string name) :
        _name(std::move(name)) { }

    /**
     * @brief Returns the parameters of the function
     *
     * @return A reference to the vector of parameter symbols
     */
    [[nodiscard]] auto& parameters() const { return _parameters; }

    /**
     * @brief Sets the parameters of the function
     *
     * @param symbols The parameter symbols to set
     */
    void set_parameters(std::vector<std::shared_ptr<Variable>>&& symbols) { _parameters = std::move(symbols); }

    /**
     * @brief Returns the name of the function
     *
     * @return A reference to the name string
     */
    [[nodiscard]] auto& name() const { return _name; }

    /**
     * @brief Returns the return type of the function
     *
     * @return A reference to the return type
     */
    [[nodiscard]] auto& return_type() const { return _return_type.value(); }

    /**
     * @brief Sets the return type of the function
     *
     * @param type The return type to set
     */
    void set_return_type(Type type) { _return_type = std::move(type); }

private:
    std::vector<std::shared_ptr<Variable>> _parameters{ };
    std::optional<Type> _return_type{ };
    std::string _name;
};

/**
 * @brief Represents a variable symbol in the semantic analysis
 */
class Variable {
public:
    /**
     * @brief Constructs a Variable symbol with name and type
     *
     * @param name The name of the variable
     * @param type The type of the variable
     */
    Variable(std::string name, Type type) :
        _type(type), _name(std::move(name)) { }

    /**
     * @brief Constructs a Variable symbol with only a name
     *
     * @param name The name of the variable
     */
    explicit Variable(std::string name) :
        _name(std::move(name)) { }

    /**
     * @brief Returns the type of the variable
     *
     * @return A reference to the type
     */
    [[nodiscard]] auto& type() const { return _type.value(); }

    /**
     * @brief Sets the type of the variable
     *
     * @param type The type to set
     */
    void set_type(Type type) { _type = std::move(type); }

    /**
     * @brief Returns the name of the variable
     *
     * @return A reference to the name string
     */
    [[nodiscard]] auto& name() const { return _name; }

private:
    std::optional<Type> _type{ };
    std::string _name;
};
} // namespace arkoi::sem

/**
 * @brief Represents a generic symbol, which can be a function or a variable
 */
struct Symbol : std::variant<arkoi::sem::Function, arkoi::sem::Variable> {
    using variant::variant;
};

/**
 * @brief Streams a readable description of a `Symbol`
 *
 * @param os Output stream to write to
 * @param symbol Symbol to describe
 *
 * @return Reference to @p os
 */
std::ostream& operator<<(std::ostream& os, const std::shared_ptr<Symbol>& symbol);

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
