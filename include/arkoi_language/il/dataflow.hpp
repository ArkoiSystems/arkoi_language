#pragma once

#include <concepts>

#include "arkoi_language/il/cfg.hpp"
#include "arkoi_language/il/instruction.hpp"

namespace arkoi::il {
/**
 * @brief Enumeration of dataflow analysis directions
 */
enum class DataflowDirection {
    Forward,
    Backward
};

/**
 * @brief Enumeration of dataflow analysis granularities
 */
enum class DataflowGranularity {
    Block,
    Instruction
};

/**
 * @brief Base class for dataflow analysis passes
 *
 * @tparam ResultType The type of data being analyzed (e.g., Operand)
 * @tparam DirectionType The direction of the analysis
 * @tparam GranularityType The granularity of the analysis
 */
template <typename ResultType, DataflowDirection DirectionType, DataflowGranularity GranularityType>
class DataflowPass {
public:
    using Target = std::conditional_t<GranularityType == DataflowGranularity::Block, BasicBlock, Instruction>;
    using Result [[maybe_unused]] = ResultType;
    using State = std::unordered_set<Result>;

    static constexpr auto Granularity = GranularityType;
    static constexpr auto Direction = DirectionType;

public:
    virtual ~DataflowPass() = default;

    /**
     * @brief Merges multiple states into one
     *
     * @param predecessors The states to merge
     *
     * @return The merged state
     */
    [[nodiscard]] virtual State merge(const std::vector<State>& predecessors) = 0;

    /**
     * @brief Initializes the analysis state for a target
     *
     * @param function The function being analyzed
     * @param target The target (block or instruction)
     *
     * @return The initial state
     */
    [[nodiscard]] virtual State initialize(Function& function, Target& target) = 0;

    /**
     * @brief Applies the transfer function to a target and its current state
     *
     * @param target The target (block or instruction)
     * @param state The current state
     *
     * @return The new state
     */
    [[nodiscard]] virtual State transfer(Target& target, const State& state) = 0;
};

/**
 * @brief Concept for a dataflow analysis pass
 */
template <typename T>
concept DataflowPassConcept = requires {
    typename T::Result;
    { T::Direction } -> std::convertible_to<DataflowDirection>;
    { T::Granularity } -> std::convertible_to<DataflowGranularity>;
} && std::is_base_of_v<DataflowPass<typename T::Result, T::Direction, T::Granularity>, T>;

/**
 * @brief Engine for running dataflow analyses
 *
 * @tparam Pass The dataflow pass to run
 */
template <DataflowPassConcept Pass>
class DataflowAnalysis {
public:
    using Key = std::conditional_t<Pass::Granularity == DataflowGranularity::Block, BasicBlock*, Instruction*>;
    using State = std::unordered_set<typename Pass::Result>;

public:
    /**
     * @brief Constructs a DataflowAnalysis with pass arguments
     *
     * @tparam Args The types of arguments for the pass constructor
     * @param args The arguments for the pass constructor
     */
    template <typename... Args>
    explicit DataflowAnalysis(Args&&... args) :
        _pass(std::make_unique<Pass>(std::forward<Args>(args)...)) { }

    /**
     * @brief Runs the dataflow analysis on a function
     *
     * @param function The function to analyze
     */
    void run(Function& function);

    /**
     * @brief Returns the output states for all targets
     *
     * @return A reference to the map of output states
     */
    [[nodiscard]] auto& out() const { return _out; }

    /**
     * @brief Returns the input states for all targets
     *
     * @return A reference to the map of input states
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
