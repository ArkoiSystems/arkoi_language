#pragma once

#include <functional>
#include <string>

#include "arkoi_language/ast/visitor.hpp"
#include "arkoi_language/ast/nodes.hpp"

namespace arkoi::ast {
class ASTPrinter : public Visitor {
public:
    explicit ASTPrinter(std::stringstream& output) :
        _output(output) { }

public:
    [[nodiscard]] static std::stringstream print(Program& program);

    void print_child(const std::function<void()>& callback, bool is_last);

    void visit(Program& node) override;

    void visit(Function& node) override;

    void visit(Block& node) override;

    void visit(Parameter& node) override;

    void visit(Immediate& node) override;

    void visit(Variable& node) override;

    void visit(Return& node) override;

    void visit(Identifier& node) override;

    void visit(Binary& node) override;

    void visit(Cast& node) override;

    void visit(Assign& node) override;

    void visit(Call& node) override;

    void visit(If& node) override;

    [[nodiscard]] auto& output() const { return _output; }

private:
    std::stringstream& _output;
    std::string _indentation;
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
