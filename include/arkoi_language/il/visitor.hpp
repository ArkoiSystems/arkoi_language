#pragma once

namespace arkoi::il {
class BasicBlock;
class Function;
class Constant;
class Module;
class Binary;
class Return;
class Alloca;
class Store;
class Load;
class Cast;
class Call;
class Goto;
class If;

/**
 * @brief Base class for IL visitors
 */
class Visitor {
public:
    virtual ~Visitor() = default;

    /**
     * @brief Visits a Module
     *
     * @param module The module to visit
     */
    virtual void visit(Module& module) = 0;

    /**
     * @brief Visits a Function
     *
     * @param function The function to visit
     */
    virtual void visit(Function& function) = 0;

    /**
     * @brief Visits a BasicBlock
     *
     * @param block The basic block to visit
     */
    virtual void visit(BasicBlock& block) = 0;

    /**
     * @brief Visits a Return instruction
     *
     * @param instruction The instruction to visit
     */
    virtual void visit(Return& instruction) = 0;

    /**
     * @brief Visits a Binary instruction
     *
     * @param instruction The instruction to visit.
     */
    virtual void visit(Binary& instruction) = 0;

    /**
     * @brief Visits a Cast instruction
     *
     * @param instruction The instruction to visit
     */
    virtual void visit(Cast& instruction) = 0;

    /**
     * @brief Visits a Call instruction
     *
     * @param instruction The instruction to visit
     */
    virtual void visit(Call& instruction) = 0;

    /**
     * @brief Visits an If instruction
     *
     * @param instruction The instruction to visit
     */
    virtual void visit(If& instruction) = 0;

    /**
     * @brief Visits a Goto instruction
     *
     * @param instruction The instruction to visit
     */
    virtual void visit(Goto& instruction) = 0;

    /**
     * @brief Visits an Alloca instruction
     *
     * @param instruction The instruction to visit
     */
    virtual void visit(Alloca& instruction) = 0;

    /**
     * @brief Visits a Store instruction
     *
     * @param instruction The instruction to visit
     */
    virtual void visit(Store& instruction) = 0;

    /**
     * @brief Visits a Load instruction
     *
     * @param instruction The instruction to visit
     */
    virtual void visit(Load& instruction) = 0;

    /**
     * @brief Visits a Constant instruction
     *
     * @param instruction The instruction to visit
     */
    virtual void visit(Constant& instruction) = 0;
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
