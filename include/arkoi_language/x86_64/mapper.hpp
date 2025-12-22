#pragma once

#include <unordered_map>

#include "arkoi_language/il/instruction.hpp"
#include "arkoi_language/utils/ordered_set.hpp"
#include "arkoi_language/x86_64/operand.hpp"

namespace arkoi::x86_64 {
/**
 * @brief Visitor for mapping IL operands to x86_64 assembly operands
 */
class Mapper final : il::Visitor {
public:
    /**
     * @brief Constructs a Mapper for a given function
     *
     * @param function The IL function to map
     */
    explicit Mapper(il::Function& function);

    /**
     * @brief Accesses or creates a mapping for an IL variable
     *
     * @param variable The IL variable
     *
     * @return The corresponding x86_64 operand
     */
    [[nodiscard]] Operand& operator[](const il::Variable& variable);

    /**
     * @brief Returns the x86_64 operand for a given IL operand
     *
     * @param operand The IL operand
     *
     * @return The x86_64 operand
     */
    [[nodiscard]] Operand operator[](const il::Operand& operand);

    /**
     * @brief Returns the total size of the stack frame
     *
     * @return The stack size in bytes
     */
    [[nodiscard]] size_t stack_size() const;

    /**
     * @brief Returns the IL function associated with this mapper
     *
     * @return A reference to the function
     */
    [[nodiscard]] auto& function() const { return _function; }

    /**
     * @brief Returns the register used for return values of the given type
     *
     * @param target The type of the return value
     *
     * @return The return register
     */
    [[nodiscard]] static Register return_register(const sem::Type& target);

    /**
     * @brief Aligns a size to 16 bytes
     *
     * @param input The size to be aligned
     *
     * @return The aligned size
     */
    [[nodiscard]] static size_t align_size(size_t input);

private:
    /**
     * @brief Visits an IL Module (empty implementation)
     */
    void visit(il::Module&) override { }

    /**
     * @brief Visits an IL Function
     *
     * @param function The function to visit
     */
    void visit(il::Function& function) override;

    /**
     * @brief Visits an IL BasicBlock
     *
     * @param block The basic block to visit
     */
    void visit(il::BasicBlock& block) override;

    /**
     * @brief Visits an IL Binary instruction
     *
     * @param instruction The instruction to visit
     */
    void visit(il::Binary& instruction) override;

    /**
     * @brief Visits an IL Cast instruction
     *
     * @param instruction The instruction to visit
     */
    void visit(il::Cast& instruction) override;

    /**
     * @brief Visits an IL Return instruction
     *
     * @param instruction The instruction to visit
     */
    void visit(il::Return& instruction) override;

    /**
     * @brief Visits an IL Call instruction
     *
     * @param instruction The instruction to visit
     */
    void visit(il::Call& instruction) override;

    /**
     * @brief Maps function parameters to registers or stack locations
     *
     * @param parameters The list of parameters
     * @param use_redzone Whether to utilize the x86_64 red zone
     */
    void _map_parameters(const std::vector<il::Variable>& parameters, bool use_redzone);

    /**
     * @brief Visits an IL If instruction (empty implementation)
     */
    void visit(il::If&) override { }

    /**
     * @brief Visits an IL Goto instruction (empty implementation)
     */
    void visit(il::Goto&) override { }

    /**
     * @brief Visits an IL Alloca instruction
     *
     * @param instruction The instruction to visit
     */
    void visit(il::Alloca& instruction) override;

    /**
     * @brief Visits an IL Store instruction (empty implementation)
     */
    void visit(il::Store&) override { }

    /**
     * @brief Visits an IL Load instruction
     *
     * @param instruction The instruction to visit
     */
    void visit(il::Load& instruction) override;

    /**
     * @brief Visits an IL Constant instruction
     *
     * @param instruction The instruction to visit
     */
    void visit(il::Constant& instruction) override;

    /**
     * @brief Adds an operand to the set of locals
     *
     * @param operand The operand to add
     */
    void _add_local(const il::Operand& operand);

    /**
     * @brief Adds a mapping from an IL variable to a register
     *
     * @param variable The IL variable
     * @param reg The x86_64 register
     */
    void _add_register(const il::Variable& variable, const Register& reg);

    /**
     * @brief Adds a mapping from an IL variable to a memory location
     *
     * @param variable The IL variable
     * @param memory The x86_64 memory location
     */
    void _add_memory(const il::Variable& variable, const Memory& memory);

private:
    std::unordered_map<il::Operand, Operand> _mappings{ };
    OrderedSet<il::Operand> _locals{ };
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
