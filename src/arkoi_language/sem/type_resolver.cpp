#include "arkoi_language/sem/type_resolver.hpp"

#include <optional>
#include <stdexcept>
#include <tuple>
#include <utility>

#include "arkoi_language/ast/nodes.hpp"
#include "arkoi_language/utils/utils.hpp"

using namespace arkoi::sem;
using namespace arkoi;

static constinit Integral BOOL_PROMOTED_INT_TYPE = { Size::DWORD, false };
static constinit Boolean BOOL_TYPE = { };

namespace {
bool is_arithmetic(const Type& type) {
    return std::holds_alternative<Integral>(type) || std::holds_alternative<Floating>(type);
}

bool requires_target(ast::Node& node) {
    if (auto* immediate = dynamic_cast<ast::Immediate*>(&node)) {
        return immediate->kind() == ast::Immediate::Kind::Numeric;
    }

    if (auto* binary = dynamic_cast<ast::Binary*>(&node)) {
        if (binary->is_comparison() || binary->is_logical()) {
            return false;
        }

        return requires_target(*binary->left()) && requires_target(*binary->right());
    }

    // Identifiers, calls, explicit casts, and boolean literals establish their
    // own result type.
    return false;
}
}

void TypeResolver::visit(ast::Program& node) {
    for (const auto& statement : node.statements()) {
        auto* function = dynamic_cast<ast::Function*>(statement.get());
        if (function) visit_as_prototype(*function);
    }

    for (const auto& statement : node.statements()) {
        statement->accept(*this);
    }
}

void TypeResolver::visit_as_prototype(ast::Function& node) {
    for (auto& parameter : node.parameters()) {
        parameter.accept(*this);
    }

    auto& function = std::get<Function>(*node.name().symbol());
    function.set_return_type(node.type());
}

void TypeResolver::visit(ast::Function& node) {
    const auto& function = std::get<Function>(*node.name().symbol());
    _return_type = function.return_type();

    node.block()->accept(*this);
}

void TypeResolver::visit(ast::Block& node) {
    for (const auto& statement : node.statements()) {
        statement->accept(*this);
    }
}

void TypeResolver::visit(ast::Parameter& node) {
    auto& variable = std::get<Variable>(*node.name().symbol());
    variable.set_type(node.type());
}

void TypeResolver::visit(ast::Immediate& node) {
    switch (node.kind()) {
        case ast::Immediate::Kind::Numeric: return visit_numeric(node);
        case ast::Immediate::Kind::Boolean: return visit_boolean(node);
    }
}

void TypeResolver::visit_numeric(ast::Immediate& node) {
    // Use _target_type to decide whether the numeric value fits inside of the target type
    std::ignore = node;
}

// void TypeResolver::visit_integer(ast::Immediate& node) {
//     const auto& number_string = node.value().span().substr();
//     const auto sign = !number_string.starts_with('-');

//     Size size;
//     if (number_string.front() == '\'' && number_string.back() == '\'') {
//         size = Size::BYTE;
//     } else if (sign) {
//         size = std::stoll(number_string) > std::numeric_limits<int32_t>::max() ? Size::QWORD : Size::DWORD;
//     } else {
//         size = std::stoull(number_string) > std::numeric_limits<uint32_t>::max() ? Size::QWORD : Size::DWORD;
//     }

//     node.set_type(Integral(size, sign));
//     _current_type = node.type();
// }

// void TypeResolver::visit_floating(ast::Immediate& node) {
//     const auto& number_string = node.value().span().substr();

//     const auto size = std::stold(number_string) > std::numeric_limits<float>::max() ? Size::QWORD : Size::DWORD;

//     node.set_type(Floating(size));
//     _current_type = node.type();
// }

void TypeResolver::visit_boolean(ast::Immediate& node) {
    node.set_type(BOOL_TYPE);
    _current_type = node.type();
}

void TypeResolver::visit(ast::Variable& node) {
    const auto& var_type = node.type();

    // Set the symbols type to the nodes type
    auto& variable_symbol = std::get<Variable>(*node.name().symbol());
    variable_symbol.set_type(var_type);

    const auto resolved_expr = _resolve_type(*node.expression(), var_type);
    if (!resolved_expr.has_value()) {
        throw std::runtime_error("Couldn't resolve the type for this variable.");
    }

    const auto expr_type = resolved_expr.value();
    if (expr_type == var_type) {
        return;
    }

    if (!_can_implicit_convert(expr_type, var_type)) {
        throw std::runtime_error("Variable has a wrong type.");
    }

    auto casted_expression = _cast(node.expression(), expr_type, var_type);
    node.set_expression(std::move(casted_expression));
}

