#pragma once

#include <functional>
#include <memory>
#include <stack>
#include <unordered_set>
#include <utility>
#include <vector>

#include "arkoi_language/il/instruction.hpp"

namespace arkoi::il {
class Function;

/**
 * @brief Represents a basic block in the Control Flow Graph (CFG)
 */
class BasicBlock {
public:
    using Predecessors = std::unordered_set<BasicBlock*>;
    using Instructions = std::vector<Instruction>;

public:
    /**
     * @brief Constructs a BasicBlock with the given parameters
     *
     * @param label The label of the basic block
     */
    explicit BasicBlock(std::string label) :
        _branch(), _next(), _label(std::move(label)) { }

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    void accept(Visitor& visitor) { visitor.visit(*this); }

    /**
     * @brief Emplace a new instruction at the end of the block
     *
     * @tparam Type The type of the instruction
     * @tparam Args The argument types for the instruction constructor
     * @param args The arguments for the instruction constructor
     *
     * @return A reference to the newly created instruction.
     */
    template <typename Type, typename... Args>
    Instruction& emplace_back(Args&&... args);

    /**
     * @brief Returns the label of the basic block
     *
     * @return A reference to the label string
     */
    [[nodiscard]] auto& label() const { return _label; }

    /**
     * @brief Returns the branch target (if any)
     *
     * @return A pointer to the branch target basic block
     */
    [[nodiscard]] auto* branch() const { return _branch; }

    /**
     * @brief Sets the branch target
     *
     * @param branch The target basic block
     */
    void set_branch(BasicBlock* branch);

    /**
     * @brief Returns the next sequential basic block
     *
     * @return A pointer to the next basic block
     */
    [[nodiscard]] auto* next() const { return _next; }

    /**
     * @brief Sets the next sequential basic block
     *
     * @param next The next basic block
     */
    void set_next(BasicBlock* next);

    /**
     * @brief Returns the set of predecessor basic blocks
     *
     * @return A reference to the set of predecessors
     */
    [[nodiscard]] auto& predecessors() { return _predecessors; }

    /**
     * @brief Returns the vector of instructions in the block
     *
     * @return A reference to the vector of instructions
     */
    [[nodiscard]] auto& instructions() { return _instructions; }

    /**
     * @brief Returns an iterator to the first instruction
     *
     * @return An iterator to the beginning of the instructions
     */
    Instructions::iterator begin() { return _instructions.begin(); }

    /**
     * @brief Returns an iterator to the end of instructions
     *
     * @return An iterator to the end of the instructions
     */
    Instructions::iterator end() { return _instructions.end(); }

private:
    Instructions _instructions{ };
    Predecessors _predecessors;
    BasicBlock* _branch;
    BasicBlock* _next;
    std::string _label;
};

/**
 * @brief Iterator for traversing basic blocks in a function
 */
class BlockIterator {
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = BasicBlock;
    using reference = value_type&;
    using pointer = value_type*;

public:
    /**
     * @brief Constructs a BlockIterator for a function
     *
     * @param function The function to iterate over
     */
    explicit BlockIterator(Function* function);

    /**
     * @brief Dereferences the iterator
     *
     * @return A reference to the current basic block
     */
    [[nodiscard]] reference operator*() const { return *_current; }

    /**
     * @brief Accesses members of the current basic block
     *
     * @return A pointer to the current basic block
     */
    [[nodiscard]] pointer operator->() const { return _current; }

    /**
     * @brief Pre-increment operator
     *
     * @return A reference to this iterator
     */
    BlockIterator& operator++();

    /**
     * @brief Post-increment operator
     *
     * @return The iterator before incrementing
     */
    BlockIterator operator++(int);

    /**
     * @brief Equality compares only the current pointed element
     *
     * @param left Left-hand block iterator
     * @param right Right-hand block iterator
     *
     * @return True if both iterators refer to the same pointer
     */
    friend bool operator==(const BlockIterator& left, const BlockIterator& right) {
        return left._current == right._current;
    }

