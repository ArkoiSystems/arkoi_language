#pragma once

#include <sstream>

/**
 * @brief Helper struct for the visitor pattern using `std::variant`.
 *
 * This utility allows for a concise syntax when visiting a `std::variant`
 * by providing multiple lambda expressions.
 *
 * Example usage:
 * @code
 * std::variant<int, float> v = 10;
 * std::visit(match {
 *     [](int i) { std::cout << "int: " << i << "\n"; },
 *     [](float f) { std::cout << "float: " << f << "\n"; }
 * }, v);
 * @endcode
 *
 * @tparam Ts The types (usually lambdas) to be overloaded.
 */
template <class... Ts>
struct match : Ts... {
    using Ts::operator()...;
};

/**
 * @brief Deduction guide for the `match` helper.
 */
template <class... Ts>
match(Ts...) -> match<Ts...>;

/**
 * @brief Temporarily replaces a value and restores it when leaving scope.
 *
 * The original value of `target` is retained for the lifetime of the
 * `ScopedValue`. The replacement can be changed with `set()`, and the original
 * value is restored when the guard is destroyed.
 *
 * @tparam T The guarded value type. It must be nothrow move-constructible and
 * nothrow swappable.
 */
template<typename T>
class [[nodiscard]] ScopedValue final {
    static_assert(std::is_nothrow_move_constructible_v<T>, "ScopedValue requires noexcept move construction");
    static_assert(std::is_nothrow_swappable_v<T>, "ScopedValue requires noexcept swap");

public:
    /**
     * @brief Replaces `target` with `init` for the lifetime of this guard.
     *
     * @param target The value to replace temporarily.
     * @param init The temporary value assigned to `target`.
     */
    explicit ScopedValue(T& target, T init) noexcept 
        :  _target(target), _saved(std::move(init)) { 
        std::swap(_target, _saved);
    }

    /**
     * @brief Replaces `target` with `std::nullopt` for this guard's lifetime.
     *
     * This overload is available when `T` can be constructed from
     * `std::nullopt`.
     *
     * @param target The value to clear temporarily.
     */
    explicit ScopedValue(T& target) noexcept 
        : ScopedValue(target, std::nullopt) {}

    /**
     * @brief Restores the value held by `target` before construction.
     */
    ~ScopedValue() {
         std::swap(_target, _saved);
    }

    ScopedValue(const ScopedValue&) = delete;
    ScopedValue& operator=(const ScopedValue&) = delete;

    ScopedValue(ScopedValue&&) = delete;
    ScopedValue& operator=(ScopedValue&&) = delete;

    /**
     * @brief Assigns a new temporary value to the guarded target.
     *
     * @tparam U The assigned value type.
     * @param value The value to forward to the target's assignment operator.
     */
    template <typename U> requires std::assignable_from<T&, U&&>
    void set(U&& value) noexcept(std::is_nothrow_assignable_v<T&, U&&>) {
        _target = std::forward<U>(value);
    }

    /**
     * @brief Returns a constant reference to the current target value.
     */
    [[nodiscard]] const T& getTarget() const noexcept { return _target; }

    /**
     * @brief Returns a mutable reference to the current target value.
     */
    [[nodiscard]] T& getTarget() noexcept { return _target; }

    /**
     * @brief Returns a constant reference to the value restored on destruction.
     */
    [[nodiscard]] const T& getSaved() const noexcept { return _saved; }

    /**
     * @brief Returns a mutable reference to the value restored on destruction.
     */
    [[nodiscard]] T& getSaved() noexcept { return _saved; }

private:
    T& _target;
    T _saved;
};

/**
 * @brief Deduces the guarded type from the target argument.
 *
 * @tparam T The target's value type.
 * @tparam U The initial temporary value type.
 */
template <typename T, typename U>
ScopedValue(T&, U&&) -> ScopedValue<T>;

/**
 * @brief Converts a value to its string representation using `operator<<`.
 *
 * This is a generic helper that works for any type that has an overloaded
 * `std::ostream& operator<<(std::ostream&, const T&)`.
 *
 * @tparam T The type of the value to convert.
 * @param value The value to convert to a string.
 * @return The string representation of the value.
 */
template <typename T>
std::string to_string(const T& value) {
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

/**
 * @brief Returns the base path of a given file path.
 *
 * For example, if the input is "/home/arkoi/test.ark", the output would be "/home/arkoi/test".
 *
 * @param path The file path to extract the directory from.
 * @return The base directory path.
 */
std::string get_base_path(const std::string& path);

/**
 * @brief Reads the entire content of a file into a single string.
 *
 * This function handles opening the file, reading all bytes until EOF,
 * and closing the file.
 *
 * @param path The filesystem path to the file to be read.
 * @return A string containing the full content of the file.
 * @throws std::runtime_error if the file cannot be opened.
 */
std::string read_file(const std::string& path);

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
