#pragma once

#include <utility>

#include "arkoi_language/ast/visitor.hpp"
#include "arkoi_language/front/token.hpp"
#include "arkoi_language/sem/symbol_table.hpp"
#include "arkoi_language/sem/type.hpp"

#include "pretty_diagnostics/span.hpp"

namespace arkoi::ast {
/**
 * @brief Base class for all AST nodes
 */
class Node {
public:
    virtual ~Node() = default;

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    virtual void accept(Visitor& visitor) = 0;

    /**
     * @brief Returns the source code span of the entire node
     *
     * @return The span of the node
     */
    [[nodiscard]] virtual pretty_diagnostics::Span span() = 0;
};

/**
 * @brief Represents a program in the AST.
 */
class Program final : public Node {
public:
    /**
     * @brief Constructs a Program node with the given parameters
     *
     * @param statements The statements defined in the program
     * @param span The source code span of the program
     * @param table The symbol table used for the program
     */
    Program(
        std::vector<std::unique_ptr<Node>>&& statements,
        pretty_diagnostics::Span span,
        std::shared_ptr<sem::SymbolTable> table
    ) : _statements(std::move(statements)), _table(std::move(table)), _span(std::move(span)) { }

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the entire node
     *
     * @return The span of the node
     */
    [[nodiscard]] pretty_diagnostics::Span span() override { return _span; }

    /**
     * @brief Returns the statements defined in the program
     *
     * @return A reference to the vector of statements
     */
    [[nodiscard]] auto& statements() const { return _statements; }

    /**
     * @brief Returns the symbol table used for the program
     *
     * @return A reference to the symbol table
     */
    [[nodiscard]] auto& table() const { return _table; }

private:
    std::vector<std::unique_ptr<Node>> _statements;
    std::shared_ptr<sem::SymbolTable> _table;
    pretty_diagnostics::Span _span;
};

/**
 * @brief Represents a block of statements in the AST
 */
class Block final : public Node {
public:
    /**
     * @brief Constructs a Block node with the given parameters
     *
     * @param statements The statements defined in the block
     * @param span The source code span of the block
     * @param table The symbol table used for the block
     */
    Block(
        std::vector<std::unique_ptr<Node>>&& statements,
        pretty_diagnostics::Span span,
        std::shared_ptr<sem::SymbolTable> table
    ) : _statements(std::move(statements)), _table(std::move(table)), _span(std::move(span)) { }

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the entire node
     *
     * @return The span of the node
     */
    [[nodiscard]] pretty_diagnostics::Span span() override { return _span; }

    /**
     * @brief Returns the statements defined in the block
     *
     * @return A reference to the vector of statements
     */
    [[nodiscard]] auto& statements() const { return _statements; }

    /**
     * @brief Returns the symbol table used for the block
     *
     * @return A reference to the symbol table
     */
    [[nodiscard]] auto& table() const { return _table; }

private:
    std::vector<std::unique_ptr<Node>> _statements;
    std::shared_ptr<sem::SymbolTable> _table;
    pretty_diagnostics::Span _span;
};

/**
 * @brief Represents an identifier in the AST
 */
class Identifier final : public Node {
public:
#define IDENTIFIER_KIND_TYPES \
    X(Function)               \
    X(Variable)

    /**
     * @brief Enumeration of different identifier kinds
     */
    enum class Kind {
#define X(element) element,
        IDENTIFIER_KIND_TYPES
#undef X
    };

public:
    /**
     * @brief Constructs an Identifier node with the given parameters
     *
     * @param value The token representing the identifier
     * @param kind The kind of the identifier
     * @param span The source code span of the entire identifier
     */
    Identifier(front::Token value, const Kind kind, pretty_diagnostics::Span span) :
        _span(std::move(span)), _value(std::move(value)), _kind(kind) { }

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the entire node
     *
     * @return The span of the node
     */
    [[nodiscard]] pretty_diagnostics::Span span() override { return _span; }

