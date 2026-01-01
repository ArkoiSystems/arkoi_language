#pragma once

#include <utility>

#include "arkoi_language/ast/nodes.hpp"
#include "arkoi_language/il/operand.hpp"
#include "arkoi_language/il/visitor.hpp"

namespace arkoi::il {
/**
 * @brief Abstract base class for all Intermediate Language (IL) instructions.
 *
 * Each IL instruction represents a single operation in the Three-Address-ish Code (TAC-ish)
 * style representation. Instructions provide information about which operands
 * they define (write to) and which they use (read from), which is essential
 * for dataflow analysis.
 *
 * @see Visitor, Operand
 */
class InstructionBase {
public:
    virtual ~InstructionBase() = default;

    /**
     * @brief Dispatches the visitor to the concrete instruction implementation.
     *
     * @param visitor The visitor to accept.
     */
    virtual void accept(Visitor& visitor) = 0;

    /**
     * @brief Returns the list of operands that are defined (written) by this instruction.
     *
     * For example, in `x = y + z`, `x` is a defined operand.
     *
     * @return A vector of defined `Operand` objects.
     * @see uses
     */
    [[nodiscard]] virtual std::vector<Operand> defs() const { return { }; }

    /**
     * @brief Returns the list of operands that are used (read) by this instruction.
     *
     * For example, in `x = y + z`, `y` and `z` are used operands.
     *
     * @return A vector of used `Operand` objects.
     * @see defs
     */
    [[nodiscard]] virtual std::vector<Operand> uses() const { return { }; }

    /**
     * @brief Checks if the instruction result is a constant or has constant operands.
     *
     * Useful for optimizations like constant folding.
     *
     * @return True if the instruction is constant-evaluable, false otherwise.
     */
    [[nodiscard]] virtual bool is_constant() const = 0;

    /**
     * @brief Returns the optional source code span associated with this instruction.
     *
     * The span includes the starting and ending positions in the source file.
     * There may be no span attached to this instruction, so this could also be
     * std::nullopt.
     *
     * @return The optional `pretty_diagnostics::Span` of the node.
     */
    [[nodiscard]] virtual std::optional<pretty_diagnostics::Span> span() const = 0;
};

/**
 * @brief Represents an unconditional jump to a target label.
 */
class Goto final : public InstructionBase {
public:
    /**
     * @brief Constructs a `Goto` instruction.
     *
     * @param label The name of the target basic block label.
     * @param span The source location of this instruction.
     */
    explicit Goto(std::string label, std::optional<pretty_diagnostics::Span> span) :
        _span(std::move(span)), _label(std::move(label)) { }

    /**
     * @brief Accepts a visitor to process this `Goto` instruction.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns whether this instruction is constant.
     *
     * @return Always false for `Goto`.
     */
    [[nodiscard]] bool is_constant() const override { return false; }

    /**
     * @brief Returns the optional source code span associated with this instruction.
     *
     * The span includes the starting and ending positions in the source file.
     * There may be no span attached to this instruction, so this could also be
     * std::nullopt.
     *
     * @return The optional `pretty_diagnostics::Span` of the node.
     */
    [[nodiscard]] std::optional<pretty_diagnostics::Span> span() const override { return _span; }

    /**
     * @brief Returns the target label of the jump.
     *
     * @return A constant reference to the label string.
     */
    [[nodiscard]] auto& label() const { return _label; }

private:
    std::optional<pretty_diagnostics::Span> _span;
    std::string _label;
};

/**
 * @brief Represents a conditional jump (if-then-else) in the IL.
 *
 * If the condition evaluates to true, control transfers to `branch`.
 * Otherwise, it transfers to `next`.
 */
class If final : public InstructionBase {
public:
    /**
     * @brief Constructs an `If` instruction.
     *
     * @param condition The operand to evaluate as a boolean.
     * @param next The label to jump to if the condition is false.
     * @param branch The label to jump to if the condition is true.
     * @param span The source location of this instruction.
     */
    If(Operand condition, std::string next, std::string branch, std::optional<pretty_diagnostics::Span> span) :
        _span(std::move(span)), _next(std::move(next)), _branch(std::move(branch)),
        _condition(std::move(condition)) { }

