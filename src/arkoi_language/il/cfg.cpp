#include "arkoi_language/il/cfg.hpp"

#include <cassert>
#include <ranges>
#include <utility>

using namespace arkoi::il;

void BasicBlock::set_branch(BasicBlock* branch) {
    if (branch) branch->_predecessors.insert(this);
    _branch = branch;
}

void BasicBlock::set_next(BasicBlock* next) {
    if (next) next->_predecessors.insert(this);
    _next = next;
}

BlockTraversal::BlockOrder BlockTraversal::build(BasicBlock* start, const DFSOrder order) {
    BlockOrder block_order{ };
    if (start == nullptr) {
        return block_order;
    }

    std::unordered_set<BasicBlock*> visited;
    _dfs(start, order, visited, block_order.blocks);

    if (order == DFSOrder::ReversePostOrder || order == DFSOrder::ReversePreOrder) {
        std::reverse(block_order.blocks.begin(), block_order.blocks.end());
    }

    for (size_t index = 0; index < block_order.blocks.size(); index++) {
        auto* block = block_order.blocks[index];
        block_order.indices[block] = index;
    }

    return block_order;
}

void BlockTraversal::_dfs(
    BasicBlock* current,
    const DFSOrder order,
    std::unordered_set<BasicBlock*>& visited,
    std::vector<BasicBlock*>& blocks
) {
    if (!current || visited.contains(current)) {
        return;
    }

    visited.insert(current);

    if (order == DFSOrder::PreOrder || order == DFSOrder::ReversePreOrder) {
        blocks.push_back(current);
    }

    if (current->next()) _dfs(current->next(), order, visited, blocks);
    if (current->branch()) _dfs(current->branch(), order, visited, blocks);

    if (order == DFSOrder::PostOrder || order == DFSOrder::ReversePostOrder) {
        blocks.push_back(current);
    }
}

DominatorTree::Immediates DominatorTree::compute_immediates(const Function& function) {
    Immediates immediates{ };
    if (function.entry() == nullptr) return immediates;

    auto [indices, blocks] = BlockTraversal::build(function.entry(), BlockTraversal::DFSOrder::ReversePostOrder);

    for (auto* block : blocks) {
        immediates[block] = nullptr;
    }
    immediates[function.entry()] = function.entry();

    bool changed = true;
    while (changed) {
        changed = false;

        for (size_t index = 1; index < blocks.size(); index++) {
            auto* current = blocks[index];

            BasicBlock* dominator = nullptr;
            for (auto* predecessor : current->predecessors()) {
                if (immediates.at(predecessor) != nullptr) {
                    dominator = predecessor;
                    break;
                }
            }

            if (!dominator) continue;

            for (auto* predecessor : current->predecessors()) {
                if (immediates.at(predecessor) != nullptr) {
                    dominator = _intersect(predecessor, dominator, immediates, indices);
                }
            }

            if (immediates[current] != dominator) {
                immediates[current] = dominator;
                changed = true;
            }
        }
    }

    immediates[function.entry()] = nullptr;

    return immediates;
}

DominatorTree::Frontiers DominatorTree::compute_frontiers(const Function& function) {
    Frontiers frontiers{ };
    if (function.entry() == nullptr) return frontiers;

    auto immediates = compute_immediates(function);
    for (auto* block : immediates | std::views::keys) {
        frontiers[block] = { };
    }

    for (auto* block : immediates | std::views::keys) {
        if (block->predecessors().size() >= 2) {
            for (auto* predecessor : block->predecessors()) {
                auto* runner = predecessor;
                while (runner && runner != immediates.at(block)) {
                    frontiers[runner].insert(block);
                    runner = immediates.at(runner);
                }
            }
        }
    }

    return frontiers;
}

BasicBlock* DominatorTree::_intersect(
    BasicBlock* u,
    BasicBlock* v,
    const Immediates& immediates,
    const BlockTraversal::BlockOrder::Indices& rpo_indices
) {
    while (u != v) {
        while (rpo_indices.at(u) > rpo_indices.at(v)) {
            u = immediates.at(u);
        }

        while (rpo_indices.at(v) > rpo_indices.at(u)) {
            v = immediates.at(v);
        }
    }

    return u;
}

BlockIterator::BlockIterator(Function* function) :
    _function(function), _current(nullptr), _index(0) {
    if (!_function || !_function->entry()) return;

    _order = BlockTraversal::build(_function->entry(), BlockTraversal::DFSOrder::PreOrder);
    if (!_order.blocks.empty()) {
        _current = _order.blocks[_index];
    }
}

BlockIterator& BlockIterator::operator++() {
    if (!_current) return *this;

    _index++;
    if (_index < _order.blocks.size()) {
        _current = _order.blocks[_index];
    } else {
        _current = nullptr;
    }

    return *this;
}

BlockIterator BlockIterator::operator++(int) {
    BlockIterator temp = *this;
    std::ignore = ++(*this);
    return temp;
}

BlockIterator& BlockIterator::operator--() {
    if (_index > 0) {
        --_index;
        _current = _order.blocks[_index];
    }

    return *this;
}

BlockIterator BlockIterator::operator--(int) {
    BlockIterator temp = *this;
    std::ignore = --(*this);
    return temp;
}

BlockIterator::reference BlockIterator::operator[](const size_t index) const {
    return *_order.blocks[_index + index];
}

ptrdiff_t BlockIterator::operator-(const BlockIterator& other) const {
    return _index - other._index;
}

Function::Function(const std::string& name, std::vector<Variable> parameters, const sem::Type& type) :
    Function(name, std::move(parameters), type, name + "_entry", name + "_exit") { }

Function::Function(
    std::string name, std::vector<Variable> parameters, const sem::Type& type, std::string entry_label,
    std::string exit_label
) :
    _parameters(std::move(parameters)), _name(std::move(name)), _type(type) {
    _entry = emplace_back(std::move(entry_label));
    _exit = emplace_back(std::move(exit_label));
}

BasicBlock* Function::emplace_back(const std::string& label) {
    auto block = std::make_shared<BasicBlock>(label);
    _block_pool.emplace(label, block);
    return block.get();
}

bool Function::is_leaf() {
    for (auto& block : *this) {
        for (const auto& instruction : block) {
            if (std::holds_alternative<Call>(instruction)) return false;
        }
    }

    return true;
}

bool Function::remove(BasicBlock* target) {
    assert(target->predecessors().empty());

    if (target->next()) target->next()->predecessors().erase(target);
    if (target->branch()) target->branch()->predecessors().erase(target);

    return _block_pool.erase(target->label());
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
