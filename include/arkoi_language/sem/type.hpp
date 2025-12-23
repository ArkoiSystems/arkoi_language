#pragma once

#include <cstdint>
#include <variant>

#include "arkoi_language/utils/size.hpp"

namespace arkoi::sem {
/**
 * @brief Abstract base class for all semantic types in the language.
 *
 * A `Type` determines the size and layout of data, as well as the
 * operations that can be performed on it.
 */
class TypeBase {
public:
    virtual ~TypeBase() = default;

    /**
     * @brief Returns the storage size required for this type.
     *
     * @return The `Size` enumeration value (BYTE, WORD, etc).
     */
    [[nodiscard]] virtual Size size() const = 0;
};

/**
 * @brief Represents an integral semantic type.
 */
class Integral final : public TypeBase {
public:
    /**
     * @brief Constructs an `Integral` type.
     *
     * @param size The size of the integer (e.g., QWORD for 64-bit).
     * @param sign True if the integer is signed, false if unsigned.
     */
    constexpr Integral(const Size size, const bool sign) :
        _size(size), _sign(sign) { }

    /**
     * @brief Returns the storage size of the integer.
     *
     * @return The `Size` enumeration value.
     */
    [[nodiscard]] Size size() const override { return _size; }

    /**
     * @brief Checks if two integral types have the same size and signess.
     *
     * @param other Right-hand integral type.
     * @return True if both types have the same size and signess.
     */
    bool operator==(const Integral& other) const;

    /**
     * @brief Inequality based on `==`.
     *
     * @param other Right-hand integral type.
     * @return True if the types differ.
     */
    bool operator!=(const Integral& other) const;

    /**
     * @brief Returns the maximum representable value for this integral type.
     *
     * @return The maximum value as a `uint64_t`.
     */
    [[nodiscard]] uint64_t max() const;

    /**
     * @brief Returns whether the type is signed.
     *
     * @return True if signed, false otherwise.
     */
    [[nodiscard]] auto sign() const { return _sign; }

private:
    Size _size;
    bool _sign;
};

/**
 * @brief Represents a floating-point semantic type.
 */
class Floating final : public TypeBase {
public:
    /**
     * @brief Constructs a `Floating` type.
     *
     * @param size The size of the float (e.g., DWORD for f32, QWORD for f64).
     */
    explicit Floating(const Size size) :
        _size(size) { }

    /**
     * @brief Returns the storage size of the floating-point value.
     *
     * @return The `Size` enumeration value.
     */
    [[nodiscard]] Size size() const override { return _size; }

    /**
     * @brief Equality compares only the size.
     *
     * @param other Right-hand floating type.
     * @return True if both types have the same size.
     */
    bool operator==(const Floating& other) const;

    /**
     * @brief Inequality based on `==`.
     *
     * @param other Right-hand floating type.
     * @return True if the types differ.
     */
    bool operator!=(const Floating& other) const;

private:
    Size _size{ };
};

/**
 * @brief Represents the boolean semantic type.
 *
 * Booleans are always represented as a single byte in memory.
 */
class Boolean final : public TypeBase {
public:
    /**
     * @brief Returns the storage size of a boolean.
     *
     * @return Always returns `Size::BYTE`.
     */
    [[nodiscard]] Size size() const override { return Size::BYTE; }

    /**
     * @brief Equality will always be true.
     *
     * @param other Right-hand boolean type.
     * @return Always true as boolean types can't differ.
     */
    bool operator==(const Boolean& other) const;

    /**
     * @brief Inequality based on `==`.
     *
     * @param other Right-hand boolean type.
     * @return True if the types differ.
     */
    bool operator!=(const Boolean& other) const;
};

/**
 * @brief A type-safe container for any semantic type.
 *
 * `Type` is a `std::variant` of `Integral`, `Floating`, and `Boolean`.
 * It provides a uniform interface for type operations while preserving
 * concrete type information.
 */
struct Type final : TypeBase, std::variant<Integral, Floating, Boolean> {
    using variant::variant;

    /**
     * @brief Returns the size of the underlying type.
     *
     * @return The `Size` of the active type variant.
     */
    [[nodiscard]] Size size() const override;
};
} // namespace arkoi::sem

/**
 * @brief Streams a detailed description of an `Integral`.
 *
 * @param os The output stream.
 * @param type The integral type to describe.
 * @return A reference to the output stream @p os.
 */
std::ostream& operator<<(std::ostream& os, const arkoi::sem::Integral& type);

/**
 * @brief Streams a detailed description of a `Floating`.
 *
 * @param os The output stream.
 * @param type The floating type to describe.
 * @return A reference to the output stream @p os.
 */
std::ostream& operator<<(std::ostream& os, const arkoi::sem::Floating& type);

/**
 * @brief Streams a detailed description of a `Boolean`.
 *
 * @param os The output stream.
 * @param type The boolean type to describe.
 * @return A reference to the output stream @p os.
 */
std::ostream& operator<<(std::ostream& os, const arkoi::sem::Boolean& type);

/**
 * @brief Streams a detailed description of a `sem::Type`.
 *
 * @param os The output stream.
 * @param type The type to describe.
 * @return A reference to the output stream @p os.
 */
std::ostream& operator<<(std::ostream& os, const arkoi::sem::Type& type);

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
