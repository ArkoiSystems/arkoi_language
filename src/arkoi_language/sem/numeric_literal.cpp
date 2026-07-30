#include "arkoi_language/sem/numeric_literal.hpp"

#include <cerrno>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <string_view>
#include <sys/types.h>
#include <system_error>
#include <variant>

#include "arkoi_language/utils/utils.hpp"

using namespace arkoi::sem;
using namespace arkoi;

std::string remove_digit_separators(const std::string_view text) {
    std::string result;
    result.reserve(text.size());

    for (const char character : text) {
        if (character != '_') {
            result.push_back(character);
        }
    }

    return result;
}

bool is_hex(std::string_view text) {
    if (text.starts_with("+") || text.starts_with("-")) {
        text.remove_prefix(1);
    }

    return text.starts_with("0x") || text.starts_with("0X");
}

bool is_bin(std::string_view text) {
    if (text.starts_with("+") || text.starts_with("-")) {
        text.remove_prefix(1);
    }

    return text.starts_with("0b") || text.starts_with("0B");
}

bool is_oct(std::string_view text) {
    if (text.starts_with("+") || text.starts_with("-")) {
        text.remove_prefix(1);
    }

    return text.starts_with("0o") || text.starts_with("0O");
}

bool is_floating(std::string_view text) {
    if (text.starts_with("+") || text.starts_with("-")) {
        text.remove_prefix(1);
    }

    if (is_hex(text)) {
        return text.find('.') != std::string_view::npos ||
               text.find('p') != std::string_view::npos ||
               text.find('P') != std::string_view::npos;
    }

    return text.find('.') != std::string_view::npos ||
           text.find('e') != std::string_view::npos ||
           text.find('E') != std::string_view::npos;
}

bool fits_integral(const ResolvedInteger& value, const Integral& target) {
    if (!value.negative) {
        return value.magnitude <= target.max();
    }

    if (!target.sign()) {
        return false;
    }

    // Signed ranges for e.g. u8 go from -128 to 127 (thus max + 1)
    return value.magnitude <= (target.max() + 1);
}

bool fits_integral(const ResolvedFloating&, const Integral&) {
    // Implicitly converting from float to integral should never be allowed.
    return false;
}

bool fits_floating(const ResolvedInteger& value, const Floating& target) {
    const auto signed_value = value.negative 
        ? -static_cast<long double>(value.magnitude) 
        :  static_cast<long double>(value.magnitude);

    switch (target.size()) {
        case Size::DWORD: {
            const auto converted = static_cast<float>(signed_value);
            return std::isfinite(converted);
        }
        case Size::QWORD: {
            const auto converted = static_cast<double>(signed_value);
            return std::isfinite(converted);
        }
        default: {
            throw std::logic_error("invalid floating-point size");
        }
    }
}

bool fits_floating(const ResolvedFloating& value, const Floating& target) {
    switch (target.size()) {
        case Size::DWORD: {
            const auto converted = static_cast<float>(value.value);
            return std::isfinite(converted) && (value.value == 0.0L || converted != 0.0F);
        }
        case Size::QWORD: {
            const auto converted = static_cast<double>(value.value);
            return std::isfinite(converted) && (value.value == 0.0L || converted != 0.0);
        }
        default: {
            throw std::logic_error("invalid floating-point size");
        }
    }
}

ResolvedInteger parse_integer(std::string_view text) {
    // TODO(tbd): This is just temporary as characters are represented internally as u8
    if (text.length() == 3 && text.front() == '\'' && text.back() == '\'') {
        return ResolvedInteger {
            .magnitude = static_cast<u_int8_t>(text[1]),
            .base = NumericBase::Decimal,
            .negative = false,
        };
    }

    auto negative = false;
    if (text.starts_with("+") || text.starts_with("-")) {
        negative = text.front() == '-';
        text.remove_prefix(1);
    }

    auto base = NumericBase::Decimal;
    if (is_hex(text)) {
        base = NumericBase::Hex;
        text.remove_prefix(2);
    } else if (is_bin(text)) {
        base = NumericBase::Binary;
        text.remove_prefix(2);
    } else if (is_oct(text)) {
        base = NumericBase::Octal;
        text.remove_prefix(2);
    }

    const auto digits = remove_digit_separators(text);
    const auto digits_end = digits.data() + digits.size();

    if (digits.empty()) {
        throw std::runtime_error("numeric literal contains no digits");
    }

    uint64_t magnitude = 0;

    const auto [end, error] = std::from_chars(digits.data(), digits_end, magnitude, static_cast<uint8_t>(base));
    if (error == std::errc::result_out_of_range) {
        throw std::runtime_error("integer literal exceeds the supported 64-bit range");
    }

    if (error != std::errc{} || end != digits_end) {
        throw std::runtime_error("invalid integer literal");
    }

    return ResolvedInteger {
        .magnitude = magnitude,
        .base = base,
        .negative = negative,
    };
}