    /**
     * @brief Accepts a visitor to process this `If` instruction.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the operands used by this conditional jump.
     *
     * @return A vector containing the `_condition` operand.
     */
    [[nodiscard]] std::vector<Operand> uses() const override { return { _condition }; }

    /**
     * @brief Checks if the condition is an immediate constant.
     *
     * @return True if `_condition` is an `Immediate`.
     */
    [[nodiscard]] bool is_constant() const override { return std::holds_alternative<Immediate>(_condition); }

    /**
     * @brief Returns the optional source code span associated with this instruction.
     *
     * The span includes the starting and ending positions in the source file.
     * There may be no span attached to this instruction, so this could also be
     * std::nullopt.
     *
     * @return The optional `pretty_diagnostics::Span` of the node.
     */
    [[nodiscard]] std::optional<pretty_diagnostics::Span> span() const override { return _span; }

    /**
     * @brief Returns the condition operand.
     *
     * @return A reference to the `Operand`.
     */
    [[nodiscard]] auto& condition() { return _condition; }

    /**
     * @brief Returns the label for the true branch.
     *
     * @return A constant reference to the `branch` label.
     */
    [[nodiscard]] auto& branch() const { return _branch; }

    /**
     * @brief Returns the label for the false branch (fallthrough).
     *
     * @return A constant reference to the `next` label.
     */
    [[nodiscard]] auto& next() const { return _next; }

private:
    std::optional<pretty_diagnostics::Span> _span;
    std::string _next, _branch;
    Operand _condition;
};

/**
 * @brief Represents a function call in the IL.
 *
 * A `Call` defines a result variable and uses a list of argument operands.
 */
class Call final : public InstructionBase {
public:
    /**
     * @brief Constructs a `Call` instruction.
     *
     * @param result The variable where the function's return value will be stored.
     * @param name The name of the function to be called.
     * @param arguments The list of operands passed as arguments.
     * @param span The source location of this instruction.
     */
    Call(
        Variable result, std::string name, std::vector<Operand>&& arguments,
        std::optional<pretty_diagnostics::Span> span
    ) :
        _span(std::move(span)), _arguments(std::move(arguments)), _name(std::move(name)),
        _result(std::move(result)) { }

    /**
     * @brief Accepts a visitor to process this `Call` instruction.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the result variable defined by the call.
     *
     * @return A vector containing the `_result` variable.
     */
    [[nodiscard]] std::vector<Operand> defs() const override { return { _result }; }

    /**
     * @brief Returns the argument operands used by the call.
     *
     * @return A vector of `_arguments`.
     */
    [[nodiscard]] std::vector<Operand> uses() const override { return _arguments; }

    /**
     * @brief Checks if the call is constant.
     *
     * @return Always false for `Call`.
     */
    [[nodiscard]] bool is_constant() const override { return false; }

    /**
     * @brief Returns the optional source code span associated with this instruction.
     *
     * The span includes the starting and ending positions in the source file.
     * There may be no span attached to this instruction, so this could also be
     * std::nullopt.
     *
     * @return The optional `pretty_diagnostics::Span` of the node.
     */
    [[nodiscard]] std::optional<pretty_diagnostics::Span> span() const override { return _span; }

    /**
     * @brief Returns the list of argument operands.
     *
     * @return A reference to the argument vector.
     */
    [[nodiscard]] auto& arguments() { return _arguments; }

    /**
     * @brief Returns the result variable.
     *
     * @return A constant reference to the `_result` variable.
     */
    [[nodiscard]] auto& result() const { return _result; }

    /**
     * @brief Returns the name of the function being called.
     *
     * @return A constant reference to the function name string.
     */
    [[nodiscard]] auto& name() const { return _name; }

private:
    std::optional<pretty_diagnostics::Span> _span;
    std::vector<Operand> _arguments;
    std::string _name;
    Variable _result;
};

/**
 * @brief Represents a return instruction.
 *
 * Transfers control back to the caller and optionally returns a value.
 */
class Return final : public InstructionBase {
public:
    /**
     * @brief Constructs a `Return` instruction.
     *
     * @param value The operand containing the value to return.
     * @param span The source location of this instruction.
     */
    explicit Return(Operand value, std::optional<pretty_diagnostics::Span> span) :
        _span(std::move(span)), _value(std::move(value)) { }

