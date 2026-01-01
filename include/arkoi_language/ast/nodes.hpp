#pragma once

#include <utility>

#include "arkoi_language/ast/visitor.hpp"
#include "arkoi_language/front/token.hpp"
#include "arkoi_language/sem/symbol_table.hpp"
#include "arkoi_language/sem/type.hpp"

#include "pretty_diagnostics/span.hpp"

namespace arkoi::ast {
/**
 * @brief Abstract base class for all nodes in the Abstract Syntax Tree (AST).
 *
 * Each node in the AST represents a construct in the source code (e.g., a statement,
 * an expression, a function definition). All nodes must implement the `accept`
 * method for the visitor pattern and provide their source code `span`.
 *
 * @see Visitor
 */
class Node {
public:
    virtual ~Node() = default;

    /**
     * @brief Dispatches the visitor to the concrete node implementation.
     *
     * This is the entry point for the Visitor pattern.
     *
     * @param visitor The visitor to accept.
     */
    virtual void accept(Visitor& visitor) = 0;

    /**
     * @brief Returns the source code span associated with this node.
     *
     * The span includes the starting and ending positions in the source file.
     *
     * @return The `pretty_diagnostics::Span` of the node.
     */
    [[nodiscard]] virtual pretty_diagnostics::Span span() const = 0;
};

/**
 * @brief Represents the top-level unit of a source file.
 *
 * A `Program` consists of a sequence of global statements (e.g., function definitions)
 * and holds the root symbol table for the entire compilation unit.
 */
class Program final : public Node {
public:
    /**
     * @brief Constructs a `Program` node.
     *
     * @param statements The top-level statements defined in the program.
     * @param span The source code span covering the entire program.
     * @param table The global symbol table for this program.
     */
    Program(
        std::vector<std::unique_ptr<Node>>&& statements,
        pretty_diagnostics::Span span,
        std::shared_ptr<sem::SymbolTable> table
    ) : _statements(std::move(statements)), _table(std::move(table)), _span(std::move(span)) { }

    /**
     * @brief Accepts a visitor to process this `Program` node.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the program.
     *
     * @return The `pretty_diagnostics::Span` of the program.
     */
    [[nodiscard]] pretty_diagnostics::Span span() const override { return _span; }

    /**
     * @brief Returns the statements contained within the program.
     *
     * @return A constant reference to the vector of statement nodes.
     */
    [[nodiscard]] auto& statements() const { return _statements; }

    /**
     * @brief Returns the global symbol table associated with the program.
     *
     * @return A constant reference to the `sem::SymbolTable`.
     */
    [[nodiscard]] auto& table() const { return _table; }

private:
    std::vector<std::unique_ptr<Node>> _statements;
    std::shared_ptr<sem::SymbolTable> _table;
    pretty_diagnostics::Span _span;
};

/**
 * @brief Represents a block of statements enclosed in braces.
 *
 * Blocks introduce a new lexical scope and contain their own symbol table.
 */
class Block final : public Node {
public:
    /**
     * @brief Constructs a `Block` node.
     *
     * @param statements The sequence of statements within the block.
     * @param span The source code span from the opening brace to the closing brace.
     * @param table The symbol table for the scope introduced by this block.
     */
    Block(
        std::vector<std::unique_ptr<Node>>&& statements,
        pretty_diagnostics::Span span,
        std::shared_ptr<sem::SymbolTable> table
    ) : _statements(std::move(statements)), _table(std::move(table)), _span(std::move(span)) { }

    /**
     * @brief Accepts a visitor to process this `Block` node.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the block.
     *
     * @return The `pretty_diagnostics::Span` of the block.
     */
    [[nodiscard]] pretty_diagnostics::Span span() const override { return _span; }

    /**
     * @brief Returns the statements contained within the block.
     *
     * @return A constant reference to the vector of statement nodes.
     */
    [[nodiscard]] auto& statements() const { return _statements; }

