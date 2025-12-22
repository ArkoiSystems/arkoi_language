#pragma once

#include <iostream>

/**
 * @brief Enumeration of standard memory and operand sizes in bytes.
 *
 * This enum is used throughout the compiler to represent the size of
 * variables, types, and machine instructions.
 */
enum class Size : size_t {
    BYTE  = 1,  ///< 1 byte (8 bits)
    WORD  = 2,  ///< 2 bytes (16 bits)
    DWORD = 4,  ///< 4 bytes (32 bits)
    QWORD = 8,  ///< 8 bytes (64 bits)
};

/**
 * @brief Converts a `Size` enumeration value to its equivalent in bytes.
 *
 * @param size The size enumeration to convert.
 * @return The size in bytes as a `size_t`.
 * @see size_to_bits
 */
size_t size_to_bytes(const Size& size);

/**
 * @brief Converts a `Size` enumeration value to its equivalent in bits.
 *
 * Useful for calculating bit-offsets or for type systems that define
 * precision in bits.
 *
 * @param size The size enumeration to convert.
 * @return The size in bits as a `size_t`.
 * @see size_to_bytes
 */
size_t size_to_bits(const Size& size);

/**
 * @brief Streams a human-readable string representation of a `Size`.
 *
 * @param os The output stream to write to.
 * @param size The size to represent as a string.
 * @return A reference to the output stream @p os.
 */
std::ostream& operator<<(std::ostream& os, const Size& size);

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