    /**
     * @brief Accepts a visitor to process this `Return` instruction.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the operand used for the return value.
     *
     * @return A vector containing the `_value` operand.
     */
    [[nodiscard]] std::vector<Operand> uses() const override { return { _value }; }

    /**
     * @brief Checks if the return is constant.
     *
     * @return Always false for `Return`.
     */
    [[nodiscard]] bool is_constant() const override { return false; }

    /**
     * @brief Returns the optional source code span associated with this instruction.
     *
     * The span includes the starting and ending positions in the source file.
     * There may be no span attached to this instruction, so this could also be
     * std::nullopt.
     *
     * @return The optional `pretty_diagnostics::Span` of the node.
     */
    [[nodiscard]] std::optional<pretty_diagnostics::Span> span() const override { return _span; }

    /**
     * @brief Returns the operand being returned.
     *
     * @return A reference to the `_value` operand.
     */
    [[nodiscard]] auto& value() { return _value; }

private:
    std::optional<pretty_diagnostics::Span> _span;
    Operand _value;
};

/**
 * @brief Represents a binary operation between two operands.
 */
class Binary final : public InstructionBase {
public:
    /**
     * @brief Supported binary operators in the IL.
     */
    enum class Operator {
        Add,          ///< '+'
        Sub,          ///< '-'
        Mul,          ///< '*'
        Div,          ///< '/'
        GreaterThan,  ///< '>'
        LessThan,     ///< '<'
        GreaterEqual, ///< '>='
        LessEqual,    ///< '<='
    };

public:
    /**
     * @brief Constructs a `Binary` operation instruction.
     *
     * @param result The variable to store the result of the operation.
     * @param left The left operand.
     * @param op The binary operator to apply.
     * @param right The right operand.
     * @param op_type The semantic type of the operands.
     * @param span The source location of this instruction.
     */
    Binary(
        Variable result, Operand left, const Operator op, Operand right, sem::Type op_type,
        std::optional<pretty_diagnostics::Span> span
    ) :
        _span(std::move(span)), _left(std::move(left)), _right(std::move(right)),
        _op_type(std::move(op_type)), _result(std::move(result)), _op(op) { }

    /**
     * @brief Accepts a visitor to process this `Binary` instruction.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the result variable defined by this operation.
     *
     * @return A vector containing the `_result` variable.
     */
    [[nodiscard]] std::vector<Operand> defs() const override { return { _result }; }

    /**
     * @brief Returns the operands used by this operation.
     *
     * @return A vector containing the `_left` and `_right` operands.
     */
    [[nodiscard]] std::vector<Operand> uses() const override { return { _left, _right }; }

    /**
     * @brief Checks if both operands are immediate constants.
     *
     * @return True if both `_left` and `_right` are `Immediate`.
     */
    [[nodiscard]] bool is_constant() const override {
        return std::holds_alternative<Immediate>(_left) && std::holds_alternative<Immediate>(_right);
    }

    /**
     * @brief Returns the optional source code span associated with this instruction.
     *
     * The span includes the starting and ending positions in the source file.
     * There may be no span attached to this instruction, so this could also be
     * std::nullopt.
     *
     * @return The optional `pretty_diagnostics::Span` of the node.
     */
    [[nodiscard]] std::optional<pretty_diagnostics::Span> span() const override { return _span; }

    /**
     * @brief Returns the result variable.
     *
     * @return A constant reference to the `_result` variable.
     */
    [[nodiscard]] auto& result() const { return _result; }

    /**
     * @brief Returns the right operand.
     *
     * @return A reference to the `_right` operand.
     */
    [[nodiscard]] auto& right() { return _right; }

    /**
     * @brief Returns the left operand.
     *
     * @return A reference to the `_left` operand.
     */
    [[nodiscard]] auto& left() { return _left; }

    /**
     * @brief Returns the type of the operands.
     *
     * @return A constant reference to the `sem::Type`.
     */
    [[nodiscard]] auto& op_type() const { return _op_type; }

    /**
     * @brief Returns the operator.
     *
     * @return A constant reference to the `Operator`.
     */
    [[nodiscard]] auto& op() const { return _op; }

