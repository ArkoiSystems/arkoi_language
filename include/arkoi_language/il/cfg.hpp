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
 * @brief Represents a Basic Block in the Control Flow Graph (CFG).
 *
 * A basic block is a linear sequence of instructions with a single entry point
 * (the first instruction) and a single exit point (the last instruction).
 * In Arkoi IL, basic blocks are linked together to form the CFG of a function.
 */
class BasicBlock {
public:
    using Predecessors = std::unordered_set<BasicBlock*>;
    using Instructions = std::vector<Instruction>;

public:
    /**
     * @brief Constructs a `BasicBlock` with a unique label.
     *
     * @param label The symbolic name of the block (e.g., "L1", "entry").
     */
    explicit BasicBlock(std::string label) :
        _branch(), _next(), _label(std::move(label)) { }

    /**
     * @brief Dispatches the visitor to this basic block.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) { visitor.visit(*this); }

    /**
     * @brief Creates and appends a new instruction to the block.
     *
     * @tparam Type The concrete instruction type (e.g., `Binary`, `Goto`).
     * @tparam Args The constructor argument types for @p Type.
     * @param args The arguments to pass to the instruction constructor.
     * @return A reference to the newly appended `Instruction`.
     */
    template <typename Type, typename... Args>
    Instruction& emplace_back(Args&&... args);

    /**
     * @brief Returns the symbolic label of the block.
     *
     * @return A constant reference to the label string.
     */
    [[nodiscard]] auto& label() const { return _label; }

    /**
     * @brief Returns the non-sequential branch target (if any).
     *
     * For example, in an `If` instruction, this points to the "then" block.
     *
     * @return A pointer to the branch target `BasicBlock`, or nullptr.
     */
    [[nodiscard]] auto* branch() const { return _branch; }

    /**
     * @brief Sets the non-sequential branch target.
     *
     * @param branch Pointer to the destination `BasicBlock`.
     */
    void set_branch(BasicBlock* branch);

    /**
     * @brief Returns the sequential fallthrough target.
     *
     * This is the block that executes if no conditional jump is taken.
     *
     * @return A pointer to the next `BasicBlock`, or nullptr.
     */
    [[nodiscard]] auto* next() const { return _next; }

    /**
     * @brief Sets the sequential fallthrough target.
     *
     * @param next Pointer to the destination `BasicBlock`.
     */
    void set_next(BasicBlock* next);

    /**
     * @brief Returns the set of all blocks that can jump to this one.
     *
     * @return A reference to the set of predecessor `BasicBlock` pointers.
     */
    [[nodiscard]] auto& predecessors() { return _predecessors; }

    /**
     * @brief Returns the sequence of instructions within this block.
     *
     * @return A reference to the vector of `Instruction` objects.
     */
    [[nodiscard]] auto& instructions() { return _instructions; }

    /**
     * @brief Returns an iterator to the first instruction.
     */
    Instructions::iterator begin() { return _instructions.begin(); }

    /**
     * @brief Returns an iterator to the end of the instructions.
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
 * @brief An iterator for traversing basic blocks in a `Function`.
 *
 * This iterator performs a traversal (typically DFS or BFS depending on internal queue)
 * of the CFG starting from the entry block.
 */
class BlockIterator {
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = BasicBlock;
    using reference = value_type&;
    using pointer = value_type*;

public:
    /**
     * @brief Constructs an iterator starting at the entry block of a function.
     *
     * @param function The function to traverse.
     */
    explicit BlockIterator(Function* function);

    /**
     * @brief Dereferences the iterator to the current block.
     */
    [[nodiscard]] reference operator*() const { return *_current; }

    /**
     * @brief Accesses members of the current basic block.
     */
    [[nodiscard]] pointer operator->() const { return _current; }

    /**
     * @brief Advances to the next block in the CFG traversal.
     */
    BlockIterator& operator++();

    /**
     * @brief Post-increment operator.
     */
    BlockIterator operator++(int);

    /**
     * @brief Equality comparison for iterators.
     */
    friend bool operator==(const BlockIterator& left, const BlockIterator& right) {
        return left._current == right._current;
    }

