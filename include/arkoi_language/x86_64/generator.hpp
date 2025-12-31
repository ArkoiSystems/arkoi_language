#pragma once

#include <sstream>

#include "arkoi_language/il/instruction.hpp"
#include "arkoi_language/x86_64/assembly.hpp"
#include "arkoi_language/x86_64/mapper.hpp"

namespace arkoi::x86_64 {
/**
 * @brief Categorizes function arguments into registers and stack slots.
 *
 * Following the x86-64 System V ABI, arguments are passed in specific
 * registers (integer or SSE) if available; otherwise they are spilled
 * to the stack.
 */
struct ClassifiedArguments {
    std::vector<il::Operand> floating{ }; ///< Arguments passed in XMM registers.
    std::vector<il::Operand> integer{ };  ///< Arguments passed in general-purpose registers.
    std::vector<il::Operand> stack{ };    ///< Arguments passed on the stack.
};

/**
 * @brief Visitor that generates x86-64 assembly from the Intermediate Language (IL).
 *
 * `Generator` translates `il::Module` into machine-specific assembly instructions.
 * It manages register usage, stack frame setup/teardown (prologue/epilogue),
 * and handles calling convention details for both function definitions and calls.
 *
 * @see il::Visitor, Mapper, RegisterAllocater, AssemblyItem
 */
class Generator final : il::Visitor {
public:
    /**
     * @brief Constructs an x86-64 code generator for the given module.
     *
     * @param module The `il::Module` to be translated.
     */
    Generator(const std::shared_ptr<pretty_diagnostics::Source>& source, il::Module& module);

    /**
     * @brief Finalizes generation and returns the assembly source as a stream.
     *
     * @return A `std::stringstream` containing the complete assembly listing.
     */
    [[nodiscard]] std::stringstream output() const;

private:
    /**
     * @brief Translates an entire IL module.
     *
     * @param module The `il::Module` node to visit.
     */
    void visit(il::Module& module) override;

    /**
     * @brief Generates code for a single function, including prologue and epilogue.
     *
     * @param function The `il::Function` node to visit.
     */
    void visit(il::Function& function) override;

    /**
     * @brief Translates a basic block into a sequence of assembly instructions.
     *
     * @param block The `il::BasicBlock` node to visit.
     */
    void visit(il::BasicBlock& block) override;

    /**
     * @brief Returns are handled by the epilogue logic in `visit(il::Function)`.
     *
     * @param instruction The `il::Return` node to visit.
     */
    void visit([[maybe_unused]] il::Return& instruction) override { }

    /**
    * @brief Translates an IL binary operation into machine code.
     *
     * @param instruction The `il::Binary` node to visit.
     */
    void visit(il::Binary& instruction) override;

    /**
     * @brief Emits machine code for integer or floating-point addition.
     *
     * @param result The operand in which the result should be stored.
     * @param left The left-hand operand to perform the addition.
     * @param right The right-hand operand to perform the addition.
     * @param type The type of the operands.
     */
    void _add(const Operand& result, Operand left, const Operand& right, const sem::Type& type);

    /**
     * @brief Emits machine code for integer or floating-point subtraction.
     *
     * @param result The operand in which the result should be stored.
     * @param left The left-hand operand to perform the subtraction.
     * @param right The right-hand operand to perform the subtraction.
     * @param type The type of the operands.
     */
    void _sub(const Operand& result, Operand left, const Operand& right, const sem::Type& type);

    /**
     * @brief Emits machine code for integer or floating-point multiplication.
     *
     * @param result The operand in which the result should be stored.
     * @param left The left-hand operand to perform the multiplication.
     * @param right The right-hand operand to perform the multiplication.
     * @param type The type of the operands.
     */
    void _mul(const Operand& result, Operand left, const Operand& right, const sem::Type& type);

    /**
     * @brief Emits machine code for integer or floating-point division.
     *
     * @param result The operand in which the result should be stored.
     * @param left The left-hand operand to perform the division.
     * @param right The right-hand operand to perform the division.
     * @param type The type of the operands.
     */
    void _div(const Operand& result, Operand left, Operand right, const sem::Type& type);

