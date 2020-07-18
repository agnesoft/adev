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

#ifndef AGRAPH_NODE_HPP
#define AGRAPH_NODE_HPP

#include "AGraphModule.hpp"
#include "Common.hpp"
#include "Element.hpp"

namespace agraph
{
template<typename GraphType, typename GraphImpl>
class Edge;

//! The Node<GraphType, GraphImpl> is a template
//! that represents a node of a graph in given
//! \c GraphType.
//!
//! The node (also known as vertex, point) is an
//! element of a graph. The Node class provides methods
//! to access outgoing and incoming edges and to
//! iterate over them.
//!
//! The basic functionality is provided by the
//! #agraph::Element base class.
template<typename GraphType, typename GraphImpl>
class Node : public Element<Node<GraphType, GraphImpl>, GraphType>
{
public:
    //! Immutable iterator over the node's outgoing
    //! and incoming edges.
    using const_iterator = acore::ForwardIterator<Edge<GraphType, GraphImpl>, Edge<GraphType, GraphImpl>, const Node>;

    //! Default constructor.
    constexpr Node() = default;

    //! Returns the #const_iterator pointing to the
    //! first outgoing edge from this node. It does not
    //! necessarily need to be the first edge created
    //! from this node. If there are no outgoing
    //! edges end() is returned. The node itself
    //! must be valid.
    [[nodiscard]] constexpr auto begin() const -> const_iterator
    {
        const acore::size_type edge = this->graph()->mData.node(this->index()).from;

        if (edge < acore::INVALID_INDEX)
        {
            return const_iterator{edge, this};
        }

        return end();
    }

    //! Returns the combined number of outgoing and
    //! incoming connections from/to this node. This
    //! number may not represent the number of actual
    //! edges if there are circular connections (edges
    //! connecting the node to itself). The node must
    //! be valid.
    [[nodiscard]] constexpr auto edgeCount() const -> acore::size_type
    {
        return fromCount() + toCount();
    }

    //! Returns the #const_iterator pointing to the
    //! invalid edge.
    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator
    {
        return const_iterator{acore::INVALID_INDEX, this};
    }

    //! Returns the number of edges outgoing from
    //! this node. The node must be valid.
    [[nodiscard]] constexpr auto fromCount() const -> acore::size_type
    {
        return this->graph()->mData.node(this->index()).fromCount;
    }

    //! Returns the #const_iterator pointing to the
    //! first incoming edge from this node. It does
    //! not necessarily need to be the first edge
    //! created to this node. If there are no incoming
    //! edges end() is returned. The node must be
    //! valid.
    [[nodiscard]] constexpr auto rbegin() const -> const_iterator
    {
        const acore::size_type edge = this->graph()->mData.node(this->index()).to;

        if (edge < acore::INVALID_INDEX)
        {
            return const_iterator{edgeToIndex(edge), this};
        }

        return rend();
    }

    //! Returns the #const_iterator pointing to the
    //! invalid edge.
    [[nodiscard]] constexpr auto rend() const -> const_iterator
    {
        return end();
    }

    //! Returns the number of edges incoming to
    //! this node. The node must be valid.
    [[nodiscard]] constexpr auto toCount() const -> acore::size_type
    {
        return this->graph()->mData.node(this->index()).toCount;
    }

protected:
    //! Constructs a node with the \a index and
    //! the \a graph.
    constexpr Node(acore::size_type index, const GraphType *graph) noexcept :
        Element<Node<GraphType, GraphImpl>, GraphType>{index, graph}
    {
    }

private:
    friend const_iterator;
    friend GraphImpl;

    //! \private
    //! NEEDED BECAUSE OF THE BUG IN DOXYGEN 1.8.13
    [[nodiscard]] constexpr auto nextIndex(acore::size_type index) const -> acore::size_type
    {
        if (index < acore::INVALID_INDEX)
        {
            return this->graph()->mData.edge(indexToEdge(index)).nextFrom;
        }

        if (acore::INVALID_INDEX < index)
        {
            return edgeToIndex(this->graph()->mData.edge(index).nextTo);
        }

        throw acore::Exception{} << "Cannot incrementing invalid 'agraph::Node::iterator'.";
    }

    [[nodiscard]] constexpr auto referenceAt(acore::size_type index) const -> Edge<GraphType, GraphImpl>
    {
        if (acore::INVALID_INDEX < index)
        {
            return this->graph()->edge(edgeToIndex(index));
        }

        if (index < acore::INVALID_INDEX)
        {
            return this->graph()->edge(index);
        }

        throw acore::Exception{} << "Dereferencing invalid 'agraph::Node::iterator'.";
    }
};
}

#endif