    /**
     * @brief Converts an AST binary operator to its IL equivalent.
     *
     * @param op The `ast::Binary::Operator` from the AST.
     * @return The corresponding `il::Binary::Operator`.
     */
    [[nodiscard]] static Operator node_to_instruction(ast::Binary::Operator op);

private:
    std::optional<pretty_diagnostics::Span> _span;
    Operand _left, _right;
    sem::Type _op_type;
    Variable _result;
    Operator _op;
};

/**
 * @brief Represents a type conversion (cast) instruction.
 */
class Cast final : public InstructionBase {
public:
    /**
     * @brief Constructs a `Cast` instruction.
     *
     * @param result The target variable for the casted value.
     * @param source The operand to be converted.
     * @param from The original semantic type of the operand.
     * @param span The source location of this instruction.
     */
    Cast(Variable result, Operand source, sem::Type from, std::optional<pretty_diagnostics::Span> span) :
        _span(std::move(span)), _result(std::move(result)), _source(std::move(source)),
        _from(std::move(from)) { }

    /**
     * @brief Accepts a visitor to process this `Cast` instruction.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the result variable defined by the cast.
     *
     * @return A vector containing the `_result` variable.
     */
    [[nodiscard]] std::vector<Operand> defs() const override { return { _result }; }

    /**
     * @brief Returns the source operand used for the cast.
     *
     * @return A vector containing the `_source` operand.
     */
    [[nodiscard]] std::vector<Operand> uses() const override { return { _source }; }

    /**
     * @brief Checks if the source operand is an immediate constant.
     *
     * @return True if `_source` is an `Immediate`.
     */
    [[nodiscard]] bool is_constant() const override { return std::holds_alternative<Immediate>(_source); }

    /**
     * @brief Returns the optional source code span associated with this instruction.
     *
     * The span includes the starting and ending positions in the source file.
     * There may be no span attached to this instruction, so this could also be
     * std::nullopt.
     *
     * @return The optional `pretty_diagnostics::Span` of the node.
     */
    [[nodiscard]] std::optional<pretty_diagnostics::Span> span() const override { return _span; }

    /**
     * @brief Returns the source operand.
     *
     * @return A reference to the `_source` operand.
     */
    [[nodiscard]] auto& source() { return _source; }

    /**
     * @brief Returns the target result variable.
     *
     * @return A constant reference to the `_result` variable.
     */
    [[nodiscard]] auto& result() const { return _result; }

    /**
     * @brief Returns the original type of the source operand.
     *
     * @return A constant reference to the `_from` type.
     */
    [[nodiscard]] auto& from() const { return _from; }

private:
    std::optional<pretty_diagnostics::Span> _span;
    Variable _result;
    Operand _source;
    sem::Type _from;
};

/**
 * @brief Represents a stack memory allocation (`alloca`).
 *
 * Allocates space on the stack for a local variable or structure.
 */
class Alloca final : public InstructionBase {
public:
    /**
     * @brief Constructs an `Alloca` instruction.
     *
     * @param result The memory location representing the stack allocation.
     * @param span The source location of this instruction.
     */
    Alloca(Memory result, std::optional<pretty_diagnostics::Span> span) :
        _span(std::move(span)), _result(std::move(result)) { }

    /**
     * @brief Accepts a visitor to process this `Alloca` instruction.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the memory location defined by the allocation.
     *
     * @return A vector containing the `_result` memory operand.
     */
    [[nodiscard]] std::vector<Operand> defs() const override { return { _result }; }

    /**
     * @brief Checks if the allocation is constant.
     *
     * @return Always false for `Alloca`.
     */
    [[nodiscard]] bool is_constant() const override { return false; }

    /**
     * @brief Returns the optional source code span associated with this instruction.
     *
     * The span includes the starting and ending positions in the source file.
     * There may be no span attached to this instruction, so this could also be
     * std::nullopt.
     *
     * @return The optional `pretty_diagnostics::Span` of the node.
     */
    [[nodiscard]] std::optional<pretty_diagnostics::Span> span() const override { return _span; }

    /**
     * @brief Returns the allocated memory location.
     *
     * @return A constant reference to the `_result` memory location.
     */
    [[nodiscard]] auto& result() const { return _result; }

private:
    std::optional<pretty_diagnostics::Span> _span;
    Memory _result;
};

/**
 * @brief Represents a memory load instruction.
 *
 * Reads a value from a memory location into a variable.
 */
class Load final : public InstructionBase {
public:
    /**
     * @brief Constructs a `Load` instruction.
     *
     * @param result The variable to store the loaded value.
     * @param source The memory location to read from.
     * @param span The source location of this instruction.
     */
    Load(Variable result, Memory source, std::optional<pretty_diagnostics::Span> span) :
        _span(std::move(span)), _result(std::move(result)), _source(std::move(source)) { }

