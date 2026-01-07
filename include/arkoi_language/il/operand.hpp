#pragma once

#include <iostream>
#include <variant>

#include "arkoi_language/sem/type.hpp"

namespace arkoi::il {
/**
 * @brief Represents a memory location on the stack.
 *
 * `Memory` operands are used by `Alloca`, `Load`, and `Store` instructions.
 * They are identified by an index/version.
 */
class Memory final {
public:
    /**
     * @brief Constructs a `Memory` operand.
     *
     * @param name The unique name of the memory slot.
     * @param type The type of data stored at this memory location.
     */
    Memory(std::string name, sem::Type type) :
        _name(std::move(name)), _type(std::move(type)) { }

    /**
     * @brief Compares two memory locations for ordering.
     *
     * @param rhs Right-hand memory location.
     *
     * @return True if this memory location starts before @p rhs.
     */
    bool operator<(const Memory& rhs) const;

    /**
     * @brief Checks if two memory operands refer to the same location.
     *
     * @param rhs Right-hand memory location.
     *
     * @return True if both memory locations have the same indices.
     */
    bool operator==(const Memory& rhs) const;

    /**
     * @brief Checks if two memory operands refer to different locations.
     *
     * @param rhs Right-hand memory location
     *
     * @return True if the memory locations differ.
     */
    bool operator!=(const Memory& rhs) const;

    /**
     * @brief Returns the semantic type of the stored data.
     *
     * @return The `sem::Type` of this memory operand.
     */
    [[nodiscard]] sem::Type type() const { return _type; }

    /**
     * @brief Returns the unique name of this memory location.
     *
     * @return A reference to the `std::string` name.
     */
    [[nodiscard]] auto& name() const { return _name; }

private:
    std::string _name;
    sem::Type _type;
};

/**
 * @brief Represents a symbolic variable in the IL.
 *
 * Variables in Arkoi IL follow Static Single Assignment (SSA) form principles,
 * though the `version` field explicitly tracks different assignments to the
 * same original source-level variable.
 */
class Variable final {
public:
    /**
     * @brief Constructs a `Variable` operand.
     *
     * @param name The original source-level name of the variable.
     * @param type The semantic type of the variable.
     * @param version The SSA version of the variable (defaults to 0).
     */
    Variable(std::string name, sem::Type type, const size_t version = 0) :
        _name(std::move(name)), _version(version), _type(std::move(type)) { }

    /**
     * @brief Compares two variables for ordering (name then version).
     *
     * @param rhs Right-hand variable.
     *
     * @return True if this variable starts before @p rhs.
     */
    bool operator<(const Variable& rhs) const;

    /**
     * @brief Checks if two variables are identical (same name and version).
     *
     * @param rhs Right-hand variable.
     *
     * @return True if both variables refer to the same name and version.
     */
    bool operator==(const Variable& rhs) const;

    /**
     * @brief Checks if two variables are different.
     *
     * @param rhs Right-hand variable.
     *
     * @return True if the variables differ.
     */
    bool operator!=(const Variable& rhs) const;

    /**
     * @brief Returns the semantic type of the variable.
     *
     * @return The `sem::Type` of this variable.
     */
    [[nodiscard]] sem::Type type() const { return _type; }

    /**
     * @brief Returns the SSA version of the variable.
     *
     * @return The `size_t` version.
     */
    [[nodiscard]] auto version() const { return _version; }

    /**
     * @brief Returns the source-level name of the variable.
     *
     * @return A constant reference to the name string.
     */
    [[nodiscard]] auto& name() const { return _name; }

private:
    std::string _name;
    size_t _version;
    sem::Type _type;
};

/**
 * @brief Represents a literal constant (immediate) value.
 *
 * `Immediate` is a variant that can hold various primitive C++ types
 * that map to Arkoi source types.
 */
struct Immediate final : std::variant<uint64_t, int64_t, uint32_t, int32_t, double, float, bool> {
    using variant::variant;

    /**
     * @brief Maps the active variant type to a `sem::Type`.
     *
     * @return The corresponding `sem::Type`.
     */
    [[nodiscard]] sem::Type type() const;
};

/**
 * @brief A generic container for any IL operand.
 *
 * `Operand` is a `std::variant` of `Immediate`, `Variable`, and `Memory`.
 * This type is used by `InstructionBase` to represent inputs and outputs.
 */
struct Operand final : std::variant<Immediate, Variable, Memory> {
    using variant::variant;

    /**
     * @brief Returns the semantic type of the underlying operand.
     *
     * @return The `sem::Type` of the active operand kind.
     */
    [[nodiscard]] sem::Type type() const;
};
} // namespace arkoi::il

namespace std {
template <>
struct hash<arkoi::il::Variable> {
    size_t operator()(const arkoi::il::Variable& variable) const noexcept;
};

template <>
struct hash<arkoi::il::Memory> {
    size_t operator()(const arkoi::il::Memory& memory) const noexcept;
};

template <>
struct hash<arkoi::il::Immediate> {
    size_t operator()(const arkoi::il::Immediate& immediate) const noexcept;
};

template <>
struct hash<arkoi::il::Operand> {
    size_t operator()(const arkoi::il::Operand& operand) const noexcept;
};
} // namespace std

/**
 * @brief Streams a detailed description of a `Immediate`.
 *
 * @param os The output stream.
 * @param operand The immediate to describe.
 * @return A reference to the output stream @p os
 */
std::ostream& operator<<(std::ostream& os, const arkoi::il::Immediate& operand);

/**
 * @brief Streams a detailed description of a `Variable`.
 *
 * @param os The output stream.
 * @param operand The variable to describe.
 * @return A reference to the output stream @p os
 */
std::ostream& operator<<(std::ostream& os, const arkoi::il::Variable& operand);

/**
 * @brief Streams a detailed description of a `Memory`.
 *
 * @param os The output stream.
 * @param operand The memory to describe.
 * @return A reference to the output stream @p os
 */
std::ostream& operator<<(std::ostream& os, const arkoi::il::Memory& operand);

/**
 * @brief Streams a detailed description of a `Operand`.
 *
 * @param os The output stream.
 * @param operand The operand to describe.
 * @return A reference to the output stream @p os
 */
std::ostream& operator<<(std::ostream& os, const arkoi::il::Operand& operand);

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