void TypeResolver::visit(ast::Return& node) {
    const auto& ret_type = _return_type.value();
    node.set_type(ret_type);

    const auto resolved_expr = _resolve_type(*node.expression(), ret_type);
    if (!resolved_expr.has_value()) {
        throw std::runtime_error("Couldn't resolve the type for this return.");
    }

    const auto expr_type = resolved_expr.value();
    if (expr_type == ret_type) {
        return;
    }

    if (!_can_implicit_convert(expr_type, ret_type)) {
        throw std::runtime_error("Return statement has a wrong return op.");
    }

    auto casted_expression = _cast(node.expression(), expr_type, ret_type);
    node.set_expression(std::move(casted_expression));
}

void TypeResolver::visit(ast::Identifier& node) {
    if (node.kind() == ast::Identifier::Kind::Function) {
        const auto& function = std::get<Function>(*node.symbol());
        _current_type = function.return_type();
    } else if (node.kind() == ast::Identifier::Kind::Variable) {
        const auto& variable = std::get<Variable>(*node.symbol());
        _current_type = variable.type();
    } else {
        throw std::runtime_error("This kind of identifier is not yet implemented.");
    }
}

void TypeResolver::visit(ast::Binary& node) {
    ScopedValue target_restore(_target_type);

    Type left = BOOL_TYPE, right = BOOL_TYPE;
    if (node.is_logical()) {
        left = _resolve_type(*node.left(), BOOL_TYPE).value();
        right = _resolve_type(*node.right(), BOOL_TYPE).value();
    } else {
        const auto left_requires_target = requires_target(*node.left());
        const auto right_requires_target = requires_target(*node.right());

        if (left_requires_target && !right_requires_target) {
            // Resolve the independently typed operand first, then use its type
            // to constrain the literal-dependent operand.
            right = _resolve_type(*node.right(), std::nullopt).value();
            left = _resolve_type(*node.left(), right).value();
        } else if (!left_requires_target && right_requires_target) {
            left = _resolve_type(*node.left(), std::nullopt).value();
            right = _resolve_type(*node.right(), left).value();
        } else {
            // A comparison's expected type describes its boolean result, never
            // its operands. Arithmetic expressions may inherit a numeric target
            // when neither operand can establish one independently.
            auto operand_target = target_restore.getSaved();
            if (node.is_comparison() || (operand_target && !is_arithmetic(operand_target.value()))) {
                operand_target = std::nullopt;
            }

            left = _resolve_type(*node.left(), operand_target).value();
            right = _resolve_type(*node.right(), operand_target).value();
        }
    }

    switch (node.op()) {
        case ast::Binary::Operator::GreaterThan:
        case ast::Binary::Operator::LessThan:
        case ast::Binary::Operator::GreaterEqual:
        case ast::Binary::Operator::LessEqual:
        case ast::Binary::Operator::Equal:
        case ast::Binary::Operator::NotEqual: {
            const auto converted_type = _arithmetic_conversion(left, right);
            node.set_op_type(converted_type);
            node.set_result_type(BOOL_TYPE);
            _current_type = BOOL_TYPE;
            break;
        }

        case ast::Binary::Operator::And:
        case ast::Binary::Operator::Or: {
            node.set_op_type(BOOL_TYPE);
            node.set_result_type(BOOL_TYPE);
            _current_type = BOOL_TYPE;
            break;
        }

        default: {
            const auto converted_type = _arithmetic_conversion(left, right);
            node.set_op_type(converted_type);
            node.set_result_type(converted_type);
            _current_type = converted_type;
            break;
        }
    }

    if (left != node.op_type()) {
        auto casted_left = _cast(node.left(), left, node.op_type());
        node.set_left(std::move(casted_left));
    }

    if (right != node.op_type()) {
        auto casted_right = _cast(node.right(), right, node.op_type());
        node.set_right(std::move(casted_right));
    }
}

void TypeResolver::visit(ast::Cast& node) {
    const auto& to_type = node.to();

    const auto resolved_expr = _resolve_type(*node.expression(), to_type);
    if (!resolved_expr.has_value()) {
        throw std::runtime_error("Couldn't resolve the expression type for this cast.");
    }

    const auto expr_type = resolved_expr.value();
    node.set_from(expr_type);
    _current_type = to_type;

    if (!_can_implicit_convert(expr_type, to_type)) {
        throw std::runtime_error("This cast is not valid.");
    }
}