    /**
     * @brief Emits machine code for greater-than comparison.
     *
     * @param result The operand in which the result should be stored.
     * @param left The left-hand operand to perform the greater-than.
     * @param right The right-hand operand to perform the greater-than.
     * @param type The type of the operands.
     */
    void _gth(const Operand& result, Operand left, const Operand& right, const sem::Type& type);

    /**
     * @brief Emits machine code for less-than comparison.
     *
     * @param result The operand in which the result should be stored.
     * @param left The left-hand operand to perform the less-than.
     * @param right The right-hand operand to perform the less-than.
     * @param type The type of the operands.
     */
    void _lth(const Operand& result, Operand left, const Operand& right, const sem::Type& type);

    /**
     * @brief Translates an IL type cast into machine code.
     *
     * @param instruction The `il::Cast` node to visit.
     */
    void visit(il::Cast& instruction) override;

    /**
     * @brief Converts between different floating-point precisions (e.g., f32 to f64).
     *
     * @param result The operand in which the result should be stored.
     * @param source The source operand, which should get converted to a float.
     * @param from The from-type that should be changed @p to.
     * @param to The target to-type of the cast.
     */
    void _float_to_float(const Operand& result, Operand source, const sem::Floating& from, const sem::Floating& to);

    /**
     * @brief Converts between different integer widths or signedness.
     *
     * @param result The operand in which the result should be stored.
     * @param source The source operand, which should get converted to an integer.
     * @param from The from-type that should be changed @p to.
     * @param to The target to-type of the cast.
     */
    void _int_to_int(const Operand& result, Operand source, const sem::Integral& from, const sem::Integral& to);

    /**
     * @brief Converts a floating-point value to an integer (truncation).
     *
     * @param result The operand in which the result should be stored.
     * @param source The source operand, which should get converted to an integer.
     * @param from The from-type that should be changed @p to.
     * @param to The target to-type of the cast.
     */
    void _float_to_int(const Operand& result, const Operand& source, const sem::Floating& from, const sem::Integral& to);

    /**
     * @brief Converts a floating-point value to a boolean (comparison against zero).
     *
     * @param result The operand in which the result should be stored.
     * @param source The source operand, which should get converted to a boolean.
     * @param from The from-type that should be changed @p to.
     * @param to The target to-type of the cast.
     */
    void _float_to_bool(const Operand& result, const Operand& source, const sem::Floating& from, const sem::Boolean& to);

    /**
     * @brief Converts an integer to a floating-point value.
     *
     * @param result The operand in which the result should be stored.
     * @param source The source operand, which should get converted to a float.
     * @param from The from-type that should be changed @p to.
     * @param to The target to-type of the cast.
     */
    void _int_to_float(const Operand& result, Operand source, const sem::Integral& from, const sem::Floating& to);

    /**
     * @brief Converts an integer to a boolean (non-zero check).
     *
     * @param result The operand in which the result should be stored.
     * @param source The source operand, which should get converted to a boolean.
     * @param from The from-type that should be changed @p to.
     * @param to The target to-type of the cast.
     */
    void _int_to_bool(const Operand& result, Operand source, const sem::Integral& from, const sem::Boolean& to);

    /**
     * @brief Converts a boolean to a floating-point value (0.0 or 1.0).
     *
     * @param result The operand in which the result should be stored.
     * @param source The source operand, which should get converted to a float.
     * @param from The from-type that should be changed @p to.
     * @param to The target to-type of the cast.
     */
    void _bool_to_float(const Operand& result, Operand source, const sem::Boolean& from, const sem::Floating& to);

    /**
     * @brief Converts a boolean to an integer (0 or 1).
     *
     * @param result The operand in which the result should be stored.
     * @param source The source operand, which should get converted to an integer.
     * @param from The from-type that should be changed @p to.
     * @param to The target to-type of the cast.
     */
    void _bool_to_int(const Operand& result, Operand source, const sem::Boolean& from, const sem::Integral& to);

