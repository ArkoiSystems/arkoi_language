#pragma once

namespace arkoi::ast {
class Identifier;
class Parameter;
class Immediate;
class Variable;
class Function;
class Program;
class Assign;
class Return;
class Binary;
class Block;
class Cast;
class Call;
class If;

/**
 * @brief Base class for AST visitors
 */
class Visitor {
public:
    virtual ~Visitor() = default;

    /**
     * @brief Visits a Program node
     *
     * @param node The Program node to visit
     */
    virtual void visit(Program& node) = 0;

    /**
     * @brief Visits a Function node
     *
     * @param node The Function node to visit
     */
    virtual void visit(Function& node) = 0;

    /**
     * @brief Visits a Block node
     *
     * @param node The Block node to visit
     */
    virtual void visit(Block& node) = 0;

    /**
     * @brief Visits a Parameter node
     *
     * @param node The Parameter node to visit
     */
    virtual void visit(Parameter& node) = 0;

    /**
     * @brief Visits an Immediate node
     *
     * @param node The Immediate node to visit
     */
    virtual void visit(Immediate& node) = 0;

    /**
     * @brief Visits a Variable node
     *
     * @param node The Variable node to visit
     */
    virtual void visit(Variable& node) = 0;

    /**
     * @brief Visits a Return node
     *
     * @param node The Return node to visit
     */
    virtual void visit(Return& node) = 0;

    /**
     * @brief Visits an Identifier node
     *
     * @param node The Identifier node to visit
     */
    virtual void visit(Identifier& node) = 0;

    /**
     * @brief Visits a Binary node
     *
     * @param node The Binary node to visit
     */
    virtual void visit(Binary& node) = 0;

    /**
     * @brief Visits a Cast node
     *
     * @param node The Cast node to visit
     */
    virtual void visit(Cast& node) = 0;

    /**
     * @brief Visits an Assign node
     *
     * @param node The Assign node to visit
     */
    virtual void visit(Assign& node) = 0;

    /**
     * @brief Visits a Call node
     *
     * @param node The Call node to visit
     */
    virtual void visit(Call& node) = 0;

    /**
     * @brief Visits an If node
     *
     * @param node The If node to visit
     */
    virtual void visit(If& node) = 0;
};
} // namespace arkoi::ast

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
