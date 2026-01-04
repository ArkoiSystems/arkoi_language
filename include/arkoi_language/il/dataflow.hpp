#pragma once

#include <concepts>
#include <vector>
#include <stack>

#include "arkoi_language/il/cfg.hpp"
#include "arkoi_language/il/instruction.hpp"

namespace arkoi::il {
/**
 * @brief Specifies the direction of information flow in a dataflow analysis.
 */
enum class DataflowDirection {
    Forward, ///< Information flows from entry to exit (e.g., reaching definitions)
    Backward ///< Information flows from exit to entry (e.g., liveness analysis)
};

/**
 * @brief Specifies the unit of analysis for a dataflow pass.
 */
enum class DataflowGranularity {
    Block,      ///< Analysis computed once per `BasicBlock`
    Instruction ///< Analysis computed once per `Instruction`
};

/**
 * @brief Abstract base class for defining a dataflow analysis pass.
 *
 * A dataflow pass is defined by its domain (ResultType), its direction,
 * and its granularity. Subclasses must implement the meet (merge),
 * initialization, and transfer functions.
 *
 * @tparam ResultType The type of data stored in the dataflow sets (e.g., `Operand`).
 * @tparam DirectionType Whether the analysis is forward or backward.
 * @tparam GranularityType Whether the analysis operates on blocks or instructions.
 *
 * @see DataflowAnalysis, BlockLivenessAnalysis
 */
template <typename ResultType, DataflowDirection DirectionType, DataflowGranularity GranularityType>
class DataflowPass {
public:
    /**
     * @brief The structural element being analyzed (either `BasicBlock` or `Instruction`).
     */
    using Target = std::conditional_t<GranularityType == DataflowGranularity::Block, BasicBlock, Instruction>;
    using Result [[maybe_unused]] = ResultType;

    /**
     * @brief The representation of the dataflow information at a single program point.
     */
    using State = std::unordered_set<Result>;

    static constexpr auto Granularity = GranularityType;
    static constexpr auto Direction = DirectionType;

public:
    virtual ~DataflowPass() = default;

    /**
     * @brief The meet operator: combines dataflow states from multiple predecessors/successors.
     *
     * @param predecessors The collection of states to be merged.
     * @return The resulting merged state.
     */
    [[nodiscard]] virtual State merge(const std::vector<State>& predecessors) = 0;

    /**
     * @brief Provides the initial state for a target element.
     *
     * @param function The function containing the target.
     * @param target The specific block or instruction to initialize.
     * @return The starting `State`.
     */
    [[nodiscard]] virtual State initialize(Function& function, Target& target) = 0;

    /**
     * @brief The transfer function: computes the output state based on the input state and target.
     *
     * @param target The block or instruction being processed.
     * @param state The input dataflow state (from `merge` or previous iteration).
     * @return The new output state.
     */
    [[nodiscard]] virtual State transfer(Target& target, const State& state) = 0;
};

/**
 * @brief Concept that validates if a type is a valid `DataflowPass`.
 */
template <typename T>
concept DataflowPassConcept = requires {
    typename T::Result;
    { T::Direction } -> std::convertible_to<DataflowDirection>;
    { T::Granularity } -> std::convertible_to<DataflowGranularity>;
} && std::is_base_of_v<DataflowPass<typename T::Result, T::Direction, T::Granularity>, T>;

/**
 * @brief The execution engine for dataflow analysis.
 *
 * `DataflowAnalysis` implements the worklist algorithm to compute the fixed-point
 * solution for a given `DataflowPass`. It stores the computed `in` and `out` states
 * for every target in the CFG.
 *
 * @tparam Pass A type satisfying the `DataflowPassConcept`.
 */
template <DataflowPassConcept Pass>
class DataflowAnalysis {
public:
    /**
     * @brief The key used to look up states (pointer to `BasicBlock` or `Instruction`).
     */
    using Key = std::conditional_t<Pass::Granularity == DataflowGranularity::Block, BasicBlock*, Instruction*>;
    using State = std::unordered_set<typename Pass::Result>;

public:
    /**
     * @brief Constructs an analysis instance, passing arguments to the `Pass` constructor.
     *
     * @tparam Args Argument types for the pass.
     * @param args Arguments to instantiate the concrete `Pass`.
     */
    template <typename... Args>
    explicit DataflowAnalysis(Args&&... args) :
        _pass(std::make_unique<Pass>(std::forward<Args>(args)...)) { }

    /**
     * @brief Executes the worklist algorithm on the provided function.
     *
     * This method iterates until the dataflow states reach a fixed point.
     *
     * @param function The function whose CFG will be analyzed.
     */
    void run(Function& function);

    /**
     * @brief Returns the computed 'out' states for all program points.
     *
     * In a forward analysis, this is the state *after* the target.
     * In a backward analysis, this is the state *before* the target.
     *
     * @return A map from the target pointer to `State`.
     */
    [[nodiscard]] auto& out() const { return _out; }

    /**
     * @brief Returns the computed 'in' states for all program points.
     *
     * In a forward analysis, this is the state *before* the target.
     * In a backward analysis, this is the state *after* the target.
     *
     * @return A map from the target pointer to `State`.
     */
    [[nodiscard]] auto& in() const { return _in; }

private:
    std::unordered_map<Key, State> _out{ };
    std::unordered_map<Key, State> _in{ };
    std::unique_ptr<Pass> _pass;
};

#include "../../../src/arkoi_language/il/dataflow.tpp"
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
