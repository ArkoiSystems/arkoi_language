#pragma once

#include <ostream>
#include <unordered_map>
#include <unordered_set>

namespace arkoi::utils {
/**
 * @brief A generic interference graph implementation.
 *
 * This class represents an undirected graph where nodes represent variables (or other entities)
 * and edges represent interferences between them. In the context of register allocation,
 * an edge between two variables means they are live at the same time and thus cannot
 * be assigned to the same register.
 *
 * @tparam Node The type of the nodes in the graph. Must be hashable and comparable.
 */
template <typename Node>
class InterferenceGraph {
public:
    InterferenceGraph() = default;

    /**
     * @brief Adds a node to the interference graph.
     *
     * If the node does not already exist in the graph, it is added
     * with no adjacent (connected) nodes. If the node already exists,
     * this function has no effect.
     *
     * @param node The node to be added to the interference graph.
     * @see remove_node
     */
    void add_node(const Node& node);

    /**
     * @brief Removes a node and all associated edges from the interference graph.
     *
     * If the specified node exists in the graph, this method will remove
     * the node and update the adjacency sets of all other nodes that were
     * connected to it. If the node does not exist, the method does nothing.
     *
     * @param node The node to remove from the interference graph.
     * @see add_node
     */
    void remove_node(const Node& node);

    /**
     * @brief Adds an undirected edge between two nodes in the graph.
     *
     * If the nodes do not already exist in the graph, they will be added automatically
     * using @ref add_node. If the two nodes are the same, no action is taken.
     *
     * The method ensures that the two nodes become adjacent to each other,
     * meaning an undirected edge is created between the provided nodes.
     *
     * @param first The first node of the edge.
     * @param second The second node of the edge.
     * @see is_interfering
     */
    void add_edge(const Node& first, const Node& second);

    /**
     * @brief Checks if two nodes in the interference graph are interfering with each other.
     *
     * This method determines whether there is an interference (edge) between the
     * given nodes `first` and `second` in the graph. Interference is defined as
     * the presence of a direct connection between these nodes within the adjacency
     * structure of the graph.
     *
     * @param first The first node to check for interference.
     * @param second The second node to check for interference.
     * @return True if `first` and `second` are interfering (connected by an edge),
     *         false otherwise.
     * @see add_edge
     */
    [[nodiscard]] bool is_interfering(const Node& first, const Node& second) const;

    /**
     * @brief Retrieves the set of nodes that interfere with the given node in the graph.
     *
     * @param node The node for which to retrieve the interferences.
     * @return An unordered_set containing the nodes that are adjacent (interfere)
     *         with the specified node. If the node is not found in the graph,
     *         returns an empty unordered_set.
     */
    [[nodiscard]] std::unordered_set<Node> interferences(const Node& node) const;

    /**
     * @brief Retrieves all nodes within the interference graph.
     *
     * @return An unordered_set containing all nodes currently stored in the graph.
     */
    [[nodiscard]] std::unordered_set<Node> nodes() const;

    /**
     * @brief Provides access to the underlying adjacency map.
     *
     * @return A reference to the internal adjacency map.
     */
    [[nodiscard]] auto& adjacent() const { return _adjacent; }

private:
    std::unordered_map<Node, std::unordered_set<Node>> _adjacent;
};

template <typename Node>
std::ostream& operator<<(std::ostream& os, const InterferenceGraph<Node>& graph);
} // namespace arkoi::utils

#include "../../../src/arkoi_language/utils/interference_graph.tpp"

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