    /**
     * @brief Accepts a visitor to process this `Load` instruction.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the result variable defined by the load.
     *
     * @return A vector containing the `_result` variable.
     */
    [[nodiscard]] std::vector<Operand> defs() const override { return { _result }; }

    /**
     * @brief Returns the memory location used by the load.
     *
     * @return A vector containing the `_source` memory operand.
     */
    [[nodiscard]] std::vector<Operand> uses() const override { return { _source }; }

    /**
     * @brief Checks if the load is constant.
     *
     * @return Always false for `Load`.
     */
    [[nodiscard]] bool is_constant() const override { return false; }

    /**
     * @brief Returns the optional source code span associated with this instruction.
     *
     * The span includes the starting and ending positions in the source file.
     * There may be no span attached to this instruction, so this could also be
     * std::nullopt.
     *
     * @return The optional `pretty_diagnostics::Span` of the node.
     */
    [[nodiscard]] std::optional<pretty_diagnostics::Span> span() const override { return _span; }

    /**
     * @brief Returns the target variable.
     *
     * @return A constant reference to the `_result` variable.
     */
    [[nodiscard]] auto& result() const { return _result; }

    /**
     * @brief Returns the source memory location.
     *
     * @return A constant reference to the `_source` memory location.
     */
    [[nodiscard]] auto& source() const { return _source; }

private:
    std::optional<pretty_diagnostics::Span> _span;
    Variable _result;
    Memory _source;
};

/**
 * @brief Represents a memory store instruction.
 *
 * Writes an operand's value into a target memory location.
 */
class Store final : public InstructionBase {
public:
    /**
     * @brief Constructs a `Store` instruction.
     *
     * @param result The target memory location to write to.
     * @param source The operand whose value will be stored.
     * @param span The source location of this instruction.
     */
    Store(Memory result, Operand source, std::optional<pretty_diagnostics::Span> span) :
        _span(std::move(span)), _source(std::move(source)), _result(std::move(result)) { }

    /**
     * @brief Accepts a visitor to process this `Store` instruction.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the operands used for the store.
     *
     * Note: the target memory location is often considered a 'use' of the address.
     *
     * @return A vector containing the `_source` operand.
     */
    [[nodiscard]] std::vector<Operand> uses() const override { return { _source }; }

    /**
     * @brief Checks if the store is constant.
     *
     * @return Always false for `Store`.
     */
    [[nodiscard]] bool is_constant() const override { return false; }

    /**
     * @brief Returns the optional source code span associated with this instruction.
     *
     * The span includes the starting and ending positions in the source file.
     * There may be no span attached to this instruction, so this could also be
     * std::nullopt.
     *
     * @return The optional `pretty_diagnostics::Span` of the node.
     */
    [[nodiscard]] std::optional<pretty_diagnostics::Span> span() const override { return _span; }

    /**
     * @brief Returns the target memory location.
     *
     * @return A constant reference to the `_result` memory location.
     */
    [[nodiscard]] auto& result() const { return _result; }

    /**
     * @brief Returns the source operand.
     *
     * @return A reference to the `_source` operand.
     */
    [[nodiscard]] auto& source() { return _source; }

private:
    std::optional<pretty_diagnostics::Span> _span;
    Operand _source;
    Memory _result;
};

/**
 * @brief Represents a constant assignment to a variable.
 */
class Constant final : public InstructionBase {
public:
    /**
     * @brief Constructs a `Constant` instruction.
     *
     * @param result The variable to store the constant value.
     * @param immediate The literal value to assign.
     * @param span The source location of this instruction.
     */
    Constant(Variable result, Immediate immediate, std::optional<pretty_diagnostics::Span> span) :
        _span(std::move(span)), _immediate(std::move(immediate)), _result(std::move(result)) { }

    /**
     * @brief Accepts a visitor to process this `Constant` instruction.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the result variable defined by this instruction.
     *
     * @return A vector containing the `_result` variable.
     */
    [[nodiscard]] std::vector<Operand> defs() const override { return { _result }; }