    /**
     * @brief Returns the symbol table associated with this block's scope.
     *
     * @return A constant reference to the `sem::SymbolTable`.
     */
    [[nodiscard]] auto& table() const { return _table; }

private:
    std::vector<std::unique_ptr<Node>> _statements;
    std::shared_ptr<sem::SymbolTable> _table;
    pretty_diagnostics::Span _span;
};

/**
 * @brief Represents an identifier (e.g., variable or function name).
 *
 * An `Identifier` node captures the name of a symbol and, after semantic analysis,
 * holds a reference to the resolved `Symbol`.
 */
class Identifier final : public Node {
public:
#define IDENTIFIER_KIND_TYPES \
    X(Function)               \
    X(Variable)

    /**
     * @brief Specifies the kind of entity the identifier refers to.
     */
    enum class Kind {
#define X(element) element,
        IDENTIFIER_KIND_TYPES
#undef X
    };

public:
    /**
     * @brief Constructs an `Identifier` node.
     *
     * @param value The token containing the identifier's name.
     * @param kind The kind of identifier (variable or function).
     * @param span The source code span of the identifier.
     */
    Identifier(front::Token value, const Kind kind, pretty_diagnostics::Span span) :
        _span(std::move(span)), _value(std::move(value)), _kind(kind) { }

    /**
     * @brief Accepts a visitor to process this `Identifier` node.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the identifier.
     *
     * @return The `pretty_diagnostics::Span` of the identifier.
     */
    [[nodiscard]] pretty_diagnostics::Span span() const override { return _span; }

    /**
     * @brief Returns the resolved symbol for this identifier.
     *
     * This is populated during the semantic analysis phase.
     *
     * @return A shared pointer to the `Symbol`.
     * @throws std::bad_optional_access if the symbol has not been resolved.
     */
    [[nodiscard]] auto& symbol() const { return _symbol.value(); }

    /**
     * @brief Sets the resolved symbol for this identifier.
     *
     * @param symbol The `Symbol` to associate with this identifier.
     */
    void set_symbol(std::shared_ptr<Symbol> symbol) { _symbol = std::move(symbol); }

    /**
     * @brief Returns the token representing the identifier's name.
     *
     * @return A constant reference to the `front::Token`.
     */
    [[nodiscard]] auto& value() const { return _value; }

    /**
     * @brief Returns the kind of the identifier.
     *
     * @return The `Kind` of the identifier.
     */
    [[nodiscard]] auto& kind() const { return _kind; }

    /**
     * @brief Converts an identifier kind to its string representation.
     *
     * @param kind The kind to convert.
     * @return A string literal representing the kind.
     */
    static const char* to_string(const Kind kind) {
        switch (kind) {
#define X(element) case Kind::element: return #element;
            IDENTIFIER_KIND_TYPES
#undef X
        }
        return "<unknown>";
    }

private:
    std::optional<std::shared_ptr<Symbol>> _symbol{ };
    pretty_diagnostics::Span _span;
    front::Token _value;
    Kind _kind;
};

/**
 * @brief Represents a parameter in a function definition.
 *
 * A `Parameter` consists of an identifier (the parameter name) and a type.
 */
class Parameter final : public Node {
public:
    /**
     * @brief Constructs a `Parameter` node.
     *
     * @param name The identifier of the parameter.
     * @param type The semantic type of the parameter.
     * @param span The source code span of the parameter declaration.
     */
    Parameter(Identifier name, sem::Type type, pretty_diagnostics::Span span) :
        _span(std::move(span)), _name(std::move(name)), _type(std::move(type)) { }

    /**
     * @brief Accepts a visitor to process this `Parameter` node.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the parameter.
     *
     * @return The `pretty_diagnostics::Span` of the parameter.
     */
    [[nodiscard]] pretty_diagnostics::Span span() const override { return _span; }

    /**
     * @brief Returns the semantic type of the parameter.
     *
     * @return A constant reference to the `sem::Type`.
     */
    [[nodiscard]] auto& type() const { return _type; }

