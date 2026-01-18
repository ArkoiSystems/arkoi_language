#pragma once

#include <unordered_map>

#include "allocator.hpp"
#include "arkoi_language/il/instruction.hpp"
#include "arkoi_language/utils/ordered_set.hpp"
#include "arkoi_language/x86_64/operand.hpp"

namespace arkoi::x86_64 {
/**
 * @brief Represents the arguments of a function call according to the calling convention.
 */
struct CallFrame {
    /**
     * @brief Arguments to be passed in integer registers.
     *
     * Stores pointers to IL arguments that are integer types (e.g., u32, s64)
     * and will be assigned to the first available integer registers according
     * to the calling convention (e.g., RDI, RSI, RDX, etc. on x86-64 SysV ABI).
     */
    std::vector<il::Argument*> integer{ };

    /**
     * @brief Arguments to be passed in floating-point registers.
     *
     * Stores pointers to IL arguments that are floating-point types (e.g., f32, f64)
     * and will be assigned to the first available floating-point registers
     * according to the calling convention (e.g., XMM0–XMM7 on x86-64 SysV ABI).
     */
    std::vector<il::Argument*> floating{ };

    /**
     * @brief Arguments that must be passed on the stack.
     *
     * Stores pointers to IL arguments that cannot fit into registers due to
     * either exceeding the number of available registers or by calling convention rules.
     * These arguments will be pushed onto the stack (in reverse order) before
     * making the function call.
     */
    std::vector<il::Argument*> stack{ };

    /**
     * @brief Total size of the stack portion of the arguments.
     *
     * Represents the total number of bytes that need to be allocated on the
     * stack to hold the stack arguments for this call. This is used to adjust
     * the stack pointer during code generation and ensure proper alignment.
     */
    size_t stack_size{ 0 };
};

/**
 * @brief Visitor that maps abstract IL operands to physical x86-64 machine operands.
 *
 * `Resolver` is responsible for determining the storage location (register or
 * stack slot) for every `il::Operand` in a function. It calculates the necessary
 * stack frame size.
 *
 * @see il::Visitor, il::Operand, Register, Memory, RegisterAllocator
 */
class Resolver final : il::Visitor {
public:
    void run(il::Function& function, const Mapping& mapping);

    [[nodiscard]] auto& mappings() const { return _mappings; }

    /**
     * @brief Retrieves the machine operand associated with a generic IL operand.
     *
     * @param operand The source IL operand.
     * @return The mapped `x86_64::Operand`.
     */
    [[nodiscard]] Operand operator[](const il::Operand& operand) const;

    /**
     * @brief Returns the total size of the stack frame in bytes.
     *
     * This includes space for spilled variables and local allocations.
     *
     * @return The stack size.
     */
    [[nodiscard]] size_t stack_size() const;

    /**
     * @brief Returns all the call frames which got mapped.
     *
     * @return A unordered map of call frames associated with `il::Call` instructions.
     */
    [[nodiscard]] auto& call_frames() const { return _call_frames; }

    /**
     * @brief Rounds up a size to satisfy the x86-64 16-byte stack alignment.
     *
     * @param input The raw size in bytes.
     * @return The aligned size.
     */
    [[nodiscard]] static size_t align_size(size_t input);

private:
    /**
     * @brief Modules are not directly mapped by this visitor.
     *
     * @param module The `il::Module` node to visit.
     */
    void visit([[maybe_unused]] il::Module& module) override;

    /**
     * @brief Maps the function structure and its parameters.
     *
     * @param function The `il::Function` node to visit.
     */
    void visit(il::Function& function) override;

    /**
     * @brief Maps all instructions within a basic block.
     *
     * @param block The `il::BasicBlock` node to visit.
     */
    void visit(il::BasicBlock& block) override;

    /**
     * @brief Maps operands for a binary operation.
     *
     * @param instruction The `il::Binary` node to visit.
     */
    void visit(il::Binary& instruction) override;

    /**
     * @brief Maps operands for a type cast.
     *
     * @param instruction The `il::Cast` node to visit.
     */
    void visit(il::Cast& instruction) override;

    /**
     * @brief Maps the return value operand.
     *
     * @param instruction The `il::Return` node to visit.
     */
    void visit([[maybe_unused]] il::Return& instruction) override { }

    /**
     * @brief Maps the call argument.
     *
     * @param argument The `il::Argument` node to visit.
     */
    void visit(il::Argument& argument) override;

    /**
     * @brief Maps argument operands for a function call.
     *
     * @param instruction The `il::Call` node to visit.
     */
    void visit(il::Call& instruction) override;

    /**
     * @brief Jumps do not define new operand mappings.
     *
     * @param instruction The `il::If` node to visit.
     */
    void visit([[maybe_unused]] il::If& instruction) override { }

    /**
     * @brief Jumps do not define new operand mappings.
     *
     * @param instruction The `il::Goto` node to visit.
     */
    void visit([[maybe_unused]] il::Goto& instruction) override { }

    /**
     * @brief Maps a stack allocation to a specific memory operand.
     *
     * @param instruction The `il::Alloca` node to visit.
     */
    void visit(il::Alloca& instruction) override;

    /**
     * @brief Stores are handled via their source operands.
     *
     * @param instruction The `il::Store` node to visit.
     */
    void visit([[maybe_unused]] il::Store& instruction) override { }

    /**
     * @brief Maps a memory load to a destination operand.
     *
     * @param instruction The `il::Load` node to visit.
     */
    void visit(il::Load& instruction) override;

    /**
     * @brief Maps a Phi instruction.
     *
     * @param instruction The `il::Phi` node to visit.
     */
    void visit(il::Phi& instruction) override;

    /**
     * @brief Maps an Assign instruction.
     *
     * @param instruction The `il::Assign` node to visit.
     */
    void visit(il::Assign& instruction) override;

    /**
     * @brief Registers an IL operand as a local that needs storage.
     *
     * @param operand The operand to track.
     */
    void _add_local(const il::Operand& operand);

    /**
    * @brief Creates a mapping to a relative memory address (on the stack).
     *
     * @param variable The variable which should get mapped.
     * @param memory The target memory location of the mapped variable.
     */
    void _add_memory(const il::Variable& variable, const Memory& memory);

private:
    std::unordered_map<il::Call*, CallFrame> _call_frames{ };
    std::unordered_map<il::Operand, Operand> _mappings{ };
    OrderedSet<il::Operand> _locals{ };
    CallFrame _current_call_frame{ };
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
