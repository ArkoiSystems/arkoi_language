#pragma once

#include <utility>

#include "arkoi_language/ast/nodes.hpp"
#include "arkoi_language/il/operand.hpp"
#include "arkoi_language/il/visitor.hpp"

namespace arkoi::il {
/**
 * @brief Base class for all IL instructions
 */
class InstructionBase {
public:
    virtual ~InstructionBase() = default;

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    virtual void accept(Visitor& visitor) = 0;

    /**
     * @brief Returns the list of operands defined by this instruction
     *
     * @return A vector of defined operands
     */
    [[nodiscard]] virtual std::vector<Operand> defs() const { return { }; }

    /**
     * @brief Returns the list of operands used by this instruction
     *
     * @return A vector of used operands
     */
    [[nodiscard]] virtual std::vector<Operand> uses() const { return { }; }

    /**
     * @brief Checks if the instruction has a constant value
     *
     * @return True if constant, false otherwise
     */
    [[nodiscard]] virtual bool is_constant() = 0;
};

/**
 * @brief Represents an unconditional jump in the IL
 */
class Goto final : public InstructionBase {
public:
    /**
     * @brief Constructs a Goto instruction
     *
     * @param label The target label
     */
    explicit Goto(std::string label) :
        _label(std::move(label)) { }

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Checks if the instruction is constant
     *
     * @return Always false for Goto
     */
    [[nodiscard]] bool is_constant() override { return false; }

    /**
     * @brief Returns the target label
     *
     * @return A reference to the label string
     */
    [[nodiscard]] auto& label() const { return _label; }

private:
    std::string _label;
};

/**
 * @brief Represents a conditional jump in the IL
 */
class If final : public InstructionBase {
public:
    /**
     * @brief Constructs an If instruction with the given parameters
     *
     * @param condition The condition operand
     * @param next The label of the block to execute if the condition is false
     * @param branch The label of the block to execute if the condition is true
     */
    If(Operand condition, std::string next, std::string branch) :
        _next(std::move(next)), _branch(std::move(branch)), _condition(std::move(condition)) { }

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the operands used by this instruction
     *
     * @return A vector containing the condition operand
     */
    [[nodiscard]] std::vector<Operand> uses() const override { return { _condition }; }

    /**
     * @brief Checks if the instruction is constant
     *
     * @return True if the condition is an immediate value
     */
    [[nodiscard]] bool is_constant() override { return std::holds_alternative<Immediate>(_condition); }

    /**
     * @brief Returns the condition operand
     *
     * @return A reference to the condition operand
     */
    [[nodiscard]] auto& condition() { return _condition; }

    /**
     * @brief Returns the target label for the true branch
     *
     * @return A reference to the label string
     */
    [[nodiscard]] auto& branch() const { return _branch; }

    /**
     * @brief Returns the target label for the false branch
     *
     * @return A reference to the label string
     */
    [[nodiscard]] auto& next() const { return _next; }

private:
    std::string _next, _branch;
    Operand _condition;
};

/**
 * @brief Represents a function call in the IL
 */
class Call final : public InstructionBase {
public:
    /**
     * @brief Constructs a Call instruction with the given parameters
     *
     * @param result The variable where the result will be stored
     * @param name The name of the function to call
     * @param arguments The arguments to the function call
     */
    Call(Variable result, std::string name, std::vector<Operand>&& arguments) :
        _arguments(std::move(arguments)), _name(std::move(name)), _result(std::move(result)) { }

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the operands defined by this instruction
     *
     * @return A vector containing the result variable
     */
    [[nodiscard]] std::vector<Operand> defs() const override { return { _result }; }

    /**
     * @brief Returns the operands used by this instruction
     *
     * @return A vector containing the arguments
     */
    [[nodiscard]] std::vector<Operand> uses() const override { return _arguments; }

    /**
     * @brief Checks if the instruction is constant
     *
     * @return Always false for Call
     */
    [[nodiscard]] bool is_constant() override { return false; }

    /**
     * @brief Returns the arguments to the function call
     *
     * @return A reference to the vector of arguments
     */
    [[nodiscard]] auto& arguments() { return _arguments; }

    /**
     * @brief Returns the result variable
     *
     * @return A reference to the result variable
     */
    [[nodiscard]] auto& result() const { return _result; }

    /**
     * @brief Returns the name of the function
     *
     * @return A reference to the name string
     */
    [[nodiscard]] auto& name() const { return _name; }

private:
    std::vector<Operand> _arguments;
    std::string _name;
    Variable _result;
};

/**
 * @brief Represents a return instruction in the IL
 */
class Return final : public InstructionBase {
public:
    /**
     * @brief Constructs a Return instruction
     *
     * @param value The operand to return
     */
    explicit Return(Operand value) :
        _value(std::move(value)) { }

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the operands used by this instruction
     *
     * @return A vector containing the return value operand
     */
    [[nodiscard]] std::vector<Operand> uses() const override { return { _value }; }

