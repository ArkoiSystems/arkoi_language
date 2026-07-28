#include "arkoi_language/sem/numeric_literal.hpp"

#include <stdexcept>

#include "arkoi_language/utils/utils.hpp"

using namespace arkoi::sem;
using namespace arkoi;

bool fits_integral(const ResolvedInteger&, const Integral&) {
    throw std::logic_error("Integer-literal range checking is not implemented.");
}

bool fits_integral(const ResolvedFloating&, const Integral&) {
    throw std::logic_error("Floating-to-integer literal range checking is not implemented.");
}

bool fits_floating(const ResolvedInteger&, const Floating&) {
    throw std::logic_error("Integer-to-floating literal range checking is not implemented.");
}

bool fits_floating(const ResolvedFloating&, const Floating&) {
    throw std::logic_error("Floating-literal range checking is not implemented.");
}

ResolvedNumeric sem::parse_numeric(const ast::Immediate&) {
    throw std::logic_error("Numeric-literal parsing is not implemented.");
}

Type sem::infer_smallest_numeric_type(const ResolvedNumeric&) {
    throw std::logic_error("Numeric-literal type inference is not implemented.");
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
