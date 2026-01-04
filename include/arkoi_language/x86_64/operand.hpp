#pragma once

#include <array>
#include <cstdint>
#include <variant>

#include "arkoi_language/utils/size.hpp"

namespace arkoi::x86_64 {
/**
 * @brief Represents a physical x86-64 machine register.
 *
 * A `Register` is defined by its base architectural identifier (e.g., RAX, RDI)
 * and its current access size (BYTE, WORD, DWORD, or QWORD).
 */
class Register {
public:
    /**
     * @brief Architectural register bases for x86-64.
     */
    enum class Base {
        A, C, D, B, SI, DI, SP, BP, R8, R9, R10, R11, R12, R13, R14, R15,
        XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7, XMM8, XMM9, XMM10, XMM11, XMM12, XMM13, XMM14, XMM15,
    };

public:
    /**
     * @brief Constructs a `Register`.
     *
     * @param base The architectural base identifier.
     * @param size The bit-width to use for access.
     */
    constexpr Register(const Base base, const Size size) :
        _size(size), _base(base) { }

    /**
     * @brief Equality compares the size and base.
     *
     * @param other Right-hand register.
     * @return True if both registers refer to the same size and base.
     */
    bool operator==(const Register& other) const;

    /**
     * @brief Inequality based on `==`.
     *
     * @param other Right-hand register.
     * @return True if the registers differ.
     */
    bool operator!=(const Register& other) const;

    /**
     * @brief Sets the access size of the register.
     *
     * @param size The new `Size`.
     */
    void set_size(const Size size) { _size = size; }

    /**
     * @brief Returns the access size of the register.
     *
     * @return The `Size` enumeration value.
     */
    [[nodiscard]] auto size() const { return _size; }

    /**
     * @brief Returns the architectural base identifier.
     *
     * @return The `Base` enumeration value.
     */
    [[nodiscard]] auto base() const { return _base; }

private:
    Size _size;
    Base _base;
};

/**
 * @brief Represents a memory operand in x86-64 assembly (e.g., `[rbp - 8]`).
 *
 * Supports complex addressing modes including base, index, scale, and displacement.
 */
class Memory {
public:
    /**
     * @brief Represents the base address of the memory access.
     *
     * Can be a symbolic label (string), a fixed offset (int64), or a register.
     */
    struct Address : std::variant<std::string, int64_t, Register> {
        using variant::variant;
    };

public:
    /**
     * @brief Full constructor for complex addressing modes.
     *
     * @param size The size of the memory access (e.g., QWORD).
     * @param address The base address component.
     * @param index The index component.
     * @param scale The scale factor for the index.
     * @param displacement The constant displacement.
     */
    Memory(const Size size, Register address, const int64_t index, const int64_t scale, const int64_t displacement) :
        _index(index), _scale(scale), _displacement(displacement), _address(address), _size(size) { }

    /**
     * @brief Short constructor for common base+displacement addressing.
     *
     * @param size The size of the memory access.
     * @param address The base register.
     * @param displacement The constant offset.
     */
    Memory(const Size size, Register address, const int64_t displacement) :
        _index(1), _scale(1), _displacement(displacement), _address(address), _size(size) { }

    /**
     * @brief Minimal constructor for simple symbolic or fixed addresses.
     *
     * @param size The size of the memory access.
     * @param address The symbolic or fixed base address.
     */
    Memory(const Size size, Address address) :
        _index(1), _scale(1), _displacement(0), _address(std::move(address)), _size(size) { }

    /**
     * @brief Equality compares index, scale, displacement, address, and size.
     *
     * @param other Right-hand memory location.
     * @return True if both memory locations are the same.
     */
    bool operator==(const Memory& other) const;

    /**
     * @brief Inequality based on `==`.
     *
     * @param other Right-hand memory location.
     * @return True if the memory locations differ.
     */
    bool operator!=(const Memory& other) const;

