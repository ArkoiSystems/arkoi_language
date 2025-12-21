#pragma once

#include <iostream>
#include <variant>

#include "arkoi_language/sem/type.hpp"

namespace arkoi::il {
/**
 * @brief Base class for all IL operands
 */
class OperandBase {
public:
    virtual ~OperandBase() = default;

    /**
     * @brief Returns the type of the operand
     *
     * @return The type used by this operand
     */
    [[nodiscard]] virtual sem::Type type() const = 0;
};

/**
 * @brief Represents a memory location operand in the IL
 */
class Memory final : public OperandBase {
public:
    /**
     * @brief Constructs a Memory operand with the given parameters
     *
     * @param version The version (or index) of the memory location
     * @param type The type of the value stored in memory
     */
    Memory(const size_t version, sem::Type type) :
        _type(std::move(type)), _index(version) { }

    /**
     * @brief Less-than comparison based on the index
     *
     * @param rhs Right-hand span
     *
     * @return True if this memory location starts before @p rhs
     */
    bool operator<(const Memory& rhs) const;

    /**
     * @brief Equality compares only the indices
     *
     * @param rhs Right-hand span
     *
     * @return True if both memory locations have the same indices
     */
    bool operator==(const Memory& rhs) const;

    /**
     * @brief Inequality based on `==`
     *
     * @param rhs Right-hand span
     *
     * @return True if the memory locations differ
     */
    bool operator!=(const Memory& rhs) const;

    /**
     * @brief Returns the type of the operand
     *
     * @return The type used by this operand
     */
    [[nodiscard]] sem::Type type() const override { return _type; }

    /**
     * @brief Returns the index of the memory location
     *
     * @return The index/version of the memory location
     */
    [[nodiscard]] auto index() const { return _index; }

private:
    sem::Type _type;
    size_t _index;
};

/**
 * @brief Represents a variable operand in the IL
 */
class Variable final : public OperandBase {
public:
    /**
     * @brief Constructs a Variable operand with the given parameters
     *
     * @param name The name of the variable
     * @param type The type of the variable
     * @param version The version of the variable (SSA)
     */
    Variable(std::string name, sem::Type type, size_t version = 0) :
        _name(std::move(name)), _version(version), _type(std::move(type)) { }

    /**
     * @brief Less-than comparison based on the name and version
     *
     * @param rhs Right-hand span
     *
     * @return True if this variable starts before @p rhs
     */
    bool operator<(const Variable& rhs) const;

    /**
     * @brief Equality compares the name and version of the variable
     *
     * @param rhs Right-hand span
     *
     * @return True if both variables refer to the same name and version
     */
    bool operator==(const Variable& rhs) const;

    /**
     * @brief Inequality based on `==`
     *
     * @param rhs Right-hand span
     *
     * @return True if the variables differ
     */
    bool operator!=(const Variable& rhs) const;

    /**
     * @brief Returns the type of the operand
     *
     * @return The type used by this operand
     */
    [[nodiscard]] sem::Type type() const override { return _type; }

    /**
     * @brief Returns the version of the variable
     *
     * @return The version of the variable
     */
    [[nodiscard]] auto version() const { return _version; }

    /**
     * @brief Returns the name of the variable
     *
     * @return A reference to the name string
     */
    [[nodiscard]] auto& name() const { return _name; }

private:
    std::string _name;
    size_t _version;
    sem::Type _type;
};

/**
 * @brief Represents an immediate value operand in the IL
 */
struct Immediate final : OperandBase, std::variant<uint64_t, int64_t, uint32_t, int32_t, double, float, bool> {
    using variant::variant;

    /**
     * @brief Returns the type of the operand
     *
     * @return The type used by this operand
     */
    [[nodiscard]] sem::Type type() const override;
};

/**
 * @brief Represents an IL operand, which can be an immediate, a variable, or a memory location
 */
struct Operand final : OperandBase, std::variant<Immediate, Variable, Memory> {
    using variant::variant;

    /**
     * @brief Returns the type of the operand
     *
     * @return The type used by this operand
     */
    [[nodiscard]] sem::Type type() const override;
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
 * @brief Streams a readable description of a `il::Immediate`
 *
 * @param os Output stream to write to
 * @param operand Immediate to describe
 *
 * @return Reference to @p os.
 */
std::ostream& operator<<(std::ostream& os, const arkoi::il::Immediate& operand);

/**
 * @brief Streams a readable description of a `il::Variable`
 *
 * @param os Output stream to write to
 * @param operand Variable to describe
 *
 * @return Reference to @p os.
 */
std::ostream& operator<<(std::ostream& os, const arkoi::il::Variable& operand);

/**
 * @brief Streams a readable description of a `il::Memory`
 *
 * @param os Output stream to write to
 * @param operand Memory to describe
 *
 * @return Reference to @p os.
 */
std::ostream& operator<<(std::ostream& os, const arkoi::il::Memory& operand);

/**
 * @brief Streams a readable description of a `il::Operand`
 *
 * @param os Output stream to write to
 * @param operand Operand to describe
 *
 * @return Reference to @p os.
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