    /**
     * @brief Checks if the instruction is constant
     *
     * @return Always false for Return
     */
    [[nodiscard]] bool is_constant() override { return false; }

    /**
     * @brief Returns the return value operand
     *
     * @return A reference to the operand
     */
    [[nodiscard]] auto& value() { return _value; }

private:
    Operand _value;
};

/**
 * @brief Represents a binary operation in the IL
 */
class Binary final : public InstructionBase {
public:
    /**
     * @brief Enumeration of IL binary operators
     */
    enum class Operator {
        Add,
        Sub,
        Mul,
        Div,
        GreaterThan,
        LessThan,
    };

public:
    /**
     * @brief Constructs a Binary instruction with the given parameters
     *
     * @param result The variable where the result will be stored
     * @param left The left operand
     * @param op The operator
     * @param right The right operand
     * @param op_type The type of the operands
     */
    Binary(Variable result, Operand left, Operator op, Operand right, sem::Type op_type) :
        _left(std::move(left)), _right(std::move(right)), _result(std::move(result)),
        _op_type(std::move(op_type)), _op(op) { }

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the operands defined by this instruction
     *
     * @return A vector containing the result variable
     */
    [[nodiscard]] std::vector<Operand> defs() const override { return { _result }; }

    /**
     * @brief Returns the operands used by this instruction
     *
     * @return A vector containing the left and right operands
     */
    [[nodiscard]] std::vector<Operand> uses() const override { return { _left, _right }; }

    /**
     * @brief Checks if the instruction is constant
     *
     * @return True if both operands are immediate values
     */
    [[nodiscard]] bool is_constant() override {
        return std::holds_alternative<Immediate>(_left) && std::holds_alternative<Immediate>(_right);
    }

    /**
     * @brief Returns the result variable
     *
     * @return A reference to the result variable
     */
    [[nodiscard]] auto& result() const { return _result; }

    /**
     * @brief Returns the right operand
     *
     * @return A reference to the operand
     */
    [[nodiscard]] auto& right() { return _right; }

    /**
     * @brief Returns the left operand
     *
     * @return A reference to the operand
     */
    [[nodiscard]] auto& left() { return _left; }

    /**
     * @brief Returns the operand type
     *
     * @return A reference to the type
     */
    [[nodiscard]] auto& op_type() const { return _op_type; }

    /**
     * @brief Returns the operator
     *
     * @return The operator
     */
    [[nodiscard]] auto& op() const { return _op; }

    /**
     * @brief Converts an AST binary operator to an IL binary operator
     *
     * @param op The AST operator
     *
     * @return The corresponding IL operator
     */
    [[nodiscard]] static Operator node_to_instruction(ast::Binary::Operator op);

private:
    Operand _left, _right;
    Variable _result;
    sem::Type _op_type;
    Operator _op;
};

/**
 * @brief Represents a type cast in the IL
 */
class Cast final : public InstructionBase {
public:
    /**
     * @brief Constructs a Cast instruction with the given parameters
     *
     * @param result The variable where the result will be stored
     * @param source The source operand
     * @param from The source type
     */
    Cast(Variable result, Operand source, sem::Type from) :
        _result(std::move(result)), _source(std::move(source)), _from(std::move(from)) { }

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the operands defined by this instruction
     *
     * @return A vector containing the result variable
     */
    [[nodiscard]] std::vector<Operand> defs() const override { return { _result }; }

    /**
     * @brief Returns the operands used by this instruction
     *
     * @return A vector containing the source operand
     */
    [[nodiscard]] std::vector<Operand> uses() const override { return { _source }; }

    /**
     * @brief Checks if the instruction is constant
     *
     * @return True if the source operand is an immediate value
     */
    [[nodiscard]] bool is_constant() override { return std::holds_alternative<Immediate>(_source); }

    /**
     * @brief Returns the source operand
     *
     * @return A reference to the operand
     */
    [[nodiscard]] auto& source() { return _source; }

    /**
     * @brief Returns the result variable
     *
     * @return A reference to the result variable
     */
    [[nodiscard]] auto& result() const { return _result; }

    /**
     * @brief Returns the source type
     *
     * @return A reference to the type
     */
    [[nodiscard]] auto& from() const { return _from; }

private:
    Variable _result;
    Operand _source;
    sem::Type _from;
};

/**
 * @brief Represents a stack allocation (alloca) in the IL
 */
class Alloca final : public InstructionBase {
public:
    /**
     * @brief Constructs an Alloca instruction
     *
     * @param result The memory location allocated on the stack
     */
    explicit Alloca(Memory result) :
        _result(std::move(result)) { }

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the operands defined by this instruction
     *
     * @return A vector containing the allocated memory
     */
    [[nodiscard]] std::vector<Operand> defs() const override { return { _result }; }

    /**
     * @brief Checks if the instruction is constant
     *
     * @return Always false for Alloca
     */
    [[nodiscard]] bool is_constant() override { return false; }

