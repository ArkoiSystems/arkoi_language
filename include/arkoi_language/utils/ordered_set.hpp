#pragma once

#include <unordered_set>
#include <vector>

/**
 * @brief A set that maintains the insertion order of its elements.
 *
 * OrderedSet combines the properties of `std::unordered_set` (for fast lookup)
 * and `std::vector` (to preserve insertion order). This is particularly useful
 * in compiler passes where deterministic iteration order is required.
 *
 * @tparam T The type of elements in the set. Must be hashable and comparable.
 */
template <typename T>
class OrderedSet {
public:
    using const_iterator = std::vector<T>::const_iterator;
    using iterator = std::vector<T>::iterator;

public:
    /**
     * @brief Inserts a value into the set if it doesn't already exist.
     *
     * Complexity: O(1) average.
     *
     * @param value The value to insert.
     * @return True if the value was inserted, false if it already existed.
     * @see contains, erase
     */
    bool insert(const T& value);

    /**
     * @brief Erases a value from the set.
     *
     * Complexity: O(N) where N is the number of elements, as it requires
     * removing from the internal vector.
     *
     * @param value The value to erase.
     * @return True if the value was erased, false if it didn't exist.
     * @see insert
     */
    bool erase(const T& value);

    /**
     * @brief Checks if the set contains a value.
     *
     * Complexity: O(1) average.
     *
     * @param value The value to check.
     * @return True if the value exists, false otherwise.
     * @see insert
     */
    [[nodiscard]] bool contains(const T& value) const;

    /**
     * @brief Clears all elements from the set.
     */
    void clear();

    /**
     * @brief Returns the number of elements in the set.
     *
     * @return The size of the set.
     * @see empty
     */
    [[nodiscard]] size_t size() const { return _set.size(); }

    /**
     * @brief Checks if the set is empty.
     *
     * @return True if empty, false otherwise.
     * @see size
     */
    [[nodiscard]] bool empty() const { return _set.empty(); }

    /**
     * @brief Returns a const iterator to the first element.
     *
     * The iteration order follows the insertion order.
     *
     * @return A const iterator to the beginning of the set.
     */
    const_iterator begin() const { return _vector.begin(); }

    /**
     * @brief Returns a const iterator to the end.
     *
     * @return A const iterator to the end of the set.
     */
    const_iterator end() const { return _vector.end(); }

    /**
     * @brief Returns an iterator to the first element.
     *
     * The iteration order follows the insertion order.
     *
     * @return An iterator to the beginning of the set.
     */
    iterator begin() { return _vector.begin(); }

    /**
     * @brief Returns an iterator to the end.
     *
     * @return An iterator to the end of the set.
     */
    iterator end() { return _vector.end(); }

private:
    std::unordered_set<T> _set;
    std::vector<T> _vector;
};

#include "../../../src/arkoi_language/utils/ordered_set.tpp"

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