    /**
     * @brief This translation is getting done in `il::Call`.
     *
     * @param instruction The `il::Argument` node to visit.
     */
    void visit([[maybe_unused]] il::Argument& instruction) override { }

    /**
     * @brief Translates a function call into machine code, adhering to the ABI.
     *
     * @param instruction The `il::Call` node to visit.
     */
    void visit(il::Call& instruction) override;

    /**
     * @brief Translates a function call argument into machine code.
     *
     * @param argument The `il::Argument` to translate.
     */
    void _generate_argument(il::Argument &argument);

    /**
     * @brief Translates a conditional jump into machine code.
     *
     * @param instruction The `il::If` node to visit.
     */
    void visit(il::If& instruction) override;

    /**
     * @brief Translates an unconditional jump into machine code.
     *
     * @param instruction The `il::Goto` node to visit.
     */
    void visit(il::Goto& instruction) override;

    /**
     * @brief Stack allocations are handled by the prologue and `Mapper`.
     *
     * @param instruction The `il::Alloca` node to visit.
     */
    void visit([[maybe_unused]] il::Alloca& instruction) override { }

    /**
     * @brief Translates an IL memory store into a machine `MOV` or `MOVSS/SD`.
     *
     * @param instruction The `il::Store` node to visit.
     */
    void visit(il::Store& instruction) override;

    /**
     * @brief Translates an IL memory load into machine code.
     *
     * @param instruction The `il::Load` node to visit.
     */
    void visit(il::Load& instruction) override;

    /**
     * @brief Translates a constant assignment into machine code.
     *
     * @param instruction The `il::Constant` node to visit.
     */
    void visit(il::Constant& instruction) override;

    /**
     * @brief Resolves an IL operand to its machine location via the `Mapper`.
     *
     * @param operand The input IL operand.
     * @return The resulting x86_64 operand.
     */
    Operand _load(const il::Operand& operand);

    /**
     * @brief Emits a move-like instruction between two machine operands.
     *
     * @param source The source operand.
     * @param destination The destination operand.
     * @param type The type of the value being stored.
     */
    void _store(Operand source, const Operand& destination, const sem::Type& type);

    /**
     * @brief Ensures a machine operand is in a register, emitting a move if necessary.
     *
     * @param target The target that should be in a register.
     * @param type The type of the register in the case of a temporary store.
     * @return The resulting register the target is kept in.
     */
    [[nodiscard]] Register _adjust_to_reg(const Operand& target, const sem::Type& type);

    /**
     * @brief Moves an operand into the primary scratch register (e.g., RAX/XMM0).
     *
     * @param source The source operand that should be stored in the first temporary register.
     * @param type The target type of the register the @p source should be put in.
     * @return The resulting register the @p source was put in.
     */
    [[nodiscard]] Register _store_temp_1(const Operand& source, const sem::Type& type);

    /**
     * @brief Returns the primary scratch register base for a given type.
     *
     * @param type The type of register that should be returned.
     * @return The first temporary register of the type asked for.
     */
    [[nodiscard]] static Register _temp_1_register(const sem::Type& type);

    /**
     * @brief Moves an operand into the secondary scratch register (e.g., R10/XMM1).
     *
     * @param source The source operand that should be stored in the second temporary register.
     * @param type The target type of the register the @p source should be put in.
     * @return The resulting register the @p source was put in.
     */
    [[nodiscard]] Register _store_temp_2(const Operand& source, const sem::Type& type);

    /**
    * @brief Returns the secondary scratch register base for a given type.
     *
     * @param type The type of register that should be returned.
     * @return The second temporary register of the type asked for.
     */
    [[nodiscard]] static Register _temp_2_register(const sem::Type& type);

    /**
     * @brief Adds a directive with the specified text into the given output.
     *
     * @param directive The text of the directive.
     * @param output The output in which the directive will be placed in.
     */
    static void _directive(const std::string& directive, std::vector<AssemblyItem>& output);

