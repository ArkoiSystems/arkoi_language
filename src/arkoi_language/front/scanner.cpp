#include "arkoi_language/front/scanner.hpp"

#include <iostream>
#include <sstream>

#include "pretty_diagnostics/report.hpp"

using namespace pretty_diagnostics;
using namespace arkoi::front;
using namespace arkoi;

static constexpr size_t SPACE_INDENTATION = 4;

std::vector<Token> Scanner::tokenize() {
    std::vector<Token> tokens;
    if (_source->contents().empty()) return tokens;

    std::istringstream stream(_source->contents());
    while (std::getline(stream, _current_line)) {
        auto leading_spaces = _leading_spaces(_current_line);
        if (leading_spaces % SPACE_INDENTATION != 0) {
            const auto error = InvalidSpacingFormat({ _source, _source->from_coords(_row, 0), _source->from_coords(_row, leading_spaces) });
            _diagnostics.add(error.report());
            leading_spaces -= leading_spaces % SPACE_INDENTATION;
        }

        while (leading_spaces > _indentation) {
            const auto span = Span(
                _source,
                _source->from_coords(_row, _indentation),
                _source->from_coords(_row, _indentation + SPACE_INDENTATION)
            );
            tokens.emplace_back(Token::Type::Indentation, span);

            _indentation += SPACE_INDENTATION;
            _column += SPACE_INDENTATION;
        }

        while (leading_spaces < _indentation) {
            tokens.emplace_back(Token::Type::Dedentation, Span(_source, 0, 0));

            _indentation -= SPACE_INDENTATION;
            _column -= SPACE_INDENTATION;
        }

        while (!_is_eol() && !_current_line.empty()) {
            try {
                auto token = _next_token();
                tokens.push_back(token);
            } catch (const UnexpectedEndOfLine& error) {
                _diagnostics.add(error.report());
                break;
            } catch (const ScannerError& error) {
                _diagnostics.add(error.report());
                _next(1);
            }
        }

        const auto start_location = _source->from_coords(_row, _column);
        const auto end_location = _source->from_coords(_row, _column + 1);
        tokens.emplace_back(Token::Type::Newline, Span(_source, start_location, end_location));

        _column = _indentation;
        _row++;
    }

    while (_indentation) {
        tokens.emplace_back(Token::Type::Dedentation, Span(_source, 0, 0));

        _indentation -= SPACE_INDENTATION;
    }

    tokens.emplace_back(Token::Type::EndOfFile, Span(_source, 0, 0));

    return tokens;
}

Token Scanner::_next_token() {
    while (_try_consume(_is_space)) { }

    const auto current = _current_char();
    if (_is_ident_start(current)) {
        return _lex_identifier();
    }

    if (current == '-' || _is_digit(current)) {
        return _lex_number();
    }

    if (current == '\'') {
        return _lex_char();
    }

    if (current == '#') {
        return _lex_comment();
    }

    return _lex_special();
}

Token Scanner::_lex_comment() {
    const auto start_location = _current_location();

    _consume('#');
    while (!_is_eol()) _next(1);

    return { Token::Type::Comment, { _source, start_location, _current_location() } };
}

Token Scanner::_lex_identifier() {
    const auto start_location = _current_location();

    _consume(_is_ident_start, "_, a-z or A-Z");
    while (_try_consume(_is_ident_inner)) { }

    const auto span = Span(_source, start_location, _current_location());
    if (auto keyword = Token::lookup_keyword(span.substr())) {
        return { *keyword, span };
    }

    return { Token::Type::Identifier, span };
}

Token Scanner::_lex_number() {
    const auto start_location = _current_location();

    if (_try_consume('-') && !_is_digit(_current_char())) {
        return { Token::Type::Minus, { _source, start_location, _current_location() } };
    }

    const auto consumed = _consume(_is_digit, "0-9");
    bool floating;

    if (consumed == '0' && _try_consume('x')) {
        _consume(_is_hex, "0-9, a-f or A-F");

        while (_try_consume(_is_hex)) { }

        floating = _try_consume('.');

        while (_try_consume(_is_hex)) { }

        if (_try_consume(_is_hex_expo)) {
            std::ignore = _try_consume(_is_decimal_sign);

            while (_try_consume(_is_hex));
        }
    } else {
        while (_try_consume(_is_digit)) { }

        floating = _try_consume('.');

        while (_try_consume(_is_digit)) { }

        if (_try_consume(_is_expo)) {
            floating = true;

            std::ignore = _try_consume(_is_decimal_sign);

            while (_try_consume(_is_hex));
        }
    }

    const auto span = Span(_source, start_location, _current_location());
    const auto kind = (floating ? Token::Type::Floating : Token::Type::Integer);
    const auto number = span.substr();

    try {
        if (floating) {
            std::stold(number);
        } else if (number.starts_with("-")) {
            std::stoll(number);
        } else {
            std::stoull(number);
        }
    } catch (const std::out_of_range&) {
        throw NumberOutOfRange(span);
    }

    return { kind, span };
}

