#pragma once

#include <unordered_map>

#include "register_allocation.hpp"
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
    std::vector<il::Argument *> integer{ };

    /**
     * @brief Arguments to be passed in floating-point registers.
     *
     * Stores pointers to IL arguments that are floating-point types (e.g., f32, f64)
     * and will be assigned to the first available floating-point registers
     * according to the calling convention (e.g., XMM0–XMM7 on x86-64 SysV ABI).
     */
    std::vector<il::Argument *> floating{ };

    /**
     * @brief Arguments that must be passed on the stack.
     *
     * Stores pointers to IL arguments that cannot fit into registers due to
     * either exceeding the number of available registers or by calling convention rules.
     * These arguments will be pushed onto the stack (in reverse order) before
     * making the function call.
     */
    std::vector<il::Argument *> stack{ };

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
 * `Mapper` is responsible for determining the storage location (register or
 * stack slot) for every `il::Operand` in a function. It calculates the necessary
 * stack frame size and handles calling convention details like parameter passing.
 *
 * @see il::Visitor, il::Operand, Register, Memory, RegisterAllocater
 */
class Mapper final : il::Visitor {
public:
    /**
     * @brief Constructs a `Mapper` for the specified IL function.
     *
     * @param function The `il::Function` to process.
     */
    explicit Mapper(il::Function& function);

    /**
     * @brief Retrieves the machine operand associated with a generic IL operand.
     *
     * @param operand The source IL operand.
     * @return The mapped `x86_64::Operand`.
     */
    [[nodiscard]] Operand operator[](const il::Operand& operand);

    /**
     * @brief Returns the total size of the stack frame in bytes.
     *
     * This includes space for spilled variables and local allocations.
     *
     * @return The stack size.
     */
    [[nodiscard]] size_t stack_size() const;

    /**
     * @brief Returns the IL function associated with this mapper.
     *
     * @return A constant reference to the `il::Function`.
     */
    [[nodiscard]] auto& function() const { return _function; }

    /**
     * @brief Returns all the call frames which got mapped.
     *
     * @return A unordered map of call frames associated with `il::Call` instructions.
     */
    [[nodiscard]] auto &call_frames() { return _call_frames; }

    /**
     * @brief Returns the register allocator used for this mapping.
     *
     * @return A reference to the instance of the `RegisterAllocator`.
     */
    [[nodiscard]] auto &register_allocater() { return _register_allocator; }

    /**
     * @brief Determines the physical register used for returning a specific type.
     *
     * For example, integers are typically returned in `RAX`.
     *
     * @param target The semantic type being returned.
     * @return The corresponding `Register`.
     */
    [[nodiscard]] static Register return_register(const sem::Type& target);

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
     * @brief Places function parameters into their initial registers or stack slots.
     *
     * @param parameters The formal parameters of the function.
     */
    void _map_parameters(const std::vector<il::Variable>& parameters);

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
    void visit(il::Return& instruction) override;

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
     * @brief Maps a constant assignment.
     *
     * @param instruction The `il::Constant` node to visit.
     */
    void visit(il::Constant& instruction) override;

    /**
     * @brief Maps a Phi instruction.
     *
     * @param instruction The `il::Phi` node to visit.
     */
    void visit([[maybe_unused]] il::Phi& instruction) override { }

    /**
     * @brief Registers an IL operand as a local that needs storage.
     *
     * @param operand The operand to track.
     */
    void _add_local(const il::Operand& operand);

    /**
     * @brief Creates a mapping to a physical machine register.
     *
     * @param variable The variable which should get mapped.
     * @param reg The target register of the mapped variable.
     */
    void _add_register(const il::Variable& variable, const Register& reg);

    /**
    * @brief Creates a mapping to a relative memory address (on the stack).
     *
     * @param variable The variable which should get mapped.
     * @param memory The target memory location of the mapped variable.
     */
    void _add_memory(const il::Variable& variable, const Memory& memory);

private:
    std::unordered_map<il::Call *, CallFrame> _call_frames{ };
    std::unordered_map<il::Operand, Operand> _mappings{ };
    RegisterAllocater _register_allocator;
    OrderedSet<il::Operand> _locals{ };
    CallFrame _current_call_frame{ };
    il::Function& _function;
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