    /**
     * @brief Emits a ".loc" directive that refers to the file and the given
     *        span location.
     *
     * @param instruction The given `il::Instruction` for the debug line output.
     */
    void _debug_line(const il::Instruction& instruction);

    /**
     * @brief Emplace a label.
     *
     * @param name The name of the label.
     */
    void _label(const std::string& name);

    /**
     * @brief Emplace an unconditional jump.
     *
     * @param name The name of the target label.
     */
    void _jmp(const std::string& name);

    /**
     * @brief Emplace a jump if not zero.
     *
     * @param name The name of the target label.
     */
    void _jnz(const std::string& name);

    /**
     * @brief Emplace a function call.
     *
     * @param name The name of the target function.
     */
    void _call(const std::string& name);

    /**
     * @brief Emplace a MOVSXD instruction (sign-extend move).
     *
     * @param destination The destination operand.
     * @param source The source operand.
     */
    void _movsxd(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace a MOVSD instruction (scalar double-precision move).
     *
     * @param destination The destination operand.
     * @param source The source operand.
     */
    void _movsd(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace a MOVSS instruction (scalar single-precision move).
     *
     * @param destination The destination operand.
     * @param source The source operand.
     */
    void _movss(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace a MOVZX instruction (zero-extend move).
     *
     * @param destination The destination operand.
     * @param source The source operand.
     */
    void _movzx(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace a MOVSX instruction (sign-extend move).
     *
     * @param destination The destination operand.
     * @param source The source operand.
     */
    void _movsx(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace a MOV instruction.
     *
     * @param destination The destination operand.
     * @param source The source operand.
     */
    void _mov(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace a PUSH instruction.
     *
     * @param source The source operand.
     */
    void _push(const Operand& source);

    /**
     * @brief Emplace a POP instruction.
     *
     * @param destination The destination operand.
     */
    void _pop(const Operand& destination);

    /**
     * @brief Emplace an ADDSD instruction (scalar double-precision add).
     *
     * @param destination The destination operand.
     * @param source The source operand.
     */
    void _addsd(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace an ADDSS instruction (scalar single-precision add).
     *
     * @param destination The destination operand.
     * @param source The source operand.
     */
    void _addss(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace an ADD instruction.
     *
     * @param destination The destination operand.
     * @param source The source operand.
     */
    void _add(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace a SUBSD instruction (scalar double-precision subtract).
     *
     * @param destination The destination operand.
     * @param source The source operand.
     */
    void _subsd(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace a SUBSS instruction (scalar single-precision subtract).
     *
     * @param destination The destination operand.
     * @param source The source operand.
     */
    void _subss(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace a SUB instruction.
     *
     * @param destination The destination operand.
     * @param source The source operand.
     */
    void _sub(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace a MULSD instruction (scalar double-precision multiply).
     *
     * @param destination The destination operand.
     * @param source The source operand.
     */
    void _mulsd(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace a MULSS instruction (scalar single-precision multiply).
     *
     * @param destination The destination operand.
     * @param source The source operand.
     */
    void _mulss(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace an IMUL instruction (signed integer multiply).
     *
     * @param destination The destination operand.
     * @param source The source operand.
     */
    void _imul(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace a DIVSD instruction (scalar double-precision divide).
     *
     * @param destination The destination operand.
     * @param source The source operand.
     */
    void _divsd(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace a DIVSS instruction (scalar single-precision divide).
     *
     * @param destination The destination operand.
     * @param source The source operand.
     */
    void _divss(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace an IDIV instruction (signed integer division).
     *
     * @param source The divisor operand.
     */
    void _idiv(const Operand& source);

    /**
     * @brief Emplace a UDIV instruction (unsigned integer division).
     *
     * @param source The divisor operand.
     */
    void _udiv(const Operand& source);

    /**
     * @brief Emplace a XORPS instruction (bitwise XOR on packed single-precision values).
     *
     * @param destination The destination operand.
     * @param source The source operand.
     */
    void _xorps(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace an OR instruction.
     *
     * @param destination The destination operand.
     * @param source The source operand.
     */
    void _or(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace a UCOMISD instruction (unordered compare scalar double-precision).
     *
     * @param destination The first operand.
     * @param source The second operand.
     */
    void _ucomisd(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace a UCOMISS instruction (unordered compare scalar single-precision).
     *
     * @param destination The first operand.
     * @param source The second operand.
     */
    void _ucomiss(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace a CVTSD2SS instruction (convert double to single precision).
     *
     * @param destination The destination operand.
     * @param source The source operand.
     */
    void _cvtsd2ss(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace a CVTSS2SD instruction (convert single to double precision).
     *
     * @param destination The destination operand.
     * @param source The source operand.
     */
    void _cvtss2sd(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace a CVTSI2SD instruction (convert integer to double precision).
     *
     * @param destination The destination operand.
     * @param source The source operand.
     */
    void _cvtsi2sd(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace a CVTSI2SS instruction (convert integer to single precision).
     *
     * @param destination The destination operand.
     * @param source The source operand.
     */
    void _cvtsi2ss(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace a CVTTSD2SI instruction (truncate double to integer).
     *
     * @param destination The destination operand.
     * @param source The source operand.
     */
    void _cvttsd2si(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace a CVTTSS2SI instruction (truncate single to integer).
     *
     * @param destination The destination operand.
     * @param source The source operand.
     */
    void _cvttss2si(const Operand& destination, const Operand& source);

    /**
     * @brief Emplace a TEST instruction.
     *
     * @param first The first operand.
     * @param second The second operand.
     */
    void _test(const Operand& first, const Operand& second);

    /**
     * @brief Emplace a CMP instruction.
     *
     * @param first The first operand.
     * @param second The second operand.
     */
    void _cmp(const Operand& first, const Operand& second);

    /**
     * @brief Emplace a SETNE instruction (set byte if not equal).
     *
     * @param destination The destination operand.
     */
    void _setne(const Operand& destination);

    /**
     * @brief Emplace a SETG instruction (set byte if greater, signed).
     *
     * @param destination The destination operand.
     */
    void _setg(const Operand& destination);

    /**
     * @brief Emplace a SETA instruction (set byte if above, unsigned).
     *
     * @param destination The destination operand.
     */
    void _seta(const Operand& destination);

    /**
     * @brief Emplace a SETB instruction (set byte if below, unsigned).
     *
     * @param destination The destination operand.
     */
    void _setb(const Operand& destination);

    /**
     * @brief Emplace a SETL instruction (set byte if less, signed).
     *
     * @param destination The destination operand.
     */
    void _setl(const Operand& destination);

    /**
     * @brief Emplace a SETP instruction (set byte if parity).
     *
     * @param destination The destination operand.
     */
    void _setp(const Operand& destination);

    /**
     * @brief Emplace an ENTER instruction (create stack frame).
     *
     * @param size The size of the stack frame.
     */
    void _enter(uint16_t size);

    /**
     * @brief Emplace a SYSCALL instruction.
     */
    void _syscall();

    /**
     * @brief Emplace a LEAVE instruction (destroy stack frame).
     */
    void _leave();

    /**
     * @brief Emplace a RET instruction.
     */
    void _ret();

    /**
     * @brief Emplace a newline into the given output.
     *
     * @param output The output in which the newline will be placed in.
     */
    static void _newline(std::vector<AssemblyItem>& output);

    /**
     * @brief Returns the current mapper which must be set at calling
     *        time.
     *
     * @return Returns the current mapper.
     */
    [[nodiscard]] Mapper& current_mapper() const;

private:
    std::optional<pretty_diagnostics::Span> _debug_span{ };
    std::shared_ptr<pretty_diagnostics::Source> _source;
    std::unique_ptr<Mapper> _current_mapper{ };
    std::vector<AssemblyItem> _data{ };
    std::vector<AssemblyItem> _text{ };
    size_t _constants{ };
    il::Module& _module;
};
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