    /**
     * @brief Returns the identifier of the parameter.
     *
     * @return A reference to the `Identifier`.
     */
    [[nodiscard]] auto& name() { return _name; }

private:
    pretty_diagnostics::Span _span;
    Identifier _name;
    sem::Type _type;
};

/**
 * @brief Represents a function definition in the AST.
 *
 * A `Function` node includes its name, parameters, return type, and the body `Block`.
 * It also holds the symbol table for the function's scope.
 */
class Function final : public Node {
public:
    /**
     * @brief Constructs a `Function` node.
     *
     * @param name The identifier of the function.
     * @param parameters The list of parameters for the function.
     * @param type The return type of the function.
     * @param block The body block containing the function's statements.
     * @param span The source code span of the entire function definition.
     * @param table The symbol table for the function's scope.
     */
    Function(
        Identifier name,
        std::vector<Parameter>&& parameters,
        sem::Type type,
        std::unique_ptr<Block>&& block,
        pretty_diagnostics::Span span,
        std::shared_ptr<sem::SymbolTable> table
    ) : _table(std::move(table)), _parameters(std::move(parameters)), _span(std::move(span)),
        _block(std::move(block)), _name(std::move(name)), _type(std::move(type)) { }

    /**
     * @brief Accepts a visitor to process this `Function` node.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the function.
     *
     * @return The `pretty_diagnostics::Span` of the function.
     */
    [[nodiscard]] pretty_diagnostics::Span span() const override { return _span; }

    /**
     * @brief Returns the parameters of the function.
     *
     * @return A reference to the vector of `Parameter` nodes.
     */
    [[nodiscard]] auto& parameters() { return _parameters; }

    /**
     * @brief Returns the symbol table associated with the function's scope.
     *
     * @return A constant reference to the `sem::SymbolTable`.
     */
    [[nodiscard]] auto& table() const { return _table; }

    /**
     * @brief Returns the return type of the function.
     *
     * @return A constant reference to the `sem::Type`.
     */
    [[nodiscard]] auto& type() const { return _type; }

    /**
     * @brief Returns the body block of the function.
     *
     * @return A reference to the unique pointer of the `Block`.
     */
    [[nodiscard]] auto& block() { return _block; }

    /**
     * @brief Returns the name identifier of the function.
     *
     * @return A reference to the `Identifier`.
     */
    [[nodiscard]] auto& name() { return _name; }

private:
    std::shared_ptr<sem::SymbolTable> _table;
    std::vector<Parameter> _parameters;
    pretty_diagnostics::Span _span;
    std::unique_ptr<Block> _block;
    Identifier _name;
    sem::Type _type;
};

/**
 * @brief Represents a return statement.
 *
 * A `Return` node contains an optional expression to be returned from a function.
 */
class Return final : public Node {
public:
    /**
     * @brief Constructs a `Return` node.
     *
     * @param expression The expression to return.
     * @param span The source code span of the return statement.
     */
    Return(std::unique_ptr<Node>&& expression, pretty_diagnostics::Span span) :
        _expression(std::move(expression)), _span(std::move(span)) { }

    /**
     * @brief Accepts a visitor to process this `Return` node.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the return statement.
     *
     * @return The `pretty_diagnostics::Span` of the return statement.
     */
    [[nodiscard]] pretty_diagnostics::Span span() const override { return _span; }

    /**
     * @brief Returns the type of the expression being returned.
     *
     * This is populated during semantic analysis.
     *
     * @return A constant reference to the `sem::Type`.
     * @throws std::bad_optional_access if the type has not been set.
     */
    [[nodiscard]] auto& type() const { return _type.value(); }

    /**
     * @brief Sets the type of the expression being returned.
     *
     * @param type The semantic type to set.
     */
    void set_type(sem::Type type) { _type = std::move(type); }

    /**
     * @brief Returns the expression being returned.
     *
     * @return A reference to the unique pointer of the expression `Node`.
     */
    [[nodiscard]] auto& expression() { return _expression; }