    /**
     * @brief Inequality comparison for iterators.
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
 * @brief Represents a single function in the IL representation.
 *
 * A `Function` holds a collection of `BasicBlock` objects that form its CFG.
 * It also manages the pool of blocks and provides access to entry/exit points.
 */
class Function {
public:
    /**
     * @brief Constructs a `Function` with auto-generated labels.
     *
     * @param name The name of the function.
     * @param parameters The list of input variables.
     * @param type The semantic return type.
     */
    Function(const std::string& name, std::vector<Variable> parameters, sem::Type type);

    /**
     * @brief Constructs a `Function` with custom entry and exit labels.
     *
     * @param name The name of the function.
     * @param parameters The list of input variables.
     * @param type The semantic return type.
     * @param entry_label The label for the start of the function.
     * @param exit_label The label for the return point.
     */
    Function(
        std::string name, std::vector<Variable> parameters, sem::Type type,
        std::string entry_label, std::string exit_label
    );

    /**
     * @brief Dispatches the visitor to this function and all its blocks.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) { visitor.visit(*this); }

    /**
     * @brief Creates and appends a new basic block to the function.
     *
     * @tparam Args The constructor argument types for `BasicBlock`.
     * @param args The arguments to pass to the `BasicBlock` constructor.
     * @return A pointer to the newly created `BasicBlock`.
     */
    template <typename... Args>
    BasicBlock* emplace_back(Args&&... args);

    /**
     * @brief Determines if this function contains any `Call` instructions.
     *
     * @return True if no function calls are present, false otherwise.
     */
    [[nodiscard]] bool is_leaf();

    /**
     * @brief Removes a basic block from the function's CFG.
     *
     * @param target Pointer to the block to be removed.
     * @return True if the block was found and removed, false otherwise.
     */
    [[nodiscard]] bool remove(BasicBlock* target);

    /**
     * @brief Returns the name of the function.
     *
     * @return A constant reference to the name string.
     */
    [[nodiscard]] auto& name() const { return _name; }

    /**
     * @brief Returns the return type of the function.
     *
     * @return A constant reference to the `sem::Type`.
     */
    [[nodiscard]] auto& type() const { return _type; }

    /**
     * @brief Returns the initial entry block of the function.
     *
     * @return Pointer to the entry `BasicBlock`.
     */
    [[nodiscard]] auto* entry() const { return _entry; }

    /**
     * @brief Returns the final exit block of the function.
     *
     * @return Pointer to the exit `BasicBlock`.
     */
    [[nodiscard]] auto* exit() const { return _exit; }

    /**
     * @brief Overrides the current exit block.
     *
     * @param exit Pointer to the new exit `BasicBlock`.
     */
    void set_exit(BasicBlock* exit) { _exit = exit; }

    /**
     * @brief Returns the formal parameters of the function.
     *
     * @return A reference to the vector of parameter `Variable` objects.
     */
    [[nodiscard]] auto& parameters() { return _parameters; }

    /**
     * @brief Returns a CFG traversal iterator starting at the entry block.
     *
     * @return A `BlockIterator` representing the start state.
     */
    [[nodiscard]] BlockIterator begin() { return BlockIterator(this); }

    /**
     * @brief Returns an end-of-traversal iterator.
     *
     * @return A `BlockIterator` representing the end state.
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
 * @brief Represents a compilation unit (module) containing multiple functions.
 */
class Module {
public:
    using Functions = std::vector<Function>;

public:
    Module() = default;

    /**
     * @brief Dispatches the visitor to the module and all its functions.
     *
     * @param visitor The visitor to accept.
     */
    void accept(Visitor& visitor) { visitor.visit(*this); }

    /**
     * @brief Creates and appends a new function to the module.
     *
     * @tparam Args The constructor argument types for `Function`.
     * @param args The arguments to pass to the `Function` constructor.
     * @return A reference to the newly created `Function`.
     */
    template <typename... Args>
    Function& emplace_back(Args&&... args);

    /**
     * @brief Returns an iterator to the first function in the module.
     *
     * @return A `std::vector<Function>::iterator` representing the start state.
     */
    Functions::iterator begin() { return _functions.begin(); }

    /**
     * @brief Returns an iterator to the end of the functions list.
     *
     * @return A `std::vector<Function>::iterator` representing the end state.
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