ResolvedFloating parse_floating(const std::string_view text) {
    const auto digits = remove_digit_separators(text);
    const auto digits_end = digits.c_str() + digits.size();

    if (digits.empty()) {
        throw std::runtime_error("numeric literal contains no digits");
    }

    char* end = nullptr;
    errno = 0;

    const auto value = std::strtold(digits.c_str(), &end);

    if (errno == ERANGE || !std::isfinite(value)) {
        throw std::runtime_error("floating-point literal is outside the supported range");
    }

    if (end != digits_end) {
        throw std::runtime_error("invalid floating-point literal");
    }

    return ResolvedFloating {
        .value = value,
    };
}

ResolvedNumeric sem::parse_numeric(const ast::Immediate& node) {
    auto text = node.value().span().substr();

    if (is_floating(text)) {
        return parse_floating(text);
    }

    return parse_integer(text);
}

Floating smallest_floating_type(const ResolvedFloating& literal) {
    const auto as_f32 = static_cast<float>(literal.value);
    if (std::isfinite(as_f32) && (literal.value == 0.0L || as_f32 != 0.0F)) {
        return Floating { Size::DWORD };
    }

    const auto as_f64 = static_cast<double>(literal.value);
    if (std::isfinite(as_f64) && (literal.value == 0.0L || as_f64 != 0.0)) {
        return Floating { Size::QWORD };
    }

    throw std::runtime_error("floating-point literal cannot be represented as f64");
}

Integral smallest_integer_type(const ResolvedInteger& literal) {
    if (literal.negative) {
        if (literal.magnitude <= (std::uint64_t{1} << 7)) {
            return Integral { Size::BYTE, true };
        }

        if (literal.magnitude <= (std::uint64_t{1} << 15)) {
            return Integral { Size::WORD, true };
        }

        if (literal.magnitude <= (std::uint64_t{1} << 31)) {
            return Integral { Size::DWORD, true };
        }

        if (literal.magnitude <= (std::uint64_t{1} << 63)) {
            return Integral { Size::QWORD, true };
        }

        throw std::runtime_error("negative integer literal is outside the s64 range");
    } else {
        if (literal.magnitude <= static_cast<std::uint64_t>(std::numeric_limits<std::int8_t>::max())) {
            return Integral { Size::BYTE, true };
        }

        if (literal.magnitude <= static_cast<std::uint64_t>(std::numeric_limits<std::int16_t>::max())) {
            return Integral { Size::WORD, true };
        }

        if (literal.magnitude <= static_cast<std::uint64_t>(std::numeric_limits<std::int32_t>::max())) {
            return Integral { Size::DWORD, true };
        }

        if (literal.magnitude <= static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max())) {
            return Integral { Size::QWORD, true };
        }

        return Integral { Size::QWORD, false };
    }    
}

Type sem::infer_smallest_numeric_type(const ResolvedNumeric& resolved) {
    return std::visit(
        match{
            [](const ResolvedInteger& value) -> Type {
                return smallest_integer_type(value);
            },
            [](const ResolvedFloating& value) -> Type {
                return smallest_floating_type(value);
            },
        },
        resolved
    );
}

bool sem::fits_numeric_type(const ResolvedNumeric& value, const Type& target) {
    return std::visit(
        match{
            [](const ResolvedInteger& value, const Integral& target) {
                return fits_integral(value, target);
            },
            [](const ResolvedInteger& value, const Floating& target) {
                return fits_floating(value, target);
            },
            [](const ResolvedFloating& value, const Integral& target) {
                return fits_integral(value, target);
            },
            [](const ResolvedFloating& value, const Floating& target) {
                return fits_floating(value, target);
            },
            [](const auto&, const Boolean&) {
                return false;
            }
        },
        value,
        target
    );
}

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