    /**
     * @brief Sets the expression to be returned.
     *
     * @param node The expression `Node` to set.
     */
    void set_expression(std::unique_ptr<Node>&& node) { _expression = std::move(node); }

private:
    std::optional<sem::Type> _type{ };
    std::unique_ptr<Node> _expression;
    pretty_diagnostics::Span _span;
};

/**
 * @brief Represents an if-else conditional statement.
 *
 * An `If` node contains a condition, a mandatory `then` branch, and an optional
 * `else` branch (represented by `next`).
 */
class If final : public Node {
public:
    /**
     * @brief Constructs an `If` node.
     *
     * @param condition The condition expression.
     * @param branch The statement or block to execute if the condition is true.
     * @param next The optional statement or block to execute if the condition is false.
     * @param span The source code span of the entire if-else statement.
     */
    If(
        std::unique_ptr<Node>&& condition,
        std::unique_ptr<Node>&& branch,
        std::unique_ptr<Node>&& next,
        pretty_diagnostics::Span span
    ) : _next(std::move(next)), _branch(std::move(branch)), _condition(std::move(condition)),
        _span(std::move(span)) { }

    /**
     * @brief Accepts a visitor to process this `If` node.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the if-else statement.
     *
     * @return The `pretty_diagnostics::Span` of the if-else statement.
     */
    [[nodiscard]] pretty_diagnostics::Span span() const override { return _span; }

    /**
     * @brief Returns the `then` branch of the if statement.
     *
     * @return A constant reference to the unique pointer of the branch `Node`.
     */
    [[nodiscard]] auto& branch() const { return _branch; }

    /**
     * @brief Returns the `else` branch of the if statement.
     *
     * @return A constant reference to the unique pointer of the next `Node`.
     */
    [[nodiscard]] auto& next() const { return _next; }

    /**
     * @brief Returns the condition expression.
     *
     * @return A reference to the unique pointer of the condition `Node`.
     */
    [[nodiscard]] auto& condition() { return _condition; }

    /**
     * @brief Sets the condition expression.
     *
     * @param condition The condition `Node` to set.
     */
    void set_condition(std::unique_ptr<Node>&& condition) { _condition = std::move(condition); }

private:
    std::unique_ptr<Node> _next, _branch;
    std::unique_ptr<Node> _condition;
    pretty_diagnostics::Span _span;
};


/**
 * @brief Represents a while statement.
 */
class While final : public Node {
public:
    /**
     * @brief Constructs an `While` node.
     *
     * @param condition The condition expression.
     * @param then The node that gets executed if the condition is true.
     * @param span The source code span of the entire while statement.
     */
    While(
        std::unique_ptr<Node>&& condition,
        std::unique_ptr<Node>&& then,
        pretty_diagnostics::Span span
    ) : _condition(std::move(condition)), _span(std::move(span)),
        _then(std::move(then)) { }

    /**
     * @brief Accepts a visitor to process this `If` node.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the while statement.
     *
     * @return The `pretty_diagnostics::Span` of the while statement.
     */
    [[nodiscard]] pretty_diagnostics::Span span() const override { return _span; }

    /**
     * @brief Returns the node that is getting executed if the condition is true.
     *
     * @return A constant reference to the unique pointer of the `Node`.
     */
    [[nodiscard]] auto& then() const { return _then; }

    /**
     * @brief Returns the condition expression.
     *
     * @return A reference to the unique pointer of the condition `Node`.
     */
    [[nodiscard]] auto& condition() { return _condition; }

    /**
     * @brief Sets the condition expression.
     *
     * @param condition The condition `Node` to set.
     */
    void set_condition(std::unique_ptr<Node>&& condition) { _condition = std::move(condition); }

private:
    std::unique_ptr<Node> _condition;
    pretty_diagnostics::Span _span;
    std::unique_ptr<Node> _then;
};

/**
 * @brief Represents an assignment statement.
 *
 * An `Assign` node represents the assignment of an expression to a variable identified by name.
 */
class Assign final : public Node {
public:
    /**
     * @brief Constructs an `Assign` node.
     *
     * @param name The identifier of the target variable.
     * @param expression The expression whose value is being assigned.
     * @param span The source code span of the assignment statement.
     */
    Assign(Identifier name, std::unique_ptr<Node>&& expression, pretty_diagnostics::Span span) :
        _expression(std::move(expression)), _span(std::move(span)), _name(std::move(name)) { }

    /**
     * @brief Accepts a visitor to process this `Assign` node.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the assignment.
     *
     * @return The `pretty_diagnostics::Span` of the assignment.
     */
    [[nodiscard]] pretty_diagnostics::Span span() const override { return _span; }