    /**
     * @brief Returns the symbol associated with the identifier
     *
     * @return A reference to the symbol
     */
    [[nodiscard]] auto& symbol() const { return _symbol.value(); }

    /**
     * @brief Sets the symbol associated with the identifier
     *
     * @param symbol The symbol to set
     */
    void set_symbol(std::shared_ptr<Symbol> symbol) { _symbol = std::move(symbol); }

    /**
     * @brief Returns the token representing the identifier
     *
     * @return A reference to the token
     */
    [[nodiscard]] auto& value() const { return _value; }

    /**
     * @brief Returns the kind of the identifier
     *
     * @return The kind of the identifier
     */
    [[nodiscard]] auto& kind() const { return _kind; }

    /**
     * @brief Converts an identifier kind to a string
     *
     * @param kind The kind to convert
     *
     * @return A string representation of the kind
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
 * @brief Represents a function parameter in the AST
 */
class Parameter final : public Node {
public:
    /**
     * @brief Constructs a Parameter node with the given parameters
     *
     * @param name The name of the parameter
     * @param type The type of the parameter
     * @param span The source code span of the entire parameter
     */
    Parameter(Identifier name, sem::Type type, pretty_diagnostics::Span span) :
        _span(std::move(span)), _name(std::move(name)), _type(std::move(type)) { }

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the entire node
     *
     * @return The span of the node
     */
    [[nodiscard]] pretty_diagnostics::Span span() override { return _span; }

    /**
     * @brief Returns the type of the parameter
     *
     * @return A reference to the type
     */
    [[nodiscard]] auto& type() const { return _type; }

    /**
     * @brief Returns the name of the parameter
     *
     * @return A reference to the identifier representing the name
     */
    [[nodiscard]] auto& name() { return _name; }

private:
    pretty_diagnostics::Span _span;
    Identifier _name;
    sem::Type _type;
};

/**
 * @brief Represents a function definition in the AST
 */
class Function final : public Node {
public:
    /**
     * @brief Constructs a Function node with the given parameters
     *
     * @param name The name of the function
     * @param parameters The parameters of the function
     * @param type The return type of the function
     * @param block The body of the function
     * @param span The source code span of the entire function
     * @param table The symbol table used for the function
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
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the entire node
     *
     * @return The span of the node
     */
    [[nodiscard]] pretty_diagnostics::Span span() override { return _span; }

    /**
     * @brief Returns the parameters of the function
     *
     * @return A reference to the vector of parameters
     */
    [[nodiscard]] auto& parameters() { return _parameters; }

    /**
     * @brief Returns the symbol table for the function
     *
     * @return A reference to the symbol table
     */
    [[nodiscard]] auto& table() const { return _table; }

    /**
     * @brief Returns the return type of the function
     *
     * @return A reference to the type
     */
    [[nodiscard]] auto& type() const { return _type; }

    /**
     * @brief Returns the body of the function
     *
     * @return A reference to the unique_ptr of the block
     */
    [[nodiscard]] auto& block() { return _block; }

    /**
     * @brief Returns the name of the function
     *
     * @return A reference to the identifier representing the name
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
 * @brief Represents a return statement in the AST
 */
class Return final : public Node {
public:
    /**
     * @brief Constructs a Return node with the given parameters
     *
     * @param expression The expression to return
     * @param span The source code span of the entire return statement
     */
    Return(std::unique_ptr<Node>&& expression, pretty_diagnostics::Span span) :
        _expression(std::move(expression)), _span(std::move(span)) { }

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the entire node
     *
     * @return The span of the node
     */
    [[nodiscard]] pretty_diagnostics::Span span() override { return _span; }

    /**
     * @brief Returns the type of the returned expression
     *
     * @return A reference to the type
     */
    [[nodiscard]] auto& type() const { return _type.value(); }

    /**
     * @brief Sets the type of the returned expression
     *
     * @param type The type to set
     */
    void set_type(sem::Type type) { _type = std::move(type); }

