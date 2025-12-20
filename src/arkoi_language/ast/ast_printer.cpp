#include "arkoi_language/ast/ast_printer.hpp"

#include <sstream>

#include "arkoi_language/ast/nodes.hpp"

using namespace arkoi::ast;

static constexpr std::string NON_LAST_CHILD_PREFIX = "├─ ";
static constexpr std::string LAST_CHILD_PREFIX = "└─ ";
static constexpr std::string NON_LAST_INDENTATION = "│  ";
static constexpr std::string LAST_INDENTATION = "   ";

std::stringstream ASTPrinter::print(Program& program) {
    std::stringstream output;
    ASTPrinter printer(output);
    printer.visit(program);
    return output;
}

void ASTPrinter::print_child(const std::function<void()>& callback, const bool is_last) {
    _output << _indentation;
    _output << (is_last ? LAST_CHILD_PREFIX : NON_LAST_CHILD_PREFIX);

    const auto indent_before = _indentation;
    _indentation += (is_last ? LAST_INDENTATION : NON_LAST_INDENTATION);

    callback();

    _indentation = indent_before;
}

void ASTPrinter::visit(Program& node) {
    _output << "Program" << std::endl;

    print_child(
        [&]() {
            _output << "Statements" << std::endl;

            for (size_t index = 0; index < node.statements().size(); ++index) {
                const bool is_last = (index == node.statements().size() - 1);

                const auto& statement = node.statements()[index];
                print_child([&]() { statement->accept(*this); }, is_last);
            }
        },
        true
    );
}

void ASTPrinter::visit(Function& node) {
    _output << "Function" << std::endl;

    print_child([&]() { node.name().accept(*this); }, false);

    print_child(
        [&]() {
            _output << "Parameters" << std::endl;

            for (size_t index = 0; index < node.parameters().size(); ++index) {
                const bool is_last = (index == node.parameters().size() - 1);

                auto& parameter = node.parameters()[index];
                print_child([&]() { parameter.accept(*this); }, is_last);
            }
        },
        false
    );

    print_child([&]() { node.block()->accept(*this); }, true);
}

void ASTPrinter::visit(Block& node) {
    _output << "Block" << std::endl;

    print_child(
        [&]() {
            _output << "Statements" << std::endl;

            for (size_t index = 0; index < node.statements().size(); ++index) {
                const bool is_last = (index == node.statements().size() - 1);

                const auto& statement = node.statements()[index];
                print_child([&]() { statement->accept(*this); }, is_last);
            }
        },
        true
    );
}

void ASTPrinter::visit(Parameter& node) {
    _output << "Parameter" << std::endl;

    print_child([&]() { node.name().accept(*this); }, true);
}

void ASTPrinter::visit(Immediate& node) {
    _output << "Immediate[value=" << node.value().span().substr() << ", kind=" << Immediate::to_string(node.kind()) << "]" << std::endl;
}

void ASTPrinter::visit(Variable& node) {
    _output << "Variable" << std::endl;

    print_child([&]() { node.name().accept(*this); }, false);
    print_child([&]() { node.expression()->accept(*this); }, true);
}

void ASTPrinter::visit(Return& node) {
    _output << "Return" << std::endl;

    print_child([&]() { node.expression()->accept(*this); }, true);
}

void ASTPrinter::visit(Identifier& node) {
    _output << "Identifier[value=" << node.value().span().substr() << ", kind=" << Identifier::to_string(node.kind()) << "]" << std::endl;
}

void ASTPrinter::visit(Binary& node) {
    _output << "Binary[op=" << Binary::to_string(node.op()) << "]" << std::endl;

    print_child([&]() { node.left()->accept(*this); }, false);
    print_child([&]() { node.right()->accept(*this); }, true);
}

void ASTPrinter::visit(Cast& node) {
    _output << "Cast" << std::endl;

    print_child([&]() { node.expression()->accept(*this); }, true);
}

void ASTPrinter::visit(Assign& node) {
    _output << "Assign" << std::endl;

    print_child([&]() { node.name().accept(*this); }, false);
    print_child([&]() { node.expression()->accept(*this); }, true);
}

void ASTPrinter::visit(Call& node) {
    _output << "Call" << std::endl;

    print_child(
        [&]() {
            _output << "Arguments" << std::endl;

            for (size_t index = 0; index < node.arguments().size(); ++index) {
                const bool is_last = (index == node.arguments().size() - 1);

                const auto& argument = node.arguments()[index];
                print_child([&]() { argument->accept(*this); }, is_last);
            }
        },
        true
    );
}

void ASTPrinter::visit(If& node) {
    _output << "If" << std::endl;

    print_child([&]() { node.condition()->accept(*this); }, false);
    print_child([&]() { node.branch()->accept(*this); }, node.next() == nullptr);

    if (!node.next()) return;

    print_child([&]() { node.next()->accept(*this); }, node.next() != nullptr);
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
