#pragma once

#include "pretty_diagnostics/span.hpp"

#include <optional>
#include <utility>

namespace arkoi::front {
/**
 * @brief Represents a single lexical unit (token) in the source code.
 *
 * A `Token` stores its type (e.g., an identifier, a keyword, or an operator)
 * and its location in the source code using a `pretty_diagnostics::Span`.
 */
class Token {
public:
    /**
     * @brief Enumeration of all possible token types in the Arkoi language.
     */
    enum class Type {
        Indentation, ///< Increase in indentation level
        Dedentation, ///< Decrease in indentation level
        Newline,     ///< End of line

        Integer,    ///< Integer literal
        Floating,   ///< Floating-point literal
        Identifier, ///< User-defined name
        Comment,    ///< Source code comment

        // Keywords
        If,     ///< 'if'
        Else,   ///< 'else'
        Fun,    ///< 'fun'
        Return, ///< 'return'
        U8,     ///< 'u8'
        S8,     ///< 's8'
        U16,    ///< 'u16'
        S16,    ///< 's16'
        U32,    ///< 'u32'
        S32,    ///< 's32'
        U64,    ///< 'u64'
        S64,    ///< 's64'
        USize,  ///< 'usize'
        SSize,  ///< 'ssize'
        F32,    ///< 'f32'
        F64,    ///< 'f64'
        Bool,   ///< 'bool'
        True,   ///< 'true'
        False,  ///< 'false'

        // Special characters and operators
        LParent,     ///< '('
        RParent,     ///< ')'
        At,          ///< '@'
        Comma,       ///< ','
        Plus,        ///< '+'
        Minus,       ///< '-'
        Slash,       ///< '/'
        Asterisk,    ///< '*'
        GreaterThan, ///< '>'
        LessThan,    ///< '<'
        Equal,       ///< '='
        Colon,       ///< ':'

        EndOfFile, ///< End of source input
        Unknown,   ///< Unrecognized token
    };

public:
    /**
     * @brief Constructs a `Token`.
     *
     * @param type The category of the token.
     * @param span The precise location of the token in the source.
     */
    Token(const Type type, pretty_diagnostics::Span span) :
        _span(std::move(span)), _type(type) { }

    /**
     * @brief Returns the source code span of the token.
     *
     * @return A constant reference to the `pretty_diagnostics::Span`.
     */
    [[nodiscard]] auto& span() const { return _span; }

    /**
     * @brief Returns the type of the token.
     *
     * @return A constant reference to the `Type`.
     */
    [[nodiscard]] auto& type() const { return _type; }

    /**
     * @brief Determines if a given string is a reserved keyword.
     *
     * @param value The string to check.
     * @return The corresponding `Type` if it's a keyword, or `std::nullopt` otherwise.
     */
    [[nodiscard]] static std::optional<Type> lookup_keyword(const std::string_view& value);

    /**
     * @brief Determines the token type for a single special character.
     *
     * @param value The character to check.
     * @return The corresponding `Type` if recognized, or `std::nullopt` otherwise.
     */
    [[nodiscard]] static std::optional<Type> lookup_special(char value);

private:
    pretty_diagnostics::Span _span;
    Type _type;
};
} // namespace arkoi::front

/**
 * @brief Streams a human-readable name for a `Token::Type`.
 *
 * @param os The output stream.
 * @param type The token type to describe.
 * @return A reference to the output stream @p os
 */
std::ostream& operator<<(std::ostream& os, const arkoi::front::Token::Type& type);

/**
 * @brief Streams a detailed description of a `Token` (type and span).
 *
 * @param os The output stream.
 * @param token The token to describe.
 * @return A reference to the output stream @p os
 */
std::ostream& operator<<(std::ostream& os, const arkoi::front::Token& token);

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