    /**
     * @brief Returns the returned expression
     *
     * @return A reference to the unique_ptr of the expression
     */
    [[nodiscard]] auto& expression() { return _expression; }

    /**
     * @brief Sets the returned expression
     *
     * @param node The expression node to set
     */
    void set_expression(std::unique_ptr<Node>&& node) { _expression = std::move(node); }

private:
    std::optional<sem::Type> _type{ };
    std::unique_ptr<Node> _expression;
    pretty_diagnostics::Span _span;
};

/**
 * @brief Represents an if statement in the AST
 */
class If final : public Node {
public:
    /**
     * @brief Constructs an If node with the given parameters
     *
     * @param condition The condition of the if statement
     * @param branch The then branch of the if statement
     * @param next The else branch (if any) of the if statement
     * @param span The source code span of the entire if statement
     */
    If(
        std::unique_ptr<Node>&& condition,
        std::unique_ptr<Node>&& branch,
        std::unique_ptr<Node>&& next,
        pretty_diagnostics::Span span
    ) : _next(std::move(next)), _branch(std::move(branch)), _condition(std::move(condition)),
        _span(std::move(span)) { }

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the entire node
     *
     * @return The span of the node
     */
    [[nodiscard]] pretty_diagnostics::Span span() override { return _span; }

    /**
     * @brief Returns the then branch of the if statement
     *
     * @return A reference to the unique_ptr of the branch
     */
    [[nodiscard]] auto& branch() const { return _branch; }

    /**
     * @brief Returns the else branch of the if statement
     *
     * @return A reference to the unique_ptr of the next node
     */
    [[nodiscard]] auto& next() const { return _next; }

    /**
     * @brief Returns the condition of the if statement
     *
     * @return A reference to the unique_ptr of the condition
     */
    [[nodiscard]] auto& condition() { return _condition; }

    /**
     * @brief Sets the condition of the if statement
     *
     * @param condition The condition node to set
     */
    void set_condition(std::unique_ptr<Node>&& condition) { _condition = std::move(condition); }

private:
    std::unique_ptr<Node> _next, _branch;
    std::unique_ptr<Node> _condition;
    pretty_diagnostics::Span _span;
};

/**
 * @brief Represents an assignment statement in the AST
 */
class Assign final : public Node {
public:
    /**
     * @brief Constructs an Assign node with the given parameters
     *
     * @param name The name of the variable being assigned to
     * @param expression The expression being assigned
     * @param span The source code span of the entire assignment statement
     */
    Assign(Identifier name, std::unique_ptr<Node>&& expression, pretty_diagnostics::Span span) :
        _expression(std::move(expression)), _span(std::move(span)), _name(std::move(name)) { }

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the entire node
     *
     * @return The span of the node
     */
    [[nodiscard]] pretty_diagnostics::Span span() override { return _span; }

    /**
     * @brief Returns the expression being assigned
     *
     * @return A reference to the unique_ptr of the expression
     */
    [[nodiscard]] auto& expression() { return _expression; }

    /**
     * @brief Sets the expression being assigned
     *
     * @param node The expression node to set
     */
    void set_expression(std::unique_ptr<Node>&& node) { _expression = std::move(node); }

    /**
     * @brief Returns the name of the variable being assigned to
     *
     * @return A reference to the identifier representing the name
     */
    [[nodiscard]] auto& name() { return _name; }

private:
    std::unique_ptr<Node> _expression;
    pretty_diagnostics::Span _span;
    Identifier _name;
};

/**
 * @brief Represents a variable declaration in the AST
 */
class Variable final : public Node {
public:
    /**
     * @brief Constructs a Variable node with the given parameters
     *
     * @param name The name of the variable
     * @param type The type of the variable
     * @param expression The initial expression for the variable (optional)
     * @param span The source code span of the entire variable declaration
     */
    Variable(
        Identifier name,
        sem::Type type,
        std::unique_ptr<Node>&& expression,
        pretty_diagnostics::Span span
    ) : _expression(std::move(expression)), _span(std::move(span)), _name(std::move(name)),
        _type(std::move(type)) { }

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the entire node
     *
     * @return The span of the node
     */
    [[nodiscard]] pretty_diagnostics::Span span() override { return _span; }

