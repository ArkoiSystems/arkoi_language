#pragma once

#include <functional>
#include <string>
#include <vector>

#include "arkoi_language/front/token.hpp"
#include "arkoi_language/utils/diagnostics.hpp"

namespace arkoi::front {
/**
 * @brief The scanner (lexer) for the Arkoi language.
 *
 * The `Scanner` class is responsible for converting the raw source code string
 * into a stream of `Token` objects. It handles indentation-based scoping by
 * generating `Indentation` and `Dedentation` tokens.
 *
 * @see Token, Parser
 */
class Scanner {
public:
    /**
     * @brief Constructs a `Scanner` for the given source input.
     *
     * @param source A shared pointer to the `pretty_diagnostics::Source` containing the code.
     * @param diagnostics The diagnostic handler for reporting errors.
     */
    Scanner(const std::shared_ptr<pretty_diagnostics::Source>& source, utils::Diagnostics& diagnostics) :
        _source(source), _diagnostics(diagnostics) { }

    /**
     * @brief Performs lexical analysis and returns the full list of tokens.
     *
     * This method traverses the entire source input, identifying keywords,
     * identifiers, literals, and operators. It also tracks indentation levels.
     *
     * @return A `std::vector` of `Token` objects.
     */
    [[nodiscard]] std::vector<Token> tokenize();

private:
    /**
     * @brief Lexes the next individual token from the current position.
     *
     * @return The next parsed `Token`.
     */
    [[nodiscard]] Token _next_token();

    /**
     * @brief Lexes a source code comment (starting with '#').
     *
     * @return A `Token` of type `Comment`.
     */
    [[nodiscard]] Token _lex_comment();

    /**
     * @brief Lexes an identifier or a reserved keyword.
     *
     * @return A `Token` representing either an identifier or a keyword.
     */
    [[nodiscard]] Token _lex_identifier();

    /**
     * @brief Lexes a numeric literal, supporting both integer and floating point.
     *
     * @return A `Token` of type `Integer` or `Floating`.
     */
    [[nodiscard]] Token _lex_number();

    /**
     * @brief Lexes a single character literal or special symbol.
     *
     * @return The corresponding `Token`.
     */
    [[nodiscard]] Token _lex_char();

    /**
     * @brief Lexes special characters and potential multi-character operators.
     *
     * @return The recognized operator or symbol `Token`.
     */
    [[nodiscard]] Token _lex_special();

    /**
     * @brief Returns the character at the current scanning position without advancing.
     *
     * @return The current character.
     */
    [[nodiscard]] char _current_char() const;

    /**
     * @brief Checks if the scanner has reached the end of the current source line.
     *
     * @return True if at end-of-line, false otherwise.
     */
    [[nodiscard]] bool _is_eol() const;

    /**
     * @brief Retrieves the current source location (row and column).
     *
     * @return The current `pretty_diagnostics::Location`.
     */
    [[nodiscard]] pretty_diagnostics::Location _current_location() const;

    /**
     * @brief Returns a view of the next characters from the current column.
     *
     * Returns up to `count` characters from the current position in the current line
     * without advancing the scanner. The view will not go past the end of the line.
     *
     * @param count Number of characters to peek.
     * @return std::string_view of the next characters, or std::nullopt if at the line end.
     */
    [[nodiscard]] std::optional<std::string_view> _peek(size_t count) const;

    /**
     * @brief Advances the internal pointer to the next character in the source.
     *
     * @param count Number of characters to skip.
     */
    void _next(size_t count);

    /**
     * @brief Consumes a character if it matches the @p expected character.
     *
     * @param expected The character that is required at the current position.
     * @throws UnexpectedChar if the current character does not match.
     */
    void _consume(char expected);

    /**
     * @brief Consumes a character if it satisfies the @p predicate.
     *
     * @param predicate A function determining if the character is valid.
     * @param expected A description of what was expected (used for error reporting).
     * @return The character that was consumed.
     * @throws UnexpectedChar if the predicate is not met.
     */
    char _consume(const std::function<bool(char)>& predicate, const std::string& expected);

    /**
     * @brief Attempts to consume @p expected, returning true on success.
     *
     * If the current character matches @p expected, it is consumed and the
     * scanner advances. Otherwise, the scanner remains in the same position.
     *
     * @param expected The character to try and consume.
     * @return True if consumed, false otherwise.
     */
    [[nodiscard]] bool _try_consume(char expected);

    /**
     * @brief Attempts to consume a character matching the @p predicate.
     *
     * @param predicate A function determining if the character is valid.
     * @return The consumed character if it matched, or `std::nullopt` otherwise.
     */
    [[nodiscard]] std::optional<char> _try_consume(const std::function<bool(char)>& predicate);

    /**
     * @brief Calculates the number of leading spaces to determine indentation depth.
     *
     * @param line The source line to analyze.
     * @return The number of leading whitespace characters.
     */
    [[nodiscard]] static size_t _leading_spaces(const std::string& line);

