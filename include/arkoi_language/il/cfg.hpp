#pragma once

#include <cassert>
#include <functional>
#include <memory>
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
 * @brief Provides depth-first traversal utilities for a control flow graph (CFG).
 *
 * `BlockTraversal` can precompute DFS orders of basic blocks starting from
 * a given entry block. It supports PreOrder, PostOrder, and their reverse
 * variants. The resulting traversal order is stored in a `BlockOrder` structure
 * with both vector and index-based access for fast lookups.
 */
class BlockTraversal {
public:
    /**
     * @brief Enum specifying the desired DFS traversal order.
     */
    enum class DFSOrder {
        PreOrder,         ///< Visit each block before its successors.
        ReversePreOrder,  ///< PreOrder traversal, but result vector reversed.
        PostOrder,        ///< Visit each block after its successors.
        ReversePostOrder, ///< PostOrder traversal, but result vector reversed.
    };

    /**
     * @brief Stores the result of a block traversal.
     *
     * Contains a vector of blocks in the computed order, and a map from
     * block pointer to its index in that vector for O(1) access.
     */
    struct BlockOrder {
        std::unordered_map<BasicBlock*, std::size_t> indices{ };
        std::vector<BasicBlock*> blocks{ };
    };

public:
    /**
     * @brief Computes a DFS traversal starting at a given block.
     *
     * @param start Pointer to the starting basic block of the traversal.
     * @param order The DFS traversal order to use (default: PreOrder).
     * @return A BlockOrder containing the blocks in the requested order and their indices.
     */
    static BlockOrder build(BasicBlock* start, DFSOrder order = DFSOrder::PreOrder);

private:
    /**
     * @brief Recursive DFS helper function.
     *
     * Traverses the CFG starting from `current` and appends blocks to the output vector
     * according to the specified DFS order.
     *
     * @param current The block currently being visited.
     * @param order The traversal order (PreOrder / PostOrder / Reverse variants).
     * @param visited Set of blocks that have already been visited (prevents cycles).
     * @param blocks Vector to append blocks to, in the order dictated by `order`.
     */
    static void _dfs(
        BasicBlock* current,
        DFSOrder order,
        std::unordered_set<BasicBlock*>& visited,
        std::vector<BasicBlock*>& blocks
    );
};

/**
 * @brief An iterator for traversing basic blocks in a `Function`.
 *
 * This iterator performs a traversal (typically DFS or BFS depending on the internal queue)
 * of the CFG starting from the entry block.
 */
class BlockIterator {
public:
    using iterator_category = std::random_access_iterator_tag;
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
     * @brief Dereferences the iterator to access the current BasicBlock.
     *
     * @return Reference to the BasicBlock the iterator currently points to.
     *
     * @note Calling this on an end iterator (where _current == nullptr) is undefined
     *       behavior. Use `operator!=` to check against the end iterator first.
     */
    [[nodiscard]] reference operator*() const { return *_current; }

    /**
     * @brief Provides pointer-like access to the current BasicBlock.
     *
     * Allows access to members of the BasicBlock via `it->member`.
     *
     * @return Pointer to the current BasicBlock.
     *
     * @note Calling this on an end iterator (_current == nullptr) is undefined behavior.
     */
    [[nodiscard]] pointer operator->() const { return _current; }

    /**
     * @brief Advances the iterator to the next block in the traversal.
     *
     * @return Reference to the incremented iterator (*this).
     *
     * @note If the iterator is already at the end, calling this has no effect.
     * @note Supports range-based for loops and STL-style algorithms.
     */
    BlockIterator& operator++();

    /**
     * @brief Post-increment: advances the iterator but returns its previous value.
     *
     * @return A copy of the iterator before it was incremented.
     *
     * @note Uses the pre-increment operator internally.
     */
    BlockIterator operator++(int);

    /**
     * @brief Moves the iterator to the previous block in the traversal.
     *
     * @return Reference to the decremented iterator (*this).
     *
     * @note Only valid if the iterator supports bidirectional traversal and
     *       is not at the beginning of the range.
     */
    BlockIterator& operator--();

    /**
     * @brief Post-decrement: moves the iterator backward but returns its previous value.
     *
     * @return A copy of the iterator before it was decremented.
     *
     * @note Uses the pre-decrement operator internally.
     */
    BlockIterator operator--(int);

    /**
     * @brief Random-access: returns a reference to the block at an offset from the current iterator.
     *
     * @param index The number of positions forward from the current iterator.
     * @return Reference to the BasicBlock at the specified offset.
     *
     * @note The caller must ensure that `_index + index < _order.blocks.size()`.
     * @note This makes the iterator compatible with STL random-access algorithms.
     */
    reference operator[](size_t index) const;

    /**
     * @brief Computes the distance between two iterators over the same traversal.
     *
     * @param other Another BlockIterator pointing into the same block order.
     * @return The signed difference in positions between this iterator and `other`.
     *
     * @note Both iterators must refer to the same traversal (same `_order`).
     */
    ptrdiff_t operator-(const BlockIterator& other) const;

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
    BlockTraversal::BlockOrder _order;
    Function* _function;
    pointer _current;
    size_t _index;
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
    Function(const std::string& name, std::vector<Variable> parameters, const sem::Type& type);

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
        std::string name, std::vector<Variable> parameters, const sem::Type& type,
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
     * @brief Returns an iterator to the end of the function list.
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