    /**
     * @brief Returns the result of the alloca
     *
     * @return A reference to the result memory location
     */
    [[nodiscard]] auto& result() const { return _result; }

private:
    Memory _result;
};

/**
 * @brief Represents a memory load instruction in the IL
 */
class Load final : public InstructionBase {
public:
    /**
     * @brief Constructs a Load instruction with the given parameters
     *
     * @param result The variable where the loaded value will be stored
     * @param source The memory location to load from
     */
    Load(Variable result, Memory source) :
        _result(std::move(result)), _source(std::move(source)) { }

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the operands defined by this instruction
     *
     * @return A vector containing the result variable
     */
    [[nodiscard]] std::vector<Operand> defs() const override { return { _result }; }

    /**
     * @brief Returns the operands used by this instruction
     *
     * @return A vector containing the source memory location
     */
    [[nodiscard]] std::vector<Operand> uses() const override { return { _source }; }

    /**
     * @brief Checks if the instruction is constant
     *
     * @return Always false for Load
     */
    [[nodiscard]] bool is_constant() override { return false; }

    /**
     * @brief Returns the result variable
     *
     * @return A reference to the result variable
     */
    [[nodiscard]] auto& result() const { return _result; }

    /**
     * @brief Returns the source memory location
     *
     * @return A reference to the memory location
     */
    [[nodiscard]] auto& source() const { return _source; }

private:
    Variable _result;
    Memory _source;
};

/**
 * @brief Represents a memory store instruction in the IL
 */
class Store final : public InstructionBase {
public:
    /**
     * @brief Constructs a Store instruction with the given parameters
     *
     * @param result The target memory location
     * @param source The operand to store
     */
    Store(Memory result, Operand source) :
        _result(std::move(result)), _source(std::move(source)) { }

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the operands used by this instruction
     *
     * @return A vector containing the source operand
     */
    [[nodiscard]] std::vector<Operand> uses() const override { return { _source }; }

    /**
     * @brief Checks if the instruction is constant
     *
     * @return Always false for Store
     */
    [[nodiscard]] bool is_constant() override { return false; }

    /**
     * @brief Returns the target memory location
     *
     * @return A reference to the memory location
     */
    [[nodiscard]] auto& result() const { return _result; }

    /**
     * @brief Returns the source operand
     *
     * @return A reference to the operand
     */
    [[nodiscard]] auto& source() { return _source; }

private:
    Memory _result;
    Operand _source;
};

/**
 * @brief Represents a constant assignment in the IL
 */
class Constant final : public InstructionBase {
public:
    /**
     * @brief Constructs a Constant instruction with the given parameters
     *
     * @param result The variable where the constant will be stored
     * @param immediate The immediate value
     */
    Constant(Variable result, Immediate immediate) :
        _immediate(std::move(std::move(immediate))), _result(std::move(result)) { }

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the operands defined by this instruction
     *
     * @return A vector containing the result variable
     */
    [[nodiscard]] std::vector<Operand> defs() const override { return { _result }; }

    /**
     * @brief Returns the operands used by this instruction
     *
     * @return A vector containing the immediate value
     */
    [[nodiscard]] std::vector<Operand> uses() const override { return { _immediate }; }

    /**
     * @brief Checks if the instruction is constant
     *
     * @return Always true for Constant
     */
    [[nodiscard]] bool is_constant() override { return true; }

    /**
     * @brief Returns the result variable
     *
     * @return A reference to the result variable
     */
    [[nodiscard]] auto& result() const { return _result; }

    /**
     * @brief Returns the immediate value
     *
     * @return A reference to the immediate value
     */
    [[nodiscard]] auto& immediate() { return _immediate; }

private:
    Immediate _immediate;
    Variable _result;
};

/**
 * @brief Represents a single IL instruction, implemented as a variant of all instruction types
 */
struct Instruction final : InstructionBase, std::variant<
                               Goto, If, Cast, Call, Return,
                               Binary, Alloca, Store, Load, Constant
                           > {
    using variant::variant;

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    void accept(Visitor& visitor) override;

    /**
     * @brief Returns the list of operands defined by this instruction
     *
     * @return A vector of defined operands
     */
    [[nodiscard]] std::vector<Operand> defs() const override;

    /**
     * @brief Returns the list of operands used by this instruction
     *
     * @return A vector of used operands
     */
    [[nodiscard]] std::vector<Operand> uses() const override;

    /**
     * @brief Checks if the instruction is constant
     *
     * @return True if constant, false otherwise
     */
    [[nodiscard]] bool is_constant() override;
};
} // namespace arkoi::il

/**
 * @brief Streams a readable description of a `il::Binary::Operator`
 *
 * @param os Output stream to write to
 * @param op Operand to describe
 *
 * @return Reference to @p os
 */
std::ostream& operator<<(std::ostream& os, const arkoi::il::Binary::Operator& op);

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