    /**
     * @brief Returns the displacement value.
     *
     * @return The displacement of the address.
     */
    [[nodiscard]] auto displacement() const { return _displacement; }

    /**
     * @brief Sets the base address to the given one.
     *
     * @param address The given address.
     */
    void set_address(const Address& address) { _address = address; }

    /**
     * @brief Returns the base address component.
     *
     * @return A reference to the address.
     */
    [[nodiscard]] auto& address() const { return _address; }

    /**
     * @brief Returns the scale factor.
     *
     * @return The scale of the address.
     */
    [[nodiscard]] auto scale() const { return _scale; }

    /**
     * @brief Returns the index component.
     *
     * @return The index of the address.
     */
    [[nodiscard]] auto index() const { return _index; }

    /**
     * @brief Returns the size of the memory access.
     *
     * @return The size of the address.
     */
    [[nodiscard]] auto size() const { return _size; }

private:
    int64_t _index, _scale, _displacement;
    Address _address;
    Size _size;
};

/**
 * @brief Represents an immediate (literal) value in machine code.
 *
 * Immediates can be labels, integral values, or floating-point values.
 */
struct Immediate : std::variant<std::string, uint64_t, int64_t, uint32_t, int32_t, double, float, bool> {
    using variant::variant;
};

/**
 * @brief A generic container for any x86-64 machine operand.
 *
 * `Operand` is a `std::variant` of `Memory`, `Register`, and `Immediate`.
 */
struct Operand : std::variant<Memory, Register, Immediate> {
    using variant::variant;
};
} // namespace arkoi::x86_64

/**
 * @brief Streams a detailed description of a `Register`.
 *
 * @param os The output stream.
 * @param reg The register to describe.
 * @return A reference to the output stream @p os
 */
std::ostream& operator<<(std::ostream& os, const arkoi::x86_64::Register& reg);

/**
 * @brief Streams a detailed description of a `Register::Base`.
 *
 * @param os The output stream.
 * @param base The register base to describe.
 * @return A reference to the output stream @p os
 */
std::ostream& operator<<(std::ostream& os, const arkoi::x86_64::Register::Base& base);

/**
 * @brief Streams a detailed description of a `Memory`.
 *
 * @param os The output stream.
 * @param memory The memory to describe.
 * @return A reference to the output stream @p os
 */
std::ostream& operator<<(std::ostream& os, const arkoi::x86_64::Memory& memory);

/**
 * @brief Streams a detailed description of a `Memory::Address`.
 *
 * @param os The output stream.
 * @param address The memory address to describe.
 * @return A reference to the output stream @p os
 */
std::ostream& operator<<(std::ostream& os, const arkoi::x86_64::Memory::Address& address);

/**
 * @brief Streams a detailed description of a `Immediate`.
 *
 * @param os The output stream.
 * @param immediate The immediate to describe.
 * @return A reference to the output stream @p os
 */
std::ostream& operator<<(std::ostream& os, const arkoi::x86_64::Immediate& immediate);

/**
 * @brief Streams a detailed description of a `Operand`.
 *
 * @param os The output stream.
 * @param operand The operand to describe.
 * @return A reference to the output stream @p os
 */
std::ostream& operator<<(std::ostream& os, const arkoi::x86_64::Operand& operand);

namespace arkoi::x86_64 {
static constexpr std::array INTEGER_CALLEE_SAVED{
    Register::Base::B, Register::Base::R12, Register::Base::R13, Register::Base::R14,
    Register::Base::R15,
};

static constexpr std::array INTEGER_CALLER_SAVED{
    Register::Base::A, Register::Base::C, Register::Base::D, Register::Base::SI,
    Register::Base::DI, Register::Base::R8, Register::Base::R9
};

static constexpr std::array FLOATING_REGISTERS{
    Register::Base::XMM8, Register::Base::XMM9, Register::Base::XMM12, Register::Base::XMM13,
    Register::Base::XMM14, Register::Base::XMM15,
};

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
