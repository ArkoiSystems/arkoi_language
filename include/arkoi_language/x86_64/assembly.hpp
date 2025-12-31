#pragma once

#include <variant>
#include <vector>

#include "arkoi_language/x86_64/operand.hpp"

namespace arkoi::x86_64 {
/**
 * @brief Represents a symbolic label in x86-64 assembly code.
 *
 * Labels are used as jump targets or to identify function entry points.
 */
class Label {
public:
    /**
     * @brief Constructs a `Label`.
     *
     * @param name The string identifier for the label (e.g., "main", ".L1").
     */
    explicit Label(std::string name) :
        _name(std::move(name)) { }

    /**
     * @brief Returns the name of the label.
     *
     * @return A constant reference to the name string.
     */
    [[nodiscard]] auto& name() const { return _name; }

private:
    std::string _name;
};

/**
 * @brief Represents an assembly directive (e.g., `.text`, `.global`).
 */
class Directive {
public:
    /**
     * @brief Constructs a `Directive`.
     *
     * @param text The full text of the directive.
     */
    explicit Directive(std::string text) :
        _text(std::move(text)) { }

    /**
     * @brief Returns the directive text.
     *
     * @return A constant reference to the text string.
     */
    [[nodiscard]] auto& text() const { return _text; }

private:
    std::string _text;
};

/**
 * @brief Represents a single x86-64 machine instruction.
 *
 * An instruction consists of an opcode and zero or more operands.
 *
 * @see Opcode, Operand
 */
class Instruction {
public:
    /**
     * @brief Supported x86-64 opcodes in the assembly generation.
     */
    enum class Opcode {
        CALL, MOV, SYSCALL, ENTER, LEAVE, RET, ADDSD, ADDSS, ADD, SUBSD, SUBSS, SUB, MULSD, MULSS, IMUL, DIVSD, DIVSS,
        IDIV, DIV, UCOMISD, UCOMISS, SETA, CMP, SETG, SETB, SETL, CVTSS2SD, CVTSD2SS, MOVSXD, MOVSX, MOVZX, CVTTSD2SI,
        CVTTSS2SI, XORPS, SETNE, SETP, OR, CVTSI2SD, CVTSI2SS, TEST, JNZ, JMP, MOVSD, MOVSS, PUSH
    };

public:
    /**
     * @brief Constructs an `Instruction`.
     *
     * @param opcode The machine operation code.
     * @param operands The list of machine operands (registers, immediates, or memory).
     */
    Instruction(const Opcode opcode, std::vector<Operand> operands) :
        _operands(std::move(operands)), _opcode(opcode) { }

    /**
     * @brief Returns the instruction's operands.
     *
     * @return A constant reference to the operand vector.
     */
    [[nodiscard]] auto& operands() const { return _operands; }

    /**
     * @brief Returns the instruction's opcode.
     *
     * @return The `Opcode` value.
     */
    [[nodiscard]] auto& opcode() const { return _opcode; }

private:
    std::vector<Operand> _operands;
    Opcode _opcode;
};

/**
 * @brief A generic container for any item in an assembly source listing.
 *
 * `AssemblyItem` is a `std::variant` of `Label`, `Directive`, and `Instruction`.
 */
struct AssemblyItem final : std::variant<Label, Directive, Instruction> {
    using variant::variant;
};
} // namespace arkoi::x86_64

/**
 * @brief Streams a detailed description of a `Label`.
 *
 * @param os The output stream.
 * @param label The label to describe.
 * @return A reference to the output stream @p os.
 */
std::ostream& operator<<(std::ostream& os, const arkoi::x86_64::Label& label);

/**
 * @brief Streams a detailed description of a `Directive`.
 *
 * @param os The output stream.
 * @param directive The directive to describe.
 * @return A reference to the output stream @p os.
 */
std::ostream& operator<<(std::ostream& os, const arkoi::x86_64::Directive& directive);

/**
 * @brief Streams a detailed description of a `Instruction::Opcode`.
 *
 * @param os The output stream.
 * @param opcode The instruction opcode to describe.
 * @return A reference to the output stream @p os.
 */
std::ostream& operator<<(std::ostream& os, const arkoi::x86_64::Instruction::Opcode& opcode);

/**
 * @brief Streams a detailed description of a `Instruction`.
 *
 * @param os The output stream.
 * @param instruction The instruction to describe.
 * @return A reference to the output stream @p os.
 */
std::ostream& operator<<(std::ostream& os, const arkoi::x86_64::Instruction& instruction);

/**
 * @brief Streams a detailed description of a `AssemblyItem`.
 *
 * @param os The output stream.
 * @param item The assembly item to describe.
 * @return A reference to the output stream @p os.
 */
std::ostream& operator<<(std::ostream& os, const arkoi::x86_64::AssemblyItem& item);

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
