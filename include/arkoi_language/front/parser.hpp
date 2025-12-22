#pragma once

#include <functional>
#include <stack>
#include <vector>

#include "arkoi_language/ast/nodes.hpp"
#include "arkoi_language/front/token.hpp"
#include "arkoi_language/utils/utils.hpp"

#include "pretty_diagnostics/source.hpp"

namespace arkoi::front {
/**
 * @brief Parser for the arkoi language
 */
class Parser {
public:
    /**
     * @brief Constructs a Parser with the given parameters
     *
     * @param source The source code being parsed
     * @param tokens The vector of tokens to parse
     */
    explicit Parser(const std::shared_ptr<pretty_diagnostics::Source>& source, std::vector<Token>&& tokens) :
        _source(source), _tokens(std::move(tokens)) { }

    /**
     * @brief Parses the entire program
     *
     * @return The AST Program node
     */
    [[nodiscard]] ast::Program parse_program();

    /**
     * @brief Checks if parsing has failed
     *
     * @return True if parsing failed, false otherwise
     */
    [[nodiscard]] auto has_failed() const { return _failed; }

private:
    /**
     * @brief Parses a top-level statement in the program
     *
     * @return A unique_ptr to the parsed node
     */
    [[nodiscard]] std::unique_ptr<ast::Node> _parse_program_statement();

    /**
     * @brief Error recovery for top-level program statements
     */
    void _recover_program();

    /**
     * @brief Parses a function definition
     *
     * @param keyword The 'fun' keyword token starting the function
     *
     * @return A unique_ptr to the parsed Function node
     */
    [[nodiscard]] std::unique_ptr<ast::Function> _parse_function(const Token& keyword);

    /**
     * @brief Parses a list of function parameters
     *
     * @return A vector of parsed Parameter nodes
     */
    [[nodiscard]] std::vector<ast::Parameter> _parse_parameters();

    /**
     * @brief Error recovery for parameter lists
     */
    void _recover_parameters();

    /**
     * @brief Parses a single function parameter
     *
     * @return The parsed Parameter node
     */
    [[nodiscard]] ast::Parameter _parse_parameter();

    /**
     * @brief Parses a type annotation
     *
     * @return The parsed semantic type
     */
    [[nodiscard]] sem::Type _parse_type();

    /**
     * @brief Parses a block of statements
     *
     * @return A unique_ptr to the parsed Block node
     */
    [[nodiscard]] std::unique_ptr<ast::Block> _parse_block();

    /**
     * @brief Parses a single statement within a block
     *
     * @return A unique_ptr to the parsed node
     */
    [[nodiscard]] std::unique_ptr<ast::Node> _parse_block_statement();

    /**
     * @brief Error recovery for block statements
     */
    void _recover_block();

    /**
     * @brief Parses a return statement
     *
     * @param keyword The 'return' keyword token
     *
     * @return A unique_ptr to the parsed Return node
     */
    [[nodiscard]] std::unique_ptr<ast::Return> _parse_return(const Token& keyword);

    /**
     * @brief Parses an if statement
     *
     * @param keyword The 'if' keyword token
     *
     * @return A unique_ptr to the parsed If node
     */
    [[nodiscard]] std::unique_ptr<ast::If> _parse_if(const Token& keyword);

    /**
     * @brief Parses an assignment statement
     *
     * @param name The identifier token being assigned to
     *
     * @return A unique_ptr to the parsed Assign node
     */
    [[nodiscard]] std::unique_ptr<ast::Assign> _parse_assign(const Token& name);

    /**
     * @brief Parses a variable declaration
     *
     * @param name The identifier token being declared
     *
     * @return A unique_ptr to the parsed Variable node
     */
    [[nodiscard]] std::unique_ptr<ast::Variable> _parse_variable(const Token& name);

    /**
     * @brief Parses a function call statement
     *
     * @param name The identifier token being called
     *
     * @return A unique_ptr to the parsed Call node
     */
    [[nodiscard]] std::unique_ptr<ast::Call> _parse_call(const Token& name);

    /**
     * @brief Parses an expression
     *
     * @return A unique_ptr to the parsed expression node
     */
    [[nodiscard]] std::unique_ptr<ast::Node> _parse_expression();

    /**
     * @brief Parses a comparison expression
     *
     * @return A unique_ptr to the parsed comparison node
     */
    [[nodiscard]] std::unique_ptr<ast::Node> _parse_comparison();

