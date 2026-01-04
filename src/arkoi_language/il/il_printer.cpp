#include "arkoi_language/il/il_printer.hpp"

#include <iostream>

#include "arkoi_language/il/cfg.hpp"
#include "arkoi_language/il/instruction.hpp"
#include "arkoi_language/utils/utils.hpp"

using namespace arkoi::il;

std::stringstream ILPrinter::print(Module& module) {
    std::stringstream output;
    ILPrinter printer(output);
    printer.visit(module);
    return output;
}

void ILPrinter::visit(Module& module) {
    for (auto& function : module) {
        function.accept(*this);
    }
}

void ILPrinter::visit(Function& function) {
    _output << "fun " << function.name() << "(";

    for (size_t index = 0; index < function.parameters().size(); index++) {
        auto& parameter = function.parameters()[index];
        _output << parameter.name() << " @" << parameter.type();

        if (index != function.parameters().size() - 1) {
            _output << ", ";
        }
    }

    _output << ") @" << function.type() << ":\n";

    for (auto& block : function) {
        block.accept(*this);
    }

    _output << "\n";
}

void ILPrinter::visit(BasicBlock& block) {
    _output << block.label() << ":\n";

    for (auto& instruction : block.instructions()) {
        _output << "  ";
        instruction.accept(*this);
        _output << "\n";
    }
}

void ILPrinter::visit(Return& instruction) {
    _output << "ret " << instruction.value();
}

void ILPrinter::visit(Binary& instruction) {
    _output << instruction.result() << " @" << instruction.result().type();
    _output << " = " << to_string(instruction.op()) << " @" << instruction.op_type();
    _output << " " << instruction.left() << ", " << instruction.right();
}

void ILPrinter::visit(Cast& instruction) {
    _output << instruction.result() << " @" << instruction.result().type();
    _output << " = cast @" << instruction.from() << " " << instruction.source();
}

void ILPrinter::visit(Argument& argument) {
    _output << "arg @" << argument.source().type() << " " << argument.source();
}

void ILPrinter::visit(Call& instruction) {
    _output << instruction.result() << " @" << instruction.result().type();
    _output << " = call " << instruction.name() << ", " << instruction.arguments().size();
}

void ILPrinter::visit(Goto& instruction) {
    _output << "goto " << instruction.label();
}

void ILPrinter::visit(If& instruction) {
    _output << "if " << instruction.condition();
    _output << " then " << instruction.branch();
    _output << " else " << instruction.next();
}

void ILPrinter::visit(Alloca& instruction) {
    _output << instruction.result() << " @" << instruction.result().type();
    _output << " = alloca";
}

void ILPrinter::visit(Store& instruction) {
    _output << instruction.result() << " @" << instruction.result().type();
    _output << " = store " << instruction.source();
}

void ILPrinter::visit(Load& instruction) {
    _output << instruction.result() << " @" << instruction.result().type();
    _output << " = load " << instruction.source();
}

void ILPrinter::visit(Constant& instruction) {
    _output << instruction.result() << " @" << instruction.result().type();
    _output << " = const " << instruction.immediate();
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
