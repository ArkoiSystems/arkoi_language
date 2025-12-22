#pragma once

#include <cstdint>
#include <variant>

#include "arkoi_language/utils/size.hpp"

namespace arkoi::sem {
/**
 * @brief Base class for all semantic types
 */
class TypeBase {
public:
    virtual ~TypeBase() = default;

    /**
     * @brief Returns the size of the type
     *
     * @return The size as enumeration
     */
    [[nodiscard]] virtual Size size() const = 0;
};

/**
 * @brief Represents an integral type
 */
class Integral final : public TypeBase {
public:
    /**
     * @brief Constructs an Integral type with the given parameters
     *
     * @param size The size of the type
     * @param sign Whether the type is signed
     */
    constexpr Integral(const Size size, const bool sign) :
        _size(size), _sign(sign) { }

    /**
     * @brief Returns the size of the type
     *
     * @return The size as enumeration
     */
    [[nodiscard]] Size size() const override { return _size; }

    /**
     * @brief Equality compares the size and signess
     *
     * @param other Right-hand integral type
     *
     * @return True if both types have the same size and signess
     */
    bool operator==(const Integral& other) const;

    /**
     * @brief Inequality based on `==`
     *
     * @param other Right-hand integral type
     *
     * @return True if the types differ
     */
    bool operator!=(const Integral& other) const;

    /**
     * @brief Returns the maximum value representable by this type
     *
     * @return The maximum value
     */
    [[nodiscard]] uint64_t max() const;

    /**
     * @brief Returns whether the type is signed
     *
     * @return True if signed, false otherwise
     */
    [[nodiscard]] auto sign() const { return _sign; }

private:
    Size _size;
    bool _sign;
};

/**
 * @brief Represents a floating-point type
 */
class Floating final : public TypeBase {
public:
    /**
     * @brief Constructs a Floating type with the given size
     *
     * @param size The size of the type
     */
    explicit Floating(const Size size) :
        _size(size) { }

    /**
     * @brief Returns the size of the type
     *
     * @return The size as enumeration
     */
    [[nodiscard]] Size size() const override { return _size; }

    /**
     * @brief Equality compares only the size
     *
     * @param other Right-hand floating type
     *
     * @return True if both types have the same size
     */
    bool operator==(const Floating& other) const;

    /**
     * @brief Inequality based on `==`
     *
     * @param other Right-hand floating type
     *
     * @return True if the types differ
     */
    bool operator!=(const Floating& other) const;

private:
    Size _size{ };
};

/**
 * @brief Represents a boolean type
 */
class Boolean final : public TypeBase {
public:
    /**
     * @brief Returns the size of the type
     *
     * @return The size as enumeration (always BYTE)
     */
    [[nodiscard]] Size size() const override { return Size::BYTE; }

    /**
     * @brief Equality will always be true
     *
     * @param other Right-hand boolean type
     *
     * @return Always true as boolean types can't differ
     */
    bool operator==(const Boolean& other) const;

    /**
     * @brief Inequality based on `==`
     *
     * @param other Right-hand boolean type
     *
     * @return True if the types differ
     */
    bool operator!=(const Boolean& other) const;
};

/**
 * @brief Represents a generic semantic type, which can be integral, floating, or boolean
 */
struct Type final : TypeBase, std::variant<Integral, Floating, Boolean> {
    using variant::variant;

    /**
     * @brief Returns the size of the type
     *
     * @return The size as enumeration
     */
    [[nodiscard]] Size size() const override;
};
} // namespace arkoi::sem

/**
 * @brief Streams a readable description of a `arkoi::sem::Integral`
 *
 * @param os Output stream to write to
 * @param type Integral type to describe
 *
 * @return Reference to @p os.
 */
std::ostream& operator<<(std::ostream& os, const arkoi::sem::Integral& type);

/**
 * @brief Streams a readable description of a `arkoi::sem::Floating`
 *
 * @param os Output stream to write to
 * @param type Floating type to describe
 *
 * @return Reference to @p os.
 */
std::ostream& operator<<(std::ostream& os, const arkoi::sem::Floating& type);

/**
 * @brief Streams a readable description of a `arkoi::sem::Boolean`
 *
 * @param os Output stream to write to
 * @param type Boolean type to describe
 *
 * @return Reference to @p os.
 */
std::ostream& operator<<(std::ostream& os, const arkoi::sem::Boolean& type);

/**
 * @brief Streams a readable description of a `arkoi::sem::Type`
 *
 * @param os Output stream to write to
 * @param type Type to describe
 *
 * @return Reference to @p os.
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
