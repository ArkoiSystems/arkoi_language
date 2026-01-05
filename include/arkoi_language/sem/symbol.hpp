#pragma once

#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "arkoi_language/front/token.hpp"
#include "arkoi_language/sem/type.hpp"

namespace arkoi::sem {
/**
 * @brief Represents a variable declaration in the semantic model.
 *
 * A `Variable` symbol stores the variable's name and its resolved semantic type.
 */
class Variable {
public:
    /**
     * @brief Constructs a `Variable` symbol with an explicit name and type.
     *
     * @param name The name token of the variable.
     * @param type The semantic type of the variable.
     */
    Variable(front::Token name, Type type) :
        _type(type), _name(std::move(name)) { }

    /**
     * @brief Constructs a `Variable` symbol with only a name.
     *
     * The type is expected to be assigned later during analysis.
     *
     * @param name The name of the variable.
     */
    explicit Variable(front::Token name) :
        _name(std::move(name)) { }

    /**
     * @brief Returns the semantic type of the variable.
     *
     * @return A constant reference to the `Type`.
     * @throws std::bad_optional_access if the type has not been set.
     */
    [[nodiscard]] auto& type() const { return _type.value(); }

    /**
     * @brief Sets the semantic type of the variable.
     *
     * @param type The `Type` to associate with this variable.
     */
    void set_type(Type type) { _type = std::move(type); }

    /**
     * @brief Returns the name of the variable.
     *
     * @return A constant reference to the name token.
     */
    [[nodiscard]] auto& name() const { return _name; }

private:
    std::optional<Type> _type{ };
    front::Token _name;
};

/**
 * @brief Represents a function definition in the semantic model.
 *
 * A `Function` symbol stores the function's name, its list of parameter
 * symbols, and its return type.
 */
class Function {
public:
    /**
     * @brief Constructs a `Function` symbol.
     *
     * @param name The name token of the function.
     */
    explicit Function(front::Token name) :
        _name(std::move(name)) { }

    /**
     * @brief Returns the symbols representing the function's parameters.
     *
     * @return A constant reference to the vector of parameter-shared pointers.
     */
    [[nodiscard]] auto& parameters() const { return _parameters; }

    /**
     * @brief Sets the parameter symbols for this function.
     *
     * @param symbols A vector of shared pointers to `Variable` symbols.
     */
    void set_parameters(std::vector<std::shared_ptr<Variable>>&& symbols) { _parameters = std::move(symbols); }

    /**
     * @brief Returns the name of the function.
     *
     * @return A constant reference to the name token.
     */
    [[nodiscard]] auto& name() const { return _name; }

    /**
     * @brief Returns the semantic return type of the function.
     *
     * @return A constant reference to the `Type`.
     * @throws std::bad_optional_access if the return type has not been set.
     */
    [[nodiscard]] auto& return_type() const { return _return_type.value(); }

    /**
     * @brief Sets the semantic return type of the function.
     *
     * @param type The `Type` to set as the return type.
     */
    void set_return_type(Type type) { _return_type = std::move(type); }

private:
    std::vector<std::shared_ptr<Variable>> _parameters{ };
    std::optional<Type> _return_type{ };
    front::Token _name;
};

/**
 * @brief A type-safe container for any symbol (Function or Variable).
 *
 * `Symbol` is a `std::variant` that allows uniform handling of different
 * symbol kinds while maintaining type safety.
 */
struct Symbol : std::variant<Function, Variable> {
    using variant::variant;

    /**
     * @brief Returns the name of the symbol.
     *
     * @return A constant reference to the name token.
     */
    [[nodiscard]] const front::Token& name() const;
};
} // namespace arkoi::sem

/**
 * @brief Streams a detailed description of a `Symbol`.
 *
 * @param os The output stream.
 * @param symbol The symbol to describe.
 * @return A reference to the output stream @p os.
 */
std::ostream& operator<<(std::ostream& os, const std::shared_ptr<arkoi::sem::Symbol>& symbol);

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
