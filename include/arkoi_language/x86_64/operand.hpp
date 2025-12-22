#pragma once

#include <array>
#include <cstdint>
#include <variant>

#include "arkoi_language/utils/size.hpp"

namespace arkoi::x86_64 {
/**
 * @brief Represents an x86_64 register
 */
class Register {
public:
    /**
     * @brief Enumeration of register bases
     */
    enum class Base {
        A, C, D, B, SI, DI, SP, BP, R8, R9, R10, R11, R12, R13, R14, R15,
        XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7, XMM8, XMM9, XMM10, XMM11, XMM12, XMM13, XMM14, XMM15,
    };

public:
    /**
     * @brief Constructs a Register with the given base and size
     *
     * @param base The base of the register
     * @param size The size of the register
     */
    constexpr Register(const Base base, const Size size) :
        _size(size), _base(base) { }

    /**
     * @brief Equality compares the size and base
     *
     * @param other Right-hand register
     *
     * @return True if both register refer the same size and base
     */
    bool operator==(const Register& other) const;

    /**
     * @brief Inequality based on `==`
     *
     * @param other Right-hand register
     *
     * @return True if the registers differ
     */
    bool operator!=(const Register& other) const;

    /**
     * @brief Sets the size of the register
     *
     * @param size The size to set
     */
    void set_size(const Size size) { _size = size; }

    /**
     * @brief Returns the size of the register
     *
     * @return The size
     */
    [[nodiscard]] auto size() const { return _size; }

    /**
     * @brief Returns the base of the register
     *
     * @return The base
     */
    [[nodiscard]] auto base() const { return _base; }

private:
    Size _size;
    Base _base;
};

/**
 * @brief Represents a memory operand in x86_64 assembly
 */
class Memory {
public:
    /**
     * @brief Represents a memory address (label, offset, or register)
     */
    struct Address : std::variant<std::string, int64_t, Register> {
        using variant::variant;
    };

public:
    /**
     * @brief Constructs a Memory operand with the given parameters
     *
     * @param size The size of the memory access
     * @param address The base address
     * @param index The index register index (optional)
     * @param scale The scale factor
     * @param displacement The displacement value
     */
    Memory(Size size, Register address, int64_t index, int64_t scale, int64_t displacement) :
        _index(index), _scale(scale), _displacement(displacement), _address(address), _size(size) { }

    /**
     * @brief Constructs a Memory operand with base address and displacement
     *
     * @param size The size of the memory access
     * @param address The base address register
     * @param displacement The displacement value
     */
    Memory(Size size, Register address, int64_t displacement) :
        _index(1), _scale(1), _displacement(displacement), _address(address), _size(size) { }

    /**
     * @brief Constructs a Memory operand with an Address
     *
     * @param size The size of the memory access
     * @param address The address
     */
    Memory(Size size, Address address) :
        _index(1), _scale(1), _displacement(0), _address(std::move(address)), _size(size) { }

    /**
     * @brief Equality compares index, scale, displacement, address, and size
     *
     * @param other Right-hand memory location
     *
     * @return True if both memory location are the same
     */
    bool operator==(const Memory& other) const;

    /**
     * @brief Inequality based on `==`
     *
     * @param other Right-hand memory location
     *
     * @return True if the memory locations differ
     */
    bool operator!=(const Memory& other) const;

    /**
     * @brief Returns the displacement value
     *
     * @return The displacement
     */
    [[nodiscard]] auto displacement() const { return _displacement; }

    /**
     * @brief Returns the base address
     *
     * @return A reference to the address
     */
    [[nodiscard]] auto& address() const { return _address; }

    /**
     * @brief Returns the scale factor
     *
     * @return The scale
     */
    [[nodiscard]] auto scale() const { return _scale; }

    /**
     * @brief Returns the index value
     *
     * @return The index
     */
    [[nodiscard]] auto index() const { return _index; }

    /**
     * @brief Returns the size of the memory access
     *
     * @return The size
     */
    [[nodiscard]] auto size() const { return _size; }

private:
    int64_t _index, _scale, _displacement;
    Address _address;
    Size _size;
};

/**
 * @brief Represents an immediate value in x86_64 assembly
 */
struct Immediate : std::variant<std::string, uint64_t, int64_t, uint32_t, int32_t, double, float, bool> {
    using variant::variant;
};

/**
 * @brief Represents a generic x86_64 operand (memory, register, or immediate)
 */
struct Operand : std::variant<Memory, Register, Immediate> {
    using variant::variant;
};
} // namespace arkoi::x86_64

/**
 * @brief Streams a readable description of a `x86_64::Register`
 *
 * @param os Output stream to write to
 * @param reg Register to describe
 *
 * @return Reference to @p os
 */
std::ostream& operator<<(std::ostream& os, const arkoi::x86_64::Register& reg);

/**
 * @brief Streams a readable description of a `x86_64::Register::Base`
 *
 * @param os Output stream to write to
 * @param base Base to describe
 *
 * @return Reference to @p os
 */
std::ostream& operator<<(std::ostream& os, const arkoi::x86_64::Register::Base& base);

/**
 * @brief Streams a readable description of a `x86_64::Memory`
 *
 * @param os Output stream to write to
 * @param memory Memory to describe
 *
 * @return Reference to @p os
 */
std::ostream& operator<<(std::ostream& os, const arkoi::x86_64::Memory& memory);

/**
 * @brief Streams a readable description of a `x86_64::Memory::Address`
 *
 * @param os Output stream to write to
 * @param address Address to describe
 *
 * @return Reference to @p os
 */
std::ostream& operator<<(std::ostream& os, const arkoi::x86_64::Memory::Address& address);

/**
 * @brief Streams a readable description of a `x86_64::Immediate`
 *
 * @param os Output stream to write to
 * @param immediate Immediate to describe
 *
 * @return Reference to @p os
 */
std::ostream& operator<<(std::ostream& os, const arkoi::x86_64::Immediate& immediate);

/**
 * @brief Streams a readable description of a `x86_64::Operand`
 *
 * @param os Output stream to write to
 * @param operand Operand to describe
 *
 * @return Reference to @p os
 */
std::ostream& operator<<(std::ostream& os, const arkoi::x86_64::Operand& operand);

namespace arkoi::x86_64 {
static constexpr std::array INTEGER_ARGUMENT_REGISTERS{
    Register::Base::DI, Register::Base::SI, Register::Base::D,
    Register::Base::C, Register::Base::R8, Register::Base::R9
};

static constexpr std::array SSE_ARGUMENT_REGISTERS{
    Register::Base::XMM0, Register::Base::XMM1, Register::Base::XMM2, Register::Base::XMM3,
    Register::Base::XMM4, Register::Base::XMM5, Register::Base::XMM6, Register::Base::XMM7
};

static constexpr Register RSP(Register::Base::SP, Size::QWORD);
static constexpr Register RBP(Register::Base::BP, Size::QWORD);
static constexpr Register RAX(Register::Base::A, Size::QWORD);
static constexpr Register RDI(Register::Base::DI, Size::QWORD);
} // namespace arkoi::x86_64

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