    /**
     * @brief Parses a term (addition and subtraction)
     *
     * @return A unique_ptr to the parsed term node
     */
    [[nodiscard]] std::unique_ptr<ast::Node> _parse_term();

    /**
     * @brief Parses a factor (multiplication and division)
     *
     * @return A unique_ptr to the parsed factor node
     */
    [[nodiscard]] std::unique_ptr<ast::Node> _parse_factor();

    /**
     * @brief Parses a primary expression (literals, variables, calls, parenthesized exprs)
     *
     * @return A unique_ptr to the parsed primary node
     */
    [[nodiscard]] std::unique_ptr<ast::Node> _parse_primary();

    /**
     * @brief Returns the current semantic scope
     *
     * @return A pointer to the current symbol table
     */
    [[nodiscard]] std::shared_ptr<sem::SymbolTable> _current_scope();

    /**
     * @brief Enters a new semantic scope
     *
     * @return A pointer to the newly created symbol table
     */
    std::shared_ptr<sem::SymbolTable> _enter_scope();

    /**
     * @brief Exits the current semantic scope and returns to the parent
     */
    void _exit_scope();

    /**
     * @brief Returns the current token being processed
     *
     * @return A reference to the current token
     */
    [[nodiscard]] const Token& _current();

    /**
     * @brief Advances to the next token in the stream
     */
    void _next();

    /**
     * @brief Consumes and returns any current token, advancing to the next
     *
     * @return A reference to the consumed token
     */
    const Token& _consume_any();

    /**
     * @brief Consumes a token of the expected type, or throws UnexpectedToken
     *
     * @param type The expected token type
     *
     * @return A reference to the consumed token
     */
    const Token& _consume(Token::Type type);

    /**
     * @brief Tries to consume a token if it matches a predicate
     *
     * @param predicate The condition to check against the current token
     *
     * @return The consumed token if the predicate matches, std::nullopt otherwise
     */
    [[nodiscard]] std::optional<Token> _try_consume(const std::function<bool(const Token&)>& predicate);

    /**
     * @brief Tries to consume a token if it matches the specified type
     *
     * @param type The expected token type
     *
     * @return The consumed token if the type matches, std::nullopt otherwise
     */
    std::optional<Token> _try_consume(Token::Type type);

    /**
     * @brief Converts a token type to its corresponding AST binary operator
     *
     * @param token The token to convert
     *
     * @return The corresponding binary operator
     */
    [[nodiscard]] static ast::Binary::Operator _to_binary_operator(const Token& token);

    /**
     * @brief Checks if a token is a factor-level operator (multiplication, division)
     *
     * @param token The token to check
     *
     * @return True if it's a factor operator, false otherwise
     */
    [[nodiscard]] static bool _is_factor_operator(const Token& token);

    /**
     * @brief Checks if a token is a comparison-level operator
     *
     * @param token The token to check
     *
     * @return True if it's a comparison operator, false otherwise
     */
    [[nodiscard]] static bool _is_comparison_operator(const Token& token);

    /**
     * @brief Checks if a token is a term-level operator (addition, subtraction)
     *
     * @param token The token to check
     *
     * @return True if it's a term operator, false otherwise
     */
    [[nodiscard]] static bool _is_term_operator(const Token& token);

private:
    std::stack<std::shared_ptr<sem::SymbolTable>> _scopes{ };
    std::shared_ptr<pretty_diagnostics::Source> _source;
    std::vector<Token> _tokens;
    size_t _position{ };
    bool _failed{ };
};

/**
 * @brief Base class for all parser-related errors
 */
class ParserError : public std::runtime_error {
public:
    explicit ParserError(const std::string& error) :
        std::runtime_error(error) { }
};

/**
 * @brief Exception thrown when the parser unexpectedly reaches the end of the token stream
 */
class UnexpectedEndOfTokens final : public ParserError {
public:
    UnexpectedEndOfTokens() :
        ParserError("Unexpectedly reached the End Of Tokens") { }
};

/**
 * @brief Exception thrown when an unexpected token is encountered during parsing
 */
class UnexpectedToken final : public ParserError {
public:
    UnexpectedToken(const std::string& expected, const Token& got) :
        ParserError("Expected " + expected + " but got " + to_string(got.type())) { }
};
} // namespace arkoi::front

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