    /**
     * @brief Returns the initial expression of the variable
     *
     * @return A reference to the unique_ptr of the expression
     */
    [[nodiscard]] auto& expression() { return _expression; }

    /**
     * @brief Sets the initial expression of the variable
     *
     * @param node The expression node to set
     */
    void set_expression(std::unique_ptr<Node>&& node) { _expression = std::move(node); }

    /**
     * @brief Returns the type of the variable
     *
     * @return A reference to the type
     */
    [[nodiscard]] auto& type() const { return _type; }

    /**
     * @brief Returns the name of the variable
     *
     * @return A reference to the identifier representing the name
     */
    [[nodiscard]] auto& name() { return _name; }

private:
    std::unique_ptr<Node> _expression;
    pretty_diagnostics::Span _span;
    Identifier _name;
    sem::Type _type;
};

/**
 * @brief Represents a function call in the AST
 */
class Call final : public Node {
public:
    /**
     * @brief Constructs a Call node with the given parameters
     *
     * @param name The name of the function being called
     * @param arguments The arguments to the function call
     * @param span The source code span of the entire function call
     */
    Call(Identifier name, std::vector<std::unique_ptr<Node>>&& arguments, pretty_diagnostics::Span span) :
        _arguments(std::move(arguments)), _span(std::move(span)), _name(std::move(name)) { }

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the entire node
     *
     * @return The span of the node
     */
    [[nodiscard]] pretty_diagnostics::Span span() override { return _span; }

    /**
     * @brief Returns the arguments to the function call
     *
     * @return A reference to the vector of arguments
     */
    [[nodiscard]] auto& arguments() { return _arguments; }

    /**
     * @brief Returns the name of the function being called
     *
     * @return A reference to the identifier representing the name
     */
    [[nodiscard]] auto& name() { return _name; }

private:
    std::vector<std::unique_ptr<Node>> _arguments;
    pretty_diagnostics::Span _span;
    Identifier _name;
};

/**
 * @brief Represents an immediate value (literal) in the AST
 */
class Immediate final : public Node {
public:
#define IMMEDIATE_KIND_TYPES \
    X(Integer)     \
    X(Floating)    \
    X(Boolean)

    /**
     * @brief Enumeration of different immediate kinds
     */
    enum class Kind {
#define X(element) element,
        IMMEDIATE_KIND_TYPES
#undef X
    };

public:
    /**
     * @brief Constructs an Immediate node with the given parameters
     *
     * @param value The token representing the literal value
     * @param kind The kind of the literal
     * @param span The source code span of the entire immediate node
     */
    Immediate(front::Token value, const Kind kind, pretty_diagnostics::Span span) :
        _span(std::move(span)), _value(std::move(value)), _kind(kind) { }

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the entire node
     *
     * @return The span of the node
     */
    [[nodiscard]] pretty_diagnostics::Span span() override { return _span; }

    /**
     * @brief Returns the token representing the literal value
     *
     * @return A reference to the token
     */
    [[nodiscard]] auto& value() const { return _value; }

    /**
     * @brief Returns the kind of the literal
     *
     * @return The kind of the literal
     */
    [[nodiscard]] auto& kind() const { return _kind; }

    /**
     * @brief Returns the type of the literal
     *
     * @return A reference to the type
     */
    [[nodiscard]] auto& type() const { return *_type; }

    /**
     * @brief Sets the type of the literal
     *
     * @param type The type to set
     */
    void set_type(sem::Type type) { _type = std::move(type); }

