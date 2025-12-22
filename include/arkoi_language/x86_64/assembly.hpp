#pragma once

#include <ostream>
#include <variant>
#include <vector>

#include "arkoi_language/x86_64/operand.hpp"

namespace arkoi::x86_64 {
/**
 * @brief Represents a label in x86_64 assembly
 */
class Label {
public:
    /**
     * @brief Constructs a Label with the given name
     *
     * @param name The name of the label
     */
    explicit Label(std::string name) :
        _name(std::move(name)) { }

    /**
     * @brief Returns the name of the label
     *
     * @return A reference to the name string
     */
    [[nodiscard]] auto& name() const { return _name; }

private:
    std::string _name;
};

/**
 * @brief Represents an assembly directive
 */
class Directive {
public:
    /**
     * @brief Constructs a directive with the given content
     *
     * @param text The text of the directive
     */
    explicit Directive(std::string text) :
        _text(std::move(text)) { }

    /**
     * @brief Returns the text of the directive
     *
     * @return A reference to the text string
     */
    [[nodiscard]] auto& text() const { return _text; }

private:
    std::string _text;
};

/**
 * @brief Represents an x86_64 assembly instruction
 */
class Instruction {
public:
    /**
     * @brief Enumeration of x86_64 opcodes
     */
    enum class Opcode {
        CALL, MOV, SYSCALL, ENTER, LEAVE, RET, ADDSD, ADDSS, ADD, SUBSD, SUBSS, SUB, MULSD, MULSS, IMUL, DIVSD, DIVSS,
        IDIV, DIV, UCOMISD, UCOMISS, SETA, CMP, SETG, SETB, SETL, CVTSS2SD, CVTSD2SS, MOVSXD, MOVSX, MOVZX, CVTTSD2SI,
        CVTTSS2SI, XORPS, SETNE, SETP, OR, CVTSI2SD, CVTSI2SS, TEST, JNZ, JMP, MOVSD, MOVSS
    };

public:
    /**
     * @brief Constructs an Instruction with the given parameters
     *
     * @param opcode The opcode to be used
     * @param operands The operands for the instruction
     */
    Instruction(const Opcode opcode, std::vector<Operand> operands) :
        _operands(std::move(operands)), _opcode(opcode) { }

    /**
     * @brief Returns the operands of the instruction
     *
     * @return A reference to the vector of operands
     */
    [[nodiscard]] auto& operands() const { return _operands; }

    /**
     * @brief Returns the opcode of the instruction
     *
     * @return A reference to the opcode
     */
    [[nodiscard]] auto& opcode() const { return _opcode; }

private:
    std::vector<Operand> _operands;
    Opcode _opcode;
};

/**
 * @brief Represents a single item in an assembly listing (label, directive, or instruction)
 */
struct AssemblyItem final : std::variant<Label, Directive, Instruction> {
    using variant::variant;
};
} // namespace arkoi::x86_64

/**
 * @brief Streams a readable description of a `arkoi::x86_64::Label`
 *
 * @param os Output stream to write to
 * @param label Label to describe
 *
 * @return Reference to @p os
 */
std::ostream& operator<<(std::ostream& os, const arkoi::x86_64::Label& label);

/**
 * @brief Streams a readable description of a `arkoi::x86_64::Directive`
 *
 * @param os Output stream to write to
 * @param directive Directive to describe
 *
 * @return Reference to @p os
 */
std::ostream& operator<<(std::ostream& os, const arkoi::x86_64::Directive& directive);

/**
 * @brief Streams a readable description of a `arkoi::x86_64::Opcode`
 *
 * @param os Output stream to write to
 * @param opcode Opcode to describe
 *
 * @return Reference to @p os
 */
std::ostream& operator<<(std::ostream& os, const arkoi::x86_64::Instruction::Opcode& opcode);

/**
 * @brief Streams a readable description of a `arkoi::x86_64::Instruction`
 *
 * @param os Output stream to write to
 * @param instruction Instruction to describe
 *
 * @return Reference to @p os
 */
std::ostream& operator<<(std::ostream& os, const arkoi::x86_64::Instruction& instruction);

/**
 * @brief Streams a readable description of a `arkoi::x86_64::AssemblyItem`
 *
 * @param os Output stream to write to
 * @param item AssemblyItem to describe
 *
 * @return Reference to @p os
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
