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

#ifndef AGRAPH_DEPTHFIRSTSEARCH_HPP
#define AGRAPH_DEPTHFIRSTSEARCH_HPP

#include "AGraphModule.hpp"
#include "GraphSearch.hpp"

#include <vector>

namespace agraph
{
//! The DepthFirstSearch<GraphType> is a template
//! graph search algorithm implementing depth first
//! search.
//!
//! The depth-first search (DFS) is an algorithm for
//! traversing or searching a graph. It starts at a
//! node on the graph and explores as far as possible
//! along each branch before backtracking.
//!
//! For example consider a simple graph:
//!
//! \snippet DepthFirstSearchTest.cpp [Graph]
//!
//! Starting the search from the \c node1 the DFS
//! would search the elements of the graph in the
//! following order:
//!
//! \snippet DepthFirstSearchTest.cpp [Usage]
//!
//! Several aspects of this manner of search should be
//! noted:
//!
//! The branches are explored in the same order in
//! that they were inserted into the graph from the
//! oldest to the most recent. In other words the
//! edges are processed in the opposite order than
//! is the order in which they are presented when
//! traversing the graph manually (that is from the
//! most recent).
//!
//! Both edges and nodes constitute a depth level. The
//! AGraph graph implementation based on GraphBase
//! gives edges the same significance as to nodes.
//! Neither edges or nodes are visited more than once.
//! The algorithm can handle cyclic graphs.
template<typename GraphType>
class DepthFirstSearch : public GraphSearch<GraphType, DepthFirstSearch<GraphType>>
{
public:
    using GraphSearch<GraphType, DepthFirstSearch>::GraphSearch;

private:
    using Base = GraphSearch<GraphType, DepthFirstSearch>;
    friend Base;

    constexpr DepthFirstSearch() = default;

    constexpr auto processStack(std::vector<typename Base::Index> *stack) -> void
    {
        const auto index = stack->back();
        stack->pop_back();
        handleIndex(index);
    }
};
}

#endif