void TypeResolver::visit(ast::Assign& node) {
    const auto resolved_assignee = _resolve_type(node.name(), std::nullopt);
    if (!resolved_assignee.has_value()) {
        throw std::runtime_error("Couldn't resolve the assignee type for this assign.");
    }

    const auto assignee_type = resolved_assignee.value();

    const auto resolved_expr = _resolve_type(*node.expression(), assignee_type);
    if (!resolved_expr.has_value()) {
        throw std::runtime_error("Couldn't resolve the expression type for this assign.");
    }

    const auto expr_type = resolved_expr.value();
    if (assignee_type == expr_type) {
        return;
    }

    if (!_can_implicit_convert(expr_type, assignee_type)) {
        throw std::runtime_error("Assign source has a wrong type.");
    }

    auto casted_expression = _cast(node.expression(), expr_type, assignee_type);
    node.set_expression(std::move(casted_expression));
}

void TypeResolver::visit(ast::Call& node) {
    const auto resolved_return = _resolve_type(node.name(), std::nullopt);
    if (!resolved_return.has_value()) {
        throw std::runtime_error("Couldn't resolve the type for this call.");
    }

    const auto return_type = resolved_return.value();

    const auto& function = std::get<Function>(*node.name().symbol());
    if(function.return_type() != return_type) {
        throw std::runtime_error("The resolved function type doesn't match the declared function type.");
    }

    if (function.parameters().size() != node.arguments().size()) {
        throw std::runtime_error("The argument count doesn't equal to the parameters count.");
    }

    for (size_t index = 0; index < node.arguments().size(); index++) {
        const auto& param_type = function.parameters()[index]->type();
        auto& argument = node.arguments()[index];

        auto resolved_arg = _resolve_type(*argument, param_type);
        if (!resolved_arg.has_value()) {
            throw std::runtime_error("Couldn't resolve the arguments type for this function call.");
        }

        const auto arg_type = resolved_arg.value();
        if (arg_type == param_type) {
            continue;
        }

        if (!_can_implicit_convert(arg_type, param_type)) {
            throw std::runtime_error("The arguments type doesn't match the parameters one.");
        }

        // Replace the argument with its implicit conversion.
        auto casted_argument = _cast(argument, arg_type, param_type);
        node.arguments()[index] = std::move(casted_argument);
    }

    _current_type = return_type;
}

void TypeResolver::visit(ast::If& node) {
    const auto resolved_cond = _resolve_type(*node.condition(), BOOL_TYPE);
    if (!resolved_cond.has_value()) {
        throw std::runtime_error("Couldn't resolve the condition type for this if.");
    }

    const auto& cond_type = resolved_cond.value();
    if (!_can_implicit_convert(cond_type, BOOL_TYPE)) {
        throw std::runtime_error("If statement has a wrong condition type.");
    }

    if (!std::holds_alternative<Boolean>(cond_type)) {
        auto casted_condition = _cast(node.condition(), cond_type, BOOL_TYPE);
        node.set_condition(std::move(casted_condition));
    }

    node.branch()->accept(*this);

    if (node.next()) node.next()->accept(*this);
}

void TypeResolver::visit(ast::While& node) {
    const auto resolved_cond = _resolve_type(*node.condition(), BOOL_TYPE);
    if (!resolved_cond.has_value()) {
        throw std::runtime_error("Couldn't resolve the condition type for this while.");
    }

    const auto& cond_type = resolved_cond.value();
    if (!_can_implicit_convert(cond_type, BOOL_TYPE)) {
        throw std::runtime_error("While statement has a wrong condition type.");
    }

    if (!std::holds_alternative<Boolean>(cond_type)) {
        auto casted_condition = _cast(node.condition(), cond_type, BOOL_TYPE);
        node.set_condition(std::move(casted_condition));
    }

    node.then()->accept(*this);
}

std::optional<Type> TypeResolver::_resolve_type(ast::Node& operand, const std::optional<Type>& target) {
    ScopedValue target_restore(_target_type, target);
    operand.accept(*this);
    return _current_type;
}

