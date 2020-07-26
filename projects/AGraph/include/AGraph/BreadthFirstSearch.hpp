// Copyright 2020 Michael Vlach
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef AGRAPH_BREADTHFIRSTSEARCH_HPP
#define AGRAPH_BREADTHFIRSTSEARCH_HPP

#include "AGraphModule.hpp"
#include "GraphSearch.hpp"

#include <vector>

namespace agraph
{
//! The BreadthFirstSearch<GraphType>
//! is a template graph search algorithm implementing
//! breadth first search.
//!
//! The breadth-first search (BFS) is an algorithm for
//! traversing (searching) a graph. It starts at a
//! node on the graph and explores all of the outgoing
//! edges going to the neighbor nodes. Then it explores
//! all the destination nodes of the edges. The
//! algorithm always examines all of the elements
//! (edges  and nodes) at the present depth prior
//! to moving on to the edges or nodes at the next
//! depth level.
//!
//! For example consider a simple graph:
//!
//! \snippet BreadthFirstSearchTest.cpp [Graph]
//!
//! Starting the search from the \c node1 the BFS
//! would search the elements of the graph in the
//! following order:
//!
//! \snippet BreadthFirstSearchTest.cpp [Usage]
//!
//! Several aspects of this manner of search should be
//! noted:
//!
//! All elements at the given depth are explored
//! regardless of them being connected to different
//! elements in the previous depth (notice how edges
//! are explored at the same depth level even though
//! they originate in different nodes in the previous
//! depth level).
//!
//! Both edges and nodes constitute a depth level. The
//! AGraph graph implementation based on GraphBase
//! gives edges the same significance as to nodes.
//! Neither edges or nodes are visited more than once.
//! The algorithm can handle cyclic graphs.
template<typename GraphType>
class BreadthFirstSearch : public GraphSearch<GraphType, BreadthFirstSearch<GraphType>>
{
public:
    using GraphSearch<GraphType, BreadthFirstSearch>::GraphSearch;

private:
    using Base = GraphSearch<GraphType, BreadthFirstSearch>;
    friend Base;

    constexpr BreadthFirstSearch() = default;

    constexpr auto processStack(std::vector<typename Base::Index> *stack) -> void
    {
        for (auto index : takeStack(stack))
        {
            this->handleIndex(index);
        }
    }

    [[nodiscard]] auto takeStack(std::vector<typename Base::Index> *stack) -> std::vector<typename Base::Index>
    {
        std::vector<typename GraphSearch<GraphType, BreadthFirstSearch>::Index> currentStack;
        currentStack.swap(*stack);
        return currentStack;
    }
};
}

#endif