    /**
     * @brief Returns the expression being assigned.
     *
     * @return A reference to the unique pointer of the expression `Node`.
     */
    [[nodiscard]] auto& expression() { return _expression; }

    /**
     * @brief Sets the expression being assigned.
     *
     * @param node The expression `Node` to set.
     */
    void set_expression(std::unique_ptr<Node>&& node) { _expression = std::move(node); }

    /**
     * @brief Returns the identifier of the target variable.
     *
     * @return A reference to the target `Identifier`.
     */
    [[nodiscard]] auto& name() { return _name; }

private:
    std::unique_ptr<Node> _expression;
    pretty_diagnostics::Span _span;
    Identifier _name;
};

/**
 * @brief Represents a variable declaration statement.
 *
 * A `Variable` node includes its name, explicit type, and an optional initial expression.
 */
class Variable final : public Node {
public:
    /**
     * @brief Constructs a `Variable` declaration node.
     *
     * @param name The identifier of the declared variable.
     * @param type The declared semantic type.
     * @param expression The optional initial value expression.
     * @param span The source code span of the variable declaration.
     */
    Variable(
        Identifier name,
        sem::Type type,
        std::unique_ptr<Node>&& expression,
        pretty_diagnostics::Span span
    ) : _expression(std::move(expression)), _span(std::move(span)), _name(std::move(name)),
        _type(std::move(type)) { }

    /**
     * @brief Accepts a visitor to process this `Variable` node.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the variable declaration.
     *
     * @return The `pretty_diagnostics::Span` of the variable declaration.
     */
    [[nodiscard]] pretty_diagnostics::Span span() const override { return _span; }

    /**
     * @brief Returns the initial value expression.
     *
     * @return A reference to the unique pointer of the expression `Node`.
     */
    [[nodiscard]] auto& expression() { return _expression; }

    /**
     * @brief Sets the initial value expression.
     *
     * @param node The expression `Node` to set.
     */
    void set_expression(std::unique_ptr<Node>&& node) { _expression = std::move(node); }

    /**
     * @brief Returns the declared semantic type of the variable.
     *
     * @return A constant reference to the `sem::Type`.
     */
    [[nodiscard]] auto& type() const { return _type; }

    /**
     * @brief Returns the identifier of the declared variable.
     *
     * @return A reference to the `Identifier`.
     */
    [[nodiscard]] auto& name() { return _name; }

private:
    std::unique_ptr<Node> _expression;
    pretty_diagnostics::Span _span;
    Identifier _name;
    sem::Type _type;
};

/**
 * @brief Represents a function call expression.
 *
 * A `Call` node consists of the function's name and a list of argument expressions.
 */
class Call final : public Node {
public:
    /**
     * @brief Constructs a `Call` node.
     *
     * @param name The identifier of the function being called.
     * @param arguments The list of expressions passed as arguments.
     * @param span The source code span of the function call.
     */
    Call(Identifier name, std::vector<std::unique_ptr<Node>>&& arguments, pretty_diagnostics::Span span) :
        _arguments(std::move(arguments)), _span(std::move(span)), _name(std::move(name)) { }

    /**
     * @brief Accepts a visitor to process this `Call` node.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the call.
     *
     * @return The `pretty_diagnostics::Span` of the call.
     */
    [[nodiscard]] pretty_diagnostics::Span span() const override { return _span; }

    /**
     * @brief Returns the arguments passed to the function.
     *
     * @return A reference to the vector of argument `Node` pointers.
     */
    [[nodiscard]] auto& arguments() { return _arguments; }

    /**
     * @brief Returns the name of the function being called.
     *
     * @return A reference to the function `Identifier`.
     */
    [[nodiscard]] auto& name() { return _name; }

private:
    std::vector<std::unique_ptr<Node>> _arguments;
    pretty_diagnostics::Span _span;
    Identifier _name;
};

/**
 * @brief Represents a literal (immediate) value in the source code.
 *
 * Immediates can be integers, floating-point numbers, or booleans.
 */
class Immediate final : public Node {
public:
#define IMMEDIATE_KIND_TYPES \
    X(Integer)     \
    X(Floating)    \
    X(Boolean)

