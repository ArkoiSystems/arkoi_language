#pragma once

#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "arkoi_language/front/token.hpp"

namespace arkoi::front {
/**
 * @brief Scanner for the arkoi language
 */
class Scanner {
public:
    /**
     * @brief Constructs a Scanner with the given source
     *
     * @param source The source code to tokenize
     */
    explicit Scanner(const std::shared_ptr<pretty_diagnostics::Source>& source) :
        _source(source) { }

    /**
     * @brief Tokenizes the source code
     *
     * @return A vector of tokens
     */
    [[nodiscard]] std::vector<Token> tokenize();

    /**
     * @brief Checks if scanning has failed
     *
     * @return True if scanning failed, false otherwise
     */
    [[nodiscard]] auto has_failed() const { return _failed; }

private:
    /**
     * @brief Lexes the next token from the source code
     *
     * @return The next parsed Token
     */
    [[nodiscard]] Token _next_token();

    /**
     * @brief Lexes a comment
     *
     * @return The Comment token
     */
    [[nodiscard]] Token _lex_comment();

    /**
     * @brief Lexes an identifier or keyword
     *
     * @return The Identifier or Keyword token
     */
    [[nodiscard]] Token _lex_identifier();

    /**
     * @brief Lexes a numeric literal (integer or floating point)
     *
     * @return The numeric token
     */
    [[nodiscard]] Token _lex_number();

    /**
     * @brief Lexes a single character
     *
     * @return The token representing the character
     */
    [[nodiscard]] Token _lex_char();

    /**
     * @brief Lexes special characters and multi-character operators
     *
     * @return The special character or operator token
     */
    [[nodiscard]] Token _lex_special();

    /**
     * @brief Returns the character at the current scanner position
     *
     * @return The current character
     */
    [[nodiscard]] char _current_char() const;

    /**
     * @brief Checks if the scanner has reached the end of the current line
     *
     * @return True if at end of line, false otherwise
     */
    [[nodiscard]] bool _is_eol() const;

    /**
     * @brief Returns the current source location (row, column)
     *
     * @return The current Location
     */
    [[nodiscard]] pretty_diagnostics::Location _current_location() const;

    /**
     * @brief Advances the scanner to the next character
     */
    void _next();

    /**
     * @brief Consumes a character if it matches the expected character, otherwise throws UnexpectedChar
     *
     * @param expected The expected character
     */
    void _consume(char expected);

    /**
     * @brief Consumes a character if it matches a predicate, otherwise throws UnexpectedChar
     *
     * @param predicate The condition to check
     * @param expected A string describing what was expected (for error messages)
     *
     * @return The consumed character
     */
    char _consume(const std::function<bool(char)>& predicate, const std::string& expected);

    /**
     * @brief Tries to consume a character if it matches the expected character
     *
     * @param expected The expected character
     *
     * @return True if consumed, false otherwise
     */
    [[nodiscard]] bool _try_consume(char expected);

    /**
     * @brief Tries to consume a character if it matches a predicate
     *
     * @param predicate The condition to check
     *
     * @return The consumed character if the predicate matches, std::nullopt otherwise
     */
    [[nodiscard]] std::optional<char> _try_consume(const std::function<bool(char)>& predicate);

    /**
     * @brief Counts leading spaces in a line to determine indentation level
     *
     * @param line The line of source code
     *
     * @return The number of leading spaces
     */
    [[nodiscard]] static size_t _leading_spaces(const std::string& line);

    /**
     * @brief Checks if a character is a decimal digit
     *
     * @param input The character to check
     *
     * @return True if a digit, false otherwise
     */
    [[nodiscard]] static bool _is_digit(char input);

    /**
     * @brief Checks if a character can start an identifier
     *
     * @param input The character to check
     *
     * @return True if a valid starting character, false otherwise
     */
    [[nodiscard]] static bool _is_ident_start(char input);

    /**
     * @brief Checks if a character can be part of an identifier (after the first character)
     *
     * @param input The character to check
     *
     * @return True if a valid inner character, false otherwise
     */
    [[nodiscard]] static bool _is_ident_inner(char input);

    /**
     * @brief Checks if a character is a valid ASCII character
     *
     * @param input The character to check
     *
     * @return True if ASCII, false otherwise
     */
    [[nodiscard]] static bool _is_ascii(char input);

    /**
     * @brief Checks if a character is a whitespace character (space or tab)
     *
     * @param input The character to check
     *
     * @return True if space, false otherwise
     */
    [[nodiscard]] static bool _is_space(char input);

    /**
     * @brief Checks if a character is a hexadecimal digit
     *
     * @param input The character to check
     *
     * @return True if a hex digit, false otherwise
     */
    [[nodiscard]] static bool _is_hex(char input);

    /**
     * @brief Checks if a character is a hexadecimal exponent marker
     *
     * @param input The character to check
     *
     * @return True if a hex exponent marker, false otherwise
     */
    [[nodiscard]] static bool _is_hex_expo(char input);

    /**
     * @brief Checks if a character is a decimal exponent marker
     *
     * @param input The character to check
     *
     * @return True if an exponent marker, false otherwise
     */
    [[nodiscard]] static bool _is_expo(char input);

    /**
     * @brief Checks if a character is a decimal sign (+ or -)
     *
     * @param input The character to check
     *
     * @return True if a sign character, false otherwise
     */
    [[nodiscard]] static bool _is_decimal_sign(char input);

private:
    std::shared_ptr<pretty_diagnostics::Source> _source;
    size_t _row{ }, _column{ }, _indentation{ };
    std::string _current_line;
    bool _failed{ };
};

/**
 * @brief Base class for all scanner-related errors
 */
class ScannerError : public std::runtime_error {
public:
    explicit ScannerError(const std::string& error) :
        std::runtime_error(error) { }
};

/**
 * @brief Exception thrown when the scanner unexpectedly reaches the end of a line
 */
class UnexpectedEndOfLine final : public ScannerError {
public:
    UnexpectedEndOfLine() :
        ScannerError("Unexpectedly reached the End Of Line") { }
};

/**
 * @brief Exception thrown when an unexpected character is encountered
 */
class UnexpectedChar final : public ScannerError {
public:
    UnexpectedChar(const std::string& expected, char got) :
        ScannerError("Expected " + expected + " but got " + std::string(1, got)) { }
};

/**
 * @brief Exception thrown when an unknown or invalid character is encountered
 */
class UnknownChar final : public ScannerError {
public:
    explicit UnknownChar(char got) :
        ScannerError("Didn't expect " + std::string(1, got)) { }
};

/**
 * @brief Exception thrown when a numeric literal exceeds the supported range
 */
class NumberOutOfRange final : public ScannerError {
public:
    explicit NumberOutOfRange(const std::string& number) :
        ScannerError("The number " + number + " exceeds the 64bit limitations.") { }
};
} // namespace arkoi::front

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
