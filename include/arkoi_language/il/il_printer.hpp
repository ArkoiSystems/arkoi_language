#pragma once

#include <sstream>

#include "arkoi_language/il/visitor.hpp"

namespace arkoi::il {
/**
 * @brief Visitor for printing the Intermediate Language (IL)
 */
class ILPrinter final : public Visitor {
public:
    /**
     * @brief Constructs an ILPrinter with the given output stream
     *
     * @param output The output stream to which the IL will be printed
     */
    explicit ILPrinter(std::stringstream& output) :
        _output(output) { }

public:
    /**
     * @brief Prints the IL of a module to a stringstream
     *
     * @param module The module whose IL is to be printed
     *
     * @return A stringstream containing the printed IL
     */
    [[nodiscard]] static std::stringstream print(Module& module);

    /**
     * @brief Visits a Module
     *
     * @param module The module to visit
     */
    void visit(Module& module) override;

    /**
     * @brief Visits a Function
     *
     * @param function The function to visit
     */
    void visit(Function& function) override;

    /**
     * @brief Visits a BasicBlock
     *
     * @param block The basic block to visit
     */
    void visit(BasicBlock& block) override;

    /**
     * @brief Visits a Return instruction
     *
     * @param instruction The instruction to visit
     */
    void visit(Return& instruction) override;

    /**
     * @brief Visits a Binary instruction
     *
     * @param instruction The instruction to visit
     */
    void visit(Binary& instruction) override;

    /**
     * @brief Visits a Cast instruction
     *
     * @param instruction The instruction to visit
     */
    void visit(Cast& instruction) override;

    /**
     * @brief Visits a Call instruction
     *
     * @param instruction The instruction to visit
     */
    void visit(Call& instruction) override;

    /**
     * @brief Visits a Goto instruction
     *
     * @param instruction The instruction to visit
     */
    void visit(Goto& instruction) override;

    /**
     * @brief Visits an If instruction
     *
     * @param instruction The instruction to visit
     */
    void visit(If& instruction) override;

    /**
     * @brief Visits an Alloca instruction
     *
     * @param instruction The instruction to visit
     */
    void visit(Alloca& instruction) override;

    /**
     * @brief Visits a Store instruction
     *
     * @param instruction The instruction to visit
     */
    void visit(Store& instruction) override;

    /**
     * @brief Visits a Load instruction
     *
     * @param instruction The instruction to visit
     */
    void visit(Load& instruction) override;

    /**
     * @brief Visits a Constant instruction
     *
     * @param instruction The instruction to visit
     */
    void visit(Constant& instruction) override;

    /**
     * @brief Returns the output stream
     *
     * @return The output stream
     */
    [[nodiscard]] auto& output() const { return _output; }

private:
    std::stringstream& _output;
};
} // namespace arkoi::il

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