    /**
     * @brief Specifies the kind of literal value.
     */
    enum class Kind {
#define X(element) element,
        IMMEDIATE_KIND_TYPES
#undef X
    };

public:
    /**
     * @brief Constructs an `Immediate` node.
     *
     * @param value The token containing the literal value.
     * @param kind The kind of the literal value.
     * @param span The source code span of the literal.
     */
    Immediate(front::Token value, const Kind kind, pretty_diagnostics::Span span) :
        _span(std::move(span)), _value(std::move(value)), _kind(kind) { }

    /**
     * @brief Accepts a visitor to process this `Immediate` node.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the literal.
     *
     * @return The `pretty_diagnostics::Span` of the literal.
     */
    [[nodiscard]] pretty_diagnostics::Span span() const override { return _span; }

    /**
     * @brief Returns the token representing the literal value.
     *
     * @return A constant reference to the `front::Token`.
     */
    [[nodiscard]] auto& value() const { return _value; }

    /**
     * @brief Returns the kind of the literal value.
     *
     * @return The `Kind` of the immediate.
     */
    [[nodiscard]] auto& kind() const { return _kind; }

    /**
     * @brief Returns the semantic type of the literal value.
     *
     * This is populated during semantic analysis.
     *
     * @return A constant reference to the `sem::Type`.
     */
    [[nodiscard]] auto& type() const { return *_type; }

    /**
     * @brief Sets the semantic type of the literal value.
     *
     * @param type The semantic type to set.
     */
    void set_type(sem::Type type) { _type = std::move(type); }

    /**
     * @brief Converts an immediate kind to its string representation.
     *
     * @param kind The kind to convert.
     * @return A string literal representing the kind.
     */
    static const char* to_string(const Kind kind) {
        switch (kind) {
#define X(element) case Kind::element: return #element;
            IMMEDIATE_KIND_TYPES
#undef X
        }
        return "<unknown>";
    }

private:
    std::optional<sem::Type> _type;
    pretty_diagnostics::Span _span;
    front::Token _value;
    Kind _kind;
};

/**
 * @brief Represents a binary operation expression.
 *
 * A `Binary` node consists of a left operand, an operator, and a right operand.
 */
class Binary final : public Node {
public:
#define BINARY_OPERATOR_TYPES \
    X(Add)             \
    X(Sub)             \
    X(Mul)             \
    X(Div)             \
    X(GreaterThan)     \
    X(LessThan)

    /**
     * @brief Specifies the binary operator.
     */
    enum class Operator {
#define X(element) element,
        BINARY_OPERATOR_TYPES
#undef X
    };

public:
    /**
     * @brief Constructs a `Binary` operation node.
     *
     * @param left The left operand expression.
     * @param op The binary operator to apply.
     * @param right The right operand expression.
     * @param span The source code span of the entire binary expression.
     */
    Binary(
        std::unique_ptr<Node>&& left,
        const Operator op,
        std::unique_ptr<Node>&& right,
        pretty_diagnostics::Span span
    ) : _left(std::move(left)), _right(std::move(right)), _span(std::move(span)), _op(op) { }

    /**
     * @brief Accepts a visitor to process this `Binary` node.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the binary expression.
     *
     * @return The `pretty_diagnostics::Span` of the binary expression.
     */
    [[nodiscard]] pretty_diagnostics::Span span() const override { return _span; }

    /**
     * @brief Returns the binary operator.
     *
     * @return The `Operator` of the expression.
     */
    [[nodiscard]] auto& op() const { return _op; }

    /**
     * @brief Returns the type expected for the operands.
     *
     * This is populated during semantic analysis.
     *
     * @return A constant reference to the `sem::Type`.
     * @throws std::bad_optional_access if the type has not been set.
     */
    [[nodiscard]] auto& op_type() const { return _op_type.value(); }

    /**
     * @brief Sets the expected type for the operands.
     *
     * @param type The semantic type to set.
     */
    void set_op_type(sem::Type type) { _op_type = std::move(type); }

