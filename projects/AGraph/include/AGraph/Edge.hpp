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

#ifndef AGRAPH_EDGE_HPP
#define AGRAPH_EDGE_HPP

#include "AGraphModule.hpp"
#include "Common.hpp"
#include "Element.hpp"

namespace agraph
{
template<typename GraphType, typename GraphImpl>
class Node;

//! The Edge<GraphType, typename GraphImpl> is a
//! template that represents an edge of a graph in
//! a given \c GraphType.
//!
//! The edge (also known as arc, line) is an element
//! of a graph that connects two nodes. The Edge class
//! provides methods to access the origin and the
//! destination nodes.
//!
//! The basic functionality is provided by the
//! #agraph::Element base class.
template<typename GraphType, typename GraphImpl>
class Edge : public Element<Edge<GraphType, GraphImpl>, GraphType>
{
public:
    //! Default constructor.
    constexpr Edge() noexcept = default;

    //! Returns the #agraph::Node representing the
    //! origin node of this edge. The edge must
    //! be valid.
    [[nodiscard]] constexpr auto from() const -> Node<GraphType, GraphImpl>
    {
        return this->graph()->node(this->graph()->mData.edge(edgeToIndex(this->index())).from);
    }

    //! Returns the #agraph::Node representing the
    //! destination node of this edge. The edge must
    //! be valid.
    [[nodiscard]] constexpr auto to() const -> Node<GraphType, GraphImpl>
    {
        return this->graph()->node(this->graph()->mData.edge(edgeToIndex(this->index())).to);
    }

protected:
    //! Concstructs the edge with the \a index and
    //! the \a graph.
    constexpr Edge(acore::size_type index, const GraphType *graph) noexcept :
        Element<Edge<GraphType, GraphImpl>, GraphType>{index, graph}
    {
    }

private:
    friend GraphImpl;
};
}

#endif