    /**
     * @brief Returns the immediate value used by this instruction.
     *
     * @return A vector containing the `_immediate` operand.
     */
    [[nodiscard]] std::vector<Operand> uses() const override { return { _immediate }; }

    /**
     * @brief Checks if the instruction is constant.
     *
     * @return Always true for `Constant`.
     */
    [[nodiscard]] bool is_constant() const override { return true; }

    /**
     * @brief Returns the optional source code span associated with this instruction.
     *
     * The span includes the starting and ending positions in the source file.
     * There may be no span attached to this instruction, so this could also be
     * std::nullopt.
     *
     * @return The optional `pretty_diagnostics::Span` of the node.
     */
    [[nodiscard]] std::optional<pretty_diagnostics::Span> span() const override { return _span; }

    /**
     * @brief Returns the target result variable.
     *
     * @return A constant reference to the `_result` variable.
     */
    [[nodiscard]] auto& result() const { return _result; }

    /**
     * @brief Returns the literal immediate value.
     *
     * @return A reference to the `_immediate` operand.
     */
    [[nodiscard]] auto& immediate() { return _immediate; }

private:
    std::optional<pretty_diagnostics::Span> _span;
    Immediate _immediate;
    Variable _result;
};

class Argument final : public InstructionBase {
public:
    Argument(Variable result, Operand source, std::optional<pretty_diagnostics::Span> span) :
        _span(std::move(span)), _result(std::move(result)), _source(std::move(source)) { }

    /**
     * @brief Accepts a visitor to process this `Parameter` instruction.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Checks if the instruction is constant.
     *
     * @return Always false for `Parameter`.
     */
    [[nodiscard]] bool is_constant() const override { return false; }

    /**
     * @brief Returns the optional source code span associated with this instruction.
     *
     * The span includes the starting and ending positions in the source file.
     * There may be no span attached to this instruction, so this could also be
     * std::nullopt.
     *
     * @return The optional `pretty_diagnostics::Span` of the node.
     */
    [[nodiscard]] std::optional<pretty_diagnostics::Span> span() const override { return _span; }

    /**
     * @brief Returns the result variable defined by this instruction.
     *
     * @return A vector containing the `_result` variable.
     */
    [[nodiscard]] std::vector<Operand> defs() const override { return { _result }; }

    /**
     * @brief Returns the source operand used by this instruction.
     *
     * @return A vector containing the `_source` operand.
     */
    [[nodiscard]] std::vector<Operand> uses() const override { return { _source }; }

    /**
     * @brief Returns the result operand.
     *
     * @return A reference to the `_result` variable.
     */
    [[nodiscard]] auto& result() { return _result; }

    /**
     * @brief Returns the source operand.
     *
     * @return A reference to the `_source` operand.
     */
    [[nodiscard]] auto& source() { return _source; }

private:
    std::optional<pretty_diagnostics::Span> _span;
    Variable _result;
    Operand _source;
};

/**
 * @brief A container for any IL instruction, implemented as a `std::variant`.
 *
 * This allows for type-safe polymorphic handling of instructions without
 * the overhead of virtual calls for every operation, while still supporting
 * the `Visitor` pattern.
 */
struct Instruction final : InstructionBase, std::variant<
                               Goto, If, Cast, Call, Return,
                               Binary, Alloca, Store, Load, Constant,
                               Argument
                           > {
    using variant::variant;

    /**
     * @brief Dispatches the visitor to the underlying instruction type.
     */
    void accept(Visitor& visitor) override;

    /**
     * @brief Forwards the `defs` call to the underlying instruction.
     */
    [[nodiscard]] std::vector<Operand> defs() const override;

    /**
     * @brief Forwards the `uses` call to the underlying instruction.
     */
    [[nodiscard]] std::vector<Operand> uses() const override;

    /**
     * @brief Forwards the `is_constant` call to the underlying instruction.
     */
    [[nodiscard]] bool is_constant() const override;

    /**
     * @brief Forwards the `span` call to the underlying instruction.
     */
    [[nodiscard]] std::optional<pretty_diagnostics::Span> span() const override;
};
} // namespace arkoi::il

/**
 * @brief Streams a detailed description of a `Binary::Operator`.
 *
 * @param os The output stream.
 * @param op The binary operator to describe.
 * @return A reference to the output stream @p os
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