    /**
     * @brief Converts an immediate kind to a string
     *
     * @param kind The kind to convert
     *
     * @return A string representation of the kind
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
 * @brief Represents a binary operation in the AST
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
     * @brief Enumeration of different binary operators
     */
    enum class Operator {
#define X(element) element,
        BINARY_OPERATOR_TYPES
#undef X
    };

public:
    /**
     * @brief Constructs a Binary node with the given parameters
     *
     * @param left The left operand
     * @param op The operator
     * @param right The right operand
     * @param span The source code span of the entire binary operation
     */
    Binary(
        std::unique_ptr<Node>&& left,
        const Operator op,
        std::unique_ptr<Node>&& right,
        pretty_diagnostics::Span span
    ) : _left(std::move(left)), _right(std::move(right)), _span(std::move(span)), _op(op) { }

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the entire node
     *
     * @return The span of the node
     */
    [[nodiscard]] pretty_diagnostics::Span span() override { return _span; }

    /**
     * @brief Returns the operator
     *
     * @return The operator
     */
    [[nodiscard]] auto& op() const { return _op; }

    /**
     * @brief Returns the common type of the operators
     *
     * @return A reference to the type
     */
    [[nodiscard]] auto& op_type() const { return _op_type.value(); }

    /**
     * @brief Sets the common type of the operators
     *
     * @param type The type to set
     */
    void set_op_type(sem::Type type) { _op_type = std::move(type); }

    /**
     * @brief Returns the right operand
     *
     * @return A reference to the unique_ptr of the right operand
     */
    [[nodiscard]] auto& right() { return _right; }

    /**
     * @brief Sets the right operand
     *
     * @param node The node to set as the right operand
     */
    void set_right(std::unique_ptr<Node>&& node) { _right = std::move(node); }

    /**
     * @brief Returns the left operand
     *
     * @return A reference to the unique_ptr of the left operand
     */
    [[nodiscard]] auto& left() { return _left; }

    /**
     * @brief Sets the left operand
     *
     * @param node The node to set as the left operand
     */
    void set_left(std::unique_ptr<Node>&& node) { _left = std::move(node); }

    /**
     * @brief Returns the result type of the binary operation
     *
     * @return A reference to the type
     */
    [[nodiscard]] auto& result_type() const { return _result_type.value(); }

    /**
     * @brief Sets the result type of the binary operation
     *
     * @param type The type to set
     */
    void set_result_type(sem::Type type) { _result_type = std::move(type); }

    /**
     * @brief Converts a binary operator to a string
     *
     * @param op The operator to convert
     *
     * @return A string representation of the operator
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
 * @brief Represents a type cast in the AST
 */
class Cast final : public Node {
public:
    /**
     * @brief Constructs a Cast node with both 'from' and 'to' types
     *
     * @param expression The expression being cast
     * @param from The source type
     * @param to The target type
     * @param span The source code span of the entire cast
     */
    Cast(std::unique_ptr<Node>&& expression, sem::Type from, sem::Type to, pretty_diagnostics::Span span) :
        _from(from), _expression(std::move(expression)), _span(std::move(span)), _to(std::move(to)) { }

    /**
     * @brief Constructs a Cast node with only the target 'to' type
     *
     * @param expression The expression being cast
     * @param to The target type
     * @param span The source code span of the entire cast
     */
    Cast(std::unique_ptr<Node>&& expression, sem::Type to, pretty_diagnostics::Span span) :
        _expression(std::move(expression)), _span(std::move(span)), _to(std::move(to)) { }

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    /**
     * @brief Returns the source code span of the entire node
     *
     * @return The span of the node
     */
    [[nodiscard]] pretty_diagnostics::Span span() override { return _span; }

    /**
     * @brief Returns the expression being cast
     *
     * @return A reference to the unique_ptr of the expression
     */
    [[nodiscard]] auto& expression() const { return _expression; }

    /**
     * @brief Returns the source type of the cast
     *
     * @return A reference to the 'from' type
     */
    [[nodiscard]] auto& from() const { return _from.value(); }

    /**
     * @brief Sets the source type of the cast
     *
     * @param type The type to set
     */
    void set_from(sem::Type type) { _from = std::move(type); }

    /**
     * @brief Returns the target type of the cast
     *
     * @return A reference to the 'to' type
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