    /**
     * @brief Returns the right operand.
     *
     * @return A reference to the unique pointer of the right `Node`.
     */
    [[nodiscard]] auto& right() { return _right; }

    /**
     * @brief Sets the right operand.
     *
     * @param node The `Node` to set as the right operand.
     */
    void set_right(std::unique_ptr<Node>&& node) { _right = std::move(node); }

    /**
     * @brief Returns the left operand.
     *
     * @return A reference to the unique pointer of the left `Node`.
     */
    [[nodiscard]] auto& left() { return _left; }

    /**
     * @brief Sets the left operand.
     *
     * @param node The `Node` to set as the left operand.
     */
    void set_left(std::unique_ptr<Node>&& node) { _left = std::move(node); }

    /**
     * @brief Returns the result type of the binary operation.
     *
     * This is populated during semantic analysis.
     *
     * @return A constant reference to the `sem::Type`.
     * @throws std::bad_optional_access if the type has not been set.
     */
    [[nodiscard]] auto& result_type() const { return _result_type.value(); }

    /**
     * @brief Sets the result type of the binary operation.
     *
     * @param type The semantic type to set.
     */
    void set_result_type(sem::Type type) { _result_type = std::move(type); }

    /**
     * @brief Converts a binary operator to its string representation.
     *
     * @param op The operator to convert.
     * @return A string literal representing the operator.
     */
    static const char* to_string(const Operator op) {
        switch (op) {
#define X(element) case Operator::element: return #element;
            BINARY_OPERATOR_TYPES
#undef X
        }
        return "<unknown>";
    }

private:
    std::optional<sem::Type> _result_type{ }, _op_type{ };
    std::unique_ptr<Node> _left, _right;
    pretty_diagnostics::Span _span;
    Operator _op;
};

/**
 * @brief Represents an explicit or implicit type cast.
 *
 * A `Cast` node converts an expression from one semantic type to another.
 */
class Cast final : public Node {
public:
    /**
     * @brief Constructs a `Cast` node with both source and target types.
     *
     * @param expression The expression being cast.
     * @param from The source semantic type.
     * @param to The target semantic type.
     * @param span The source code span of the entire cast.
     */
    Cast(std::unique_ptr<Node>&& expression, sem::Type from, sem::Type to, pretty_diagnostics::Span span) :
        _from(from), _expression(std::move(expression)), _span(std::move(span)), _to(std::move(to)) { }

    /**
     * @brief Constructs a `Cast` node with only the target type.
     *
     * The source type is expected to be populated later during analysis.
     *
     * @param expression The expression being cast.
     * @param to The target semantic type.
     * @param span The source code span of the entire cast.
     */
    Cast(std::unique_ptr<Node>&& expression, sem::Type to, pretty_diagnostics::Span span) :
        _expression(std::move(expression)), _span(std::move(span)), _to(std::move(to)) { }

    /**
     * @brief Accepts a visitor to process this `Cast` node.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the cast.
     *
     * @return The `pretty_diagnostics::Span` of the cast.
     */
    [[nodiscard]] pretty_diagnostics::Span span() const override { return _span; }

    /**
     * @brief Returns the expression being cast.
     *
     * @return A constant reference to the unique pointer of the expression `Node`.
     */
    [[nodiscard]] auto& expression() const { return _expression; }

    /**
     * @brief Returns the source type of the cast.
     *
     * @return A constant reference to the source `sem::Type`.
     * @throws std::bad_optional_access if the source type has not been set.
     */
    [[nodiscard]] auto& from() const { return _from.value(); }

    /**
     * @brief Sets the source type of the cast.
     *
     * @param type The source semantic type to set.
     */
    void set_from(sem::Type type) { _from = std::move(type); }

    /**
     * @brief Returns the target type of the cast.
     *
     * @return A constant reference to the target `sem::Type`.
     */
    [[nodiscard]] auto& to() const { return _to; }

private:
    std::optional<sem::Type> _from{ };
    std::unique_ptr<Node> _expression;
    pretty_diagnostics::Span _span;
    sem::Type _to;
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