Token Scanner::_lex_char() {
    const auto start_location = _current_location();

    _consume('\'');
    std::ignore = _consume(_is_ascii, "'");
    _consume('\'');

    return { Token::Type::Integer, { _source, start_location, _current_location() } };
}

Token Scanner::_lex_special() {
    const auto start_location = _current_location();

    for (size_t length = 2; length > 0; length--) {
        const auto view = _peek(length);
        if (!view.has_value()) continue;

        auto matched = Token::lookup_special(*view);
        if (!matched.has_value()) continue;

        _next(length);

        return { *matched, { _source, start_location, _current_location() } };
    }

    throw UnknownChar(_current_char(), { _source, _source->from_coords(_row, _column), _source->from_coords(_row, _column + 1) });
}

char Scanner::_current_char() const {
    if (_is_eol()) {
        throw UnexpectedEndOfLine({ _source, _source->from_coords(_row, 0), _source->from_coords(_row, _current_line.size()) });
    }

    return _current_line[_column];
}

bool Scanner::_is_eol() const {
    return _column >= _current_line.size();
}

Location Scanner::_current_location() const {
    return _source->from_coords(_row, _column);
}

std::optional<std::string_view> Scanner::_peek(const size_t count) const {
    if (_column + count > _current_line.size()) return std::nullopt;

    return std::string_view{ _current_line.data() + _column, count };
}

void Scanner::_next(const size_t count) {
    _column += count;
}

void Scanner::_consume(const char expected) {
    _consume([&](const char input) { return input == expected; }, std::string(1, expected));
}

char Scanner::_consume(const std::function<bool(char)>& predicate, const std::string& expected) {
    const auto current = _current_char();
    if (_is_eol()) {
        throw UnexpectedEndOfLine({ _source, _source->from_coords(_row, 0), _source->from_coords(_row, _current_line.size()) });
    }

    if (!predicate(current)) {
        throw UnexpectedChar(expected, { _source, _source->from_coords(_row, _column), _source->from_coords(_row, _column + 1) });
    }

    _next(1);

    return current;
}

bool Scanner::_try_consume(const char expected) {
    try {
        _consume(expected);
        return true;
    } catch (const ScannerError&) {
        return false;
    }
}

std::optional<char> Scanner::_try_consume(const std::function<bool(char)>& predicate) {
    try {
        auto consumed = _consume(predicate, "");
        return consumed;
    } catch (const ScannerError&) {
        return std::nullopt;
    }
}

size_t Scanner::_leading_spaces(const std::string& line) {
    size_t count = 0;

    for (const auto& current : line) {
        if (current != ' ') break;
        count++;
    }

    return count;
}

bool Scanner::_is_digit(const char input) {
    return std::isdigit(static_cast<unsigned char>(input));
}

bool Scanner::_is_ident_start(const char input) {
    return std::isalpha(static_cast<unsigned char>(input)) || input == '_';
}

bool Scanner::_is_ident_inner(const char input) {
    return std::isalnum(static_cast<unsigned char>(input)) || input == '_';
}

bool Scanner::_is_ascii(const char input) {
    return static_cast<unsigned char>(input) <= 127;
}

bool Scanner::_is_space(const char input) {
    return std::isspace(static_cast<unsigned char>(input));
}

bool Scanner::_is_hex(const char input) {
    return (input >= '0' && input <= '9') ||
           (input >= 'a' && input <= 'f') ||
           (input >= 'A' && input <= 'F');
}

bool Scanner::_is_hex_expo(const char input) {
    return input == 'p' || input == 'P';
}

bool Scanner::_is_expo(const char input) {
    return input == 'e' || input == 'E';
}

bool Scanner::_is_decimal_sign(const char input) {
    return input == '+' || input == '-';
}

InvalidSpacingFormat::InvalidSpacingFormat(const Span& span) :
    ScannerError(
        Report::Builder()
       .severity(Severity::Error)
       .message("Leading spaces are not a multiple of the expected indentation")
       .code("E1000")
       .label("Invalid indentation spacing", span)
       .build()
    ) { }

UnexpectedEndOfLine::UnexpectedEndOfLine(const Span& span) :
    ScannerError(
        Report::Builder()
       .severity(Severity::Error)
       .message("Unexpectedly reached the end of the line")
       .code("E1001")
       .label("Reached end of the line unexpectedly", span)
       .build()
    ) { }

UnexpectedChar::UnexpectedChar(const std::string& expected, const Span& span) :
    ScannerError(
        Report::Builder()
       .severity(Severity::Error)
       .message("Unexpected character encountered")
       .code("E1002")
       .label("Expected '" + expected + "' but got this instead", span)
       .build()
    ) { }

UnknownChar::UnknownChar(const char got, const Span& span) :
    ScannerError(
        Report::Builder()
       .severity(Severity::Error)
       .message("Unrecognized character '" + std::string(1, got) + "' found in source")
       .code("E1003")
       .label("This character was not expected", span)
       .build()
    ) { }

NumberOutOfRange::NumberOutOfRange(const Span& span) :
    ScannerError(
        Report::Builder()
       .severity(Severity::Error)
       .message("Numeric literal exceeds the 64-bit range")
       .code("E1004")
       .label("This number is out of range", span)
       .build()
    ) { }

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