    /**
     * @brief Checks if a character is a decimal digit (0-9).
     *
     * @param input The character to check.
     *
     * @return True if a digit, false otherwise.
     */
    [[nodiscard]] static bool _is_digit(char input);

    /**
     * @brief Checks if a character can start a valid identifier.
     *
     * @param input The character to check.
     *
     * @return True if a valid starting character, false otherwise.
     */
    [[nodiscard]] static bool _is_ident_start(char input);

    /**
     * @brief Checks if a character can be part of an identifier body.
     *
     * @param input The character to check.
     *
     * @return True if a valid inner character, false otherwise.
     */
    [[nodiscard]] static bool _is_ident_inner(char input);

    /**
     * @brief Checks if a character is in the standard ASCII range.
     *
     * @param input The character to check.
     *
     * @return True if ASCII, false otherwise.
     */
    [[nodiscard]] static bool _is_ascii(char input);

    /**
     * @brief Checks if a character is a space or tab.
     *
     * @param input The character to check.
     *
     * @return True if space, false otherwise.
     */
    [[nodiscard]] static bool _is_space(char input);

    /**
     * @brief Checks if a character is a valid hexadecimal digit.
     *
     * @param input The character to check.
     *
     * @return True if a hex digit, false otherwise.
     */
    [[nodiscard]] static bool _is_hex(char input);

    /**
     * @brief Checks if a character is a hexadecimal exponent marker ('p' or 'P').
     *
     * @param input The character to check.
     *
     * @return True if a hex exponent marker, false otherwise.
     */
    [[nodiscard]] static bool _is_hex_expo(char input);

    /**
     * @brief Checks if a character is a decimal exponent marker ('e' or 'E').
     *
     * @param input The character to check.
     *
     * @return True if an exponent marker, false otherwise.
     */
    [[nodiscard]] static bool _is_expo(char input);

    /**
     * @brief Checks if a character is a sign ('+' or '-').
     *
     * @param input The character to check.
     *
     * @return True if a sign character, false otherwise.
     */
    [[nodiscard]] static bool _is_decimal_sign(char input);

private:
    std::shared_ptr<pretty_diagnostics::Source> _source;
    size_t _row{ }, _column{ }, _indentation{ };
    utils::Diagnostics& _diagnostics;
    std::string _current_line;
};

/**
 * @brief Base class for all recoverable scanner (lexical) errors.
 *
 * Stores a diagnostic report describing the lexical error.
 * Thrown to unwind scanning and handled at synchronization points.
 */
class ScannerError : public std::exception {
public:
    /**
     * @brief Constructs a `ScannerError` error.
     *
     * @param report Diagnostic report describing the error.
     */
    explicit ScannerError(pretty_diagnostics::Report report)
        : _report(std::move(report)) { }

    /**
     * @brief Get the diagnostic report associated with this error.
     *
     * @return Reference to the stored diagnostic report.
     */
    [[nodiscard]] auto& report() const { return _report; }

private:
    pretty_diagnostics::Report _report;
};

/**
 * @brief Scanner error indicating invalid indentation spacing.
 *
 * Thrown when the number of leading spaces in a line does not
 * conform to the expected multiple of `SPACE_INDENTATION`.
 */
class InvalidSpacingFormat final : public ScannerError {
public:
    /**
     * @brief Constructs an `InvalidSpacingFormat` error.
     *
     * @param span Source span of the line with incorrect spacing.
     */
    explicit InvalidSpacingFormat(const pretty_diagnostics::Span& span);
};

/**
 * @brief Scanner error indicating an unexpected end of line.
 *
 * Typically fatal in certain lexical contexts.
 */
class UnexpectedEndOfLine final : public ScannerError {
public:
    /**
     * @brief Constructs an `UnexpectedEndOfLine` error.
     *
     * @param span Source span where the end of line occurred unexpectedly.
     */
    explicit UnexpectedEndOfLine(const pretty_diagnostics::Span& span);
};

/**
 * @brief Scanner error indicating an unexpected character.
 *
 * Thrown when a character does not match the expected pattern.
 */
class UnexpectedChar final : public ScannerError {
public:
    /**
     * @brief Constructs an `UnexpectedChar` error.
     *
     * @param expected Description of what was expected.
     * @param span Source span of the unexpected character.
     */
    UnexpectedChar(const std::string& expected, const pretty_diagnostics::Span& span);
};

/**
 * @brief Scanner error indicating an unrecognized character.
 */
class UnknownChar final : public ScannerError {
public:
    /**
     * @brief Constructs an `UnknownChar` error.
     *
     * @param got The unrecognized character.
     * @param span Source span where the character occurred.
     */
    UnknownChar(char got, const pretty_diagnostics::Span& span);
};

/**
 * @brief Scanner error indicating a numeric literal out of range.
 */
class NumberOutOfRange final : public ScannerError {
public:
    /**
     * @brief Constructs a `NumberOutOfRange` error.
     *
     * @param span Source span of the number literal.
     */
    explicit NumberOutOfRange(const pretty_diagnostics::Span& span);
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