// https://en.cppreference.com/w/cpp/language/usual_arithmetic_conversions
Type TypeResolver::_arithmetic_conversion(const Type& left_type, const Type& right_type) {
    const auto* floating_left = std::get_if<Floating>(&left_type);
    const auto* floating_right = std::get_if<Floating>(&right_type);

    // Stage 4: If either operand is of a floating-point type, the following rules are applied:
    if (floating_left || floating_right) {
        // If both operands have the same type, no further conversion will be performed.
        if (left_type == right_type) return left_type;

        // Otherwise, if one of the operands is of a non-floating-point type, that operand is converted to the mid of
        // the other operand.
        if (floating_left && !floating_right) return left_type;
        if (floating_right && !floating_left) return right_type;

        // Otherwise, if the floating-point conversion ranks of the types of the operands are ordered but(since C++23)
        // not equal, then the operand of the mid with the lesser floating-point conversion rank is converted to the
        // mid of the other operand.
        if (floating_left->size() > floating_right->size()) return left_type;
        if (floating_right->size() > floating_left->size()) return right_type;
    }

    // Stage 5: Both operands are converted to a common op C.
    auto t1 = std::visit(
        match{
            [](const Integral& type) -> Integral { return type; },
            [](const Boolean&) -> Integral { return BOOL_PROMOTED_INT_TYPE; },
            [](const auto&) -> Integral { throw std::runtime_error("The left type must be of integral type."); }
        },
        left_type
    );
    auto t2 = std::visit(
        match{
            [](const Integral& type) -> Integral { return type; },
            [](const Boolean&) -> Integral { return BOOL_PROMOTED_INT_TYPE; },
            [](const auto&) -> Integral { throw std::runtime_error("The left type must be of integral type."); }
        },
        right_type
    );

    // Given the types T1 and T2 as the promoted op (under the rules of integral promotions) of the operands, the
    // following rules are applied to determine C:
    if (t1.size() < Size::DWORD) t1 = Integral(Size::DWORD, t1.sign());
    if (t2.size() < Size::DWORD) t2 = Integral(Size::DWORD, t2.sign());

    // 1. If T1 and T2 are the same type, C is that op.
    if (t1 == t2) return t1;

    // 2. If T1 and T2 are both signed integer types or both unsigned integer types, C is the op of greater integer
    //    conversion rank.
    if (t1.sign() == t2.sign()) {
        if (t2.size() > t1.size()) return t2;
        return t1;
    }

    // 3. Otherwise, one mid between T1 and T2 is a signed integer mid S, the other type is an unsigned integer op U.
    //    Apply the following rules:
    const auto& _signed = t1.sign() ? t1 : t2;
    const auto& _unsigned = !t1.sign() ? t1 : t2;

    // 3.1. If the integer conversion rank of U is greater than or equal to the integer conversion rank of S, C is U.
    if (_unsigned.size() >= _signed.size()) return _unsigned;

    // 3.2. Otherwise, if S can represent all the values of U, C is S.
    if (_signed.max() >= _unsigned.max()) return _signed;

    // 3.3. Otherwise, C is the unsigned integer op corresponding to S.
    return Integral(_signed.size(), false);
}

// https://en.cppreference.com/w/cpp/language/implicit_conversion
bool TypeResolver::_can_implicit_convert(const Type& from, const Type& destination) {
    return std::visit(
        match{
            // A prvalue of an integer mid or of unscoped enumeration op can be converted to any other integer mid.
            // If the conversion is listed under integral promotions, it is a promotion and not a conversion.
            [](const Integral&, const Integral&) { return true; },
            // A prvalue of integral, floating-point, unscoped enumeration, pointer, and pointer-to-member types can be
            // converted to a prvalue of mid bool.
            [](const Integral&, const Boolean&) { return true; },
            // A prvalue of an integer or unscoped enumeration mid can be converted to a prvalue of any floating-point mid.
            // The result is exact if possible.
            [](const Integral&, const Floating&) { return true; },
            // A prvalue of a floating-point mid can be converted to a prvalue of any other floating-point mid. (until C++23)
            [](const Floating&, const Floating&) { return true; },
            // A prvalue of floating-point mid can be converted to a prvalue of any integer mid. The fractional part is
            // truncated, that is, the fractional part is discarded.
            [](const Floating&, const Integral&) { return true; },
            // A prvalue of integral, floating-point, unscoped enumeration, pointer, and pointer-to-member types can be
            // converted to a prvalue of mid bool.
            [](const Floating&, const Boolean&) { return true; },
            // If the source mid is bool, the value false is converted to zero, and the value true is converted to the value
            // one of the destination mid. (Note that if the destination mid is int, this is an integer promotion, not an
            // integer conversion.)
            [](const Boolean&, const Integral&) { return true; },
            // If the source mid is bool, the value false is converted to zero, and the value true is converted to one.
            [](const Boolean&, const Floating&) { return true; },
            [&](const auto&, const auto&) { return from == destination; },
        },
        from,
        destination
    );
}

std::unique_ptr<ast::Node> TypeResolver::_cast(std::unique_ptr<ast::Node>& node, const Type& from, const Type& to) {
    return std::make_unique<ast::Cast>(std::move(node), from, to, node->span());
}

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