    /**
     * @brief Inequality based on `==`
     *
     * @param left Left-hand block iterator
     * @param right Right-hand block iterator
     *
     * @return True if the iterators differ
     */
    friend bool operator!=(const BlockIterator& left, const BlockIterator& right) {
        return !(left == right);
    }

private:
    std::unordered_set<BasicBlock*> _visited{ };
    std::stack<BasicBlock*> _queue{ };
    Function* _function;
    pointer _current;
};

/**
 * @brief Represents a function in the Control Flow Graph (CFG)
 */
class Function {
public:
    /**
     * @brief Constructs a Function with the given parameters and automatically
     *        generated entry and exit labels
     *
     * @param name The name of the function
     * @param parameters The function parameters
     * @param type The return type
     */
    Function(const std::string& name, std::vector<Variable> parameters, sem::Type type);

    /**
     * @brief Constructs a Function with explicit entry and exit labels
     *
     * @param name The name of the function
     * @param parameters The function parameters
     * @param type The return type
     * @param entry_label The label for the entry block
     * @param exit_label The label for the exit block
     */
    Function(
        std::string name, std::vector<Variable> parameters, sem::Type type,
        std::string entry_label, std::string exit_label
    );

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) { visitor.visit(*this); }

    /**
     * @brief Emplace a new basic block in the function
     *
     * @tparam Args The argument types for the BasicBlock constructor
     * @param args The arguments for the BasicBlock constructor
     *
     * @return A pointer to the newly created basic block
     */
    template <typename... Args>
    BasicBlock* emplace_back(Args&&... args);

    /**
     * @brief Checks if the function is a leaf function (no calls)
     *
     * @return True if it's a leaf, false otherwise
     */
    [[nodiscard]] bool is_leaf();

    /**
     * @brief Removes a basic block from the function
     *
     * @param target The basic block to remove
     *
     * @return True if successful, false otherwise
     */
    [[nodiscard]] bool remove(BasicBlock* target);

    /**
     * @brief Returns the name of the function
     *
     * @return A reference to the name string
     */
    [[nodiscard]] auto& name() const { return _name; }

    /**
     * @brief Returns the return type of the function
     *
     * @return A reference to the type
     */
    [[nodiscard]] auto& type() const { return _type; }

    /**
     * @brief Returns the entry basic block
     *
     * @return A pointer to the entry block
     */
    [[nodiscard]] auto* entry() const { return _entry; }

    /**
     * @brief Returns the exit basic block
     *
     * @return A pointer to the exit block
     */
    [[nodiscard]] auto* exit() const { return _exit; }

    /**
     * @brief Sets the exit basic block
     *
     * @param exit The exit block
     */
    void set_exit(BasicBlock* exit) { _exit = exit; }

    /**
     * @brief Returns the function parameters
     *
     * @return A reference to the vector of parameters
     */
    [[nodiscard]] auto& parameters() { return _parameters; }

    /**
     * @brief Returns an iterator to the entry block
     *
     * @return A block iterator
     */
    [[nodiscard]] BlockIterator begin() { return BlockIterator(this); }

    /**
     * @brief Returns an end iterator
     *
     * @return A block iterator
     */
    [[nodiscard]] BlockIterator end() { return BlockIterator(nullptr); }

private:
    std::vector<std::shared_ptr<BasicBlock>> _block_pool{ };
    std::vector<Variable> _parameters;
    BasicBlock* _entry;
    BasicBlock* _exit;
    std::string _name;
    sem::Type _type;
};

/**
 * @brief Represents a module containing multiple functions
 */
class Module {
public:
    using Functions = std::vector<Function>;

public:
    Module() = default;

    /**
     * @brief Accepts a custom defined visitor
     *
     * @param visitor The visitor to accept
     */
    void accept(Visitor& visitor) { visitor.visit(*this); }

    /**
     * @brief Emplace a new function in the module
     *
     * @tparam Args The argument types for the Function constructor
     * @param args The arguments for the Function constructor
     *
     * @return A reference to the newly created function
     */
    template <typename... Args>
    Function& emplace_back(Args&&... args);

    /**
     * @brief Returns an iterator to the first function
     *
     * @return An iterator to the beginning of the functions
     */
    Functions::iterator begin() { return _functions.begin(); }

    /**
     * @brief Returns an iterator to the end of functions
     *
     * @return An iterator to the end of the functions
     */
    Functions::iterator end() { return _functions.end(); }

private:
    Functions _functions{ };
};

#include "../../../src/arkoi_language/il/cfg.tpp"
} // namespace arkoi::il

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
