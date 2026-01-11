#include "arkoi_language/opt/dead_code_elimination.hpp"

#include <ranges>

#include "arkoi_language/utils/utils.hpp"

using namespace arkoi::opt;
using namespace arkoi;

bool DeadCodeElimination::enter_function(il::Function& function) {
    _used.clear();

    for (auto& block : function) {
        for (auto& instr : block) {
            std::visit(
                match{
                    [&](il::Cast& instruction) {
                        _used.insert(instruction.source());
                    },
                    [&](il::Return& instruction) {
                        _used.insert(instruction.value());
                    },
                    [&](il::If& instruction) {
                        _used.insert(instruction.condition());
                    },
                    [&](il::Store& instruction) {
                        _used.insert(instruction.source());
                    },
                    [&](const il::Load& instruction) {
                        _used.insert(instruction.source());
                    },
                    [&](il::Binary& instruction) {
                        _used.insert(instruction.left());
                        _used.insert(instruction.right());
                    },
                    [&](il::Argument& instruction) {
                        _used.insert(instruction.source());
                    },
                    [&](il::Phi& instruction) {
                        for (auto& operand : instruction.incoming() | std::views::values) {
                            _used.insert(operand);
                        }
                    },
                    [&](il::Assign& instruction) {
                        _used.insert(instruction.value());
                    },
                    [&](il::Call&) { },
                    [&](il::Alloca&) { },
                    [&](il::Goto&) { },
                },
                instr
            );
        }
    }

    return false;
}

bool DeadCodeElimination::on_block(il::BasicBlock& block) {
    return std::erase_if(
        block.instructions(),
        [&](auto& instr) {
            return std::visit(
                match{
                    [&](const il::Binary& instruction) {
                        return !_used.contains(instruction.result());
                    },
                    [&](const il::Load& instruction) {
                        return !_used.contains(instruction.result());
                    },
                    [&](const il::Cast& instruction) {
                        return !_used.contains(instruction.result());
                    },
                    [&](const il::Assign& instruction) {
                        return !_used.contains(instruction.result());
                    },
                    [&](const il::Alloca& instruction) {
                        return !_used.contains(instruction.result());
                    },
                    [&](const il::Store& instruction) {
                        return !_used.contains(instruction.result());
                    },
                    [&](il::Argument&) { return false; },
                    [&](il::Return&) { return false; },
                    [&](il::Goto&) { return false; },
                    [&](il::Call&) { return false; },
                    [&](il::Phi&) { return false; },
                    [&](il::If&) { return false; },
                },
                instr
            );
        }
    );
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
