#pragma once

#include <cstdint>
#include <variant>

#include "arkoi_language/ast/nodes.hpp"
#include "arkoi_language/sem/type.hpp"

namespace arkoi::sem {
/**
 * @brief Represents the exact magnitude and sign of an integral literal.
 *
 * Storing the magnitude separately allows the full `u64` range and the
 * magnitude of `s64`'s minimum value to be represented without overflow.
 */
struct ResolvedInteger {
    uint64_t magnitude; ///< Absolute value of the literal.
    bool negative;      ///< Whether the source literal has a negative sign.
};

/**
 * @brief Represents a parsed floating-point literal.
 */
struct ResolvedFloating {
    long double value; ///< Parsed value with sufficient precision for type checks.
};

/**
 * @brief A parsed numeric literal independent of its final semantic type.
 */
using ResolvedNumeric = std::variant<ResolvedInteger, ResolvedFloating>;

/**
 * @brief Parses a numeric immediate without assigning a semantic type.
 *
 * @param node The numeric immediate to parse.
 * @return The exact integral representation or parsed floating-point value.
 */
[[nodiscard]] ResolvedNumeric parse_numeric(const ast::Immediate& node);

/**
 * @brief Infers the smallest default semantic type for a parsed literal.
 *
 * Integral literals prefer signed types through `s64`. A positive integral
 * literal uses `u64` only when its value exceeds the `s64` range.
 *
 * @param value The parsed numeric literal.
 * @return The smallest semantic type selected by the default literal rules.
 */
[[nodiscard]] Type infer_smallest_numeric_type(const ResolvedNumeric& value);

/**
 * @brief Checks whether a parsed literal is representable by a target type.
 *
 * The check uses the literal value rather than only its inferred fallback type,
 * allowing differently signed targets when the concrete value is in range.
 *
 * @param value The parsed numeric literal.
 * @param target The candidate semantic type.
 * @return True when the literal can be represented by target.
 */
[[nodiscard]] bool fits_numeric_type(const ResolvedNumeric& value, const Type& target);
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
