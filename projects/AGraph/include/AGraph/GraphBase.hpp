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

#ifndef AGRAPH_GRAPHBASE_HPP
#define AGRAPH_GRAPHBASE_HPP

#include "AGraphModule.hpp"
#include "Common.hpp"
#include "Edge.hpp"
#include "Node.hpp"

namespace agraph
{
//! The GraphBase<Data, GraphType> provides
//! directional graph implementation.
//!
//! The graph is a kind of data structure that stores
//! nodes (sometimes called vertices, points) that are
//! connected with edges (sometimes called arcs, lines).
//!
//! The GraphBase needs a storage class to be passsed
//! as the template argument. The #Node
//! and the #Edge classes act like indexes/iterators
//! for the graph itself. They need to be created
//! from the graph either with insertNode() and
//! insertEdge() methods or with node() and edge()
//! methods passing the index of a graph element.
//!
//! While the graph checks the validity of the passed
//! in indexes the operations on the GraphBase::Node
//! and #Edge are not subsequently checked
//! for validity of the element. Call #agraph::Element::isValid() if
//! the element might not be actually valid.
//!
//! The graph can be iterated in a convenient manner
//! using the ranged based for loop on the graph itself
//! to iterate over nodes and on each #Node
//! to iterate over its outgoing edges.
//!
//! The nodes and edges can be removed from the graph.
//! When nodes are removed both outgoing and incoming
//! edges will also be removed possibly invalidating
//! any #Edge objects previously created
//! for those edges.
//!
//! Each element of the graph has an index (unique
//! for nodes and edges separately). It can be used
//! to query the respective objects representing a
//! node or an edge and or to associate external data
//! with these elements.
//!
//! The key properties of this graph implementation are:
//!
//! \b Directed
//!
//! The GraphBase is a directed graph meaning the
//! edges do have a sense of origin and destination.
//! The implementation guarantees that walking through
//! the graph is as efficient in both directions.
//!
//! \b Sparse
//!
//! The GraphBase is a sparse graph that does not
//! require connections between all nodes. It is
//! implemented as the adjacency list.
//!
//! \b Incomplete
//!
//! The GraphBase can represent incomplete graphs
//! and multiple unconnected sub-graphs. It does not
//! require there to be a path that connects all the
//! node.
//!
//! \b Circular
//!
//! The circular connections - node being connected by
//! an edge to itself - are allowed.
//!
//! \b Constant Algorithmic Complexity
//!
//! The GraphBase is implemented so that operations:
//!
//! - inserting a node
//! - inserting an edge
//! - removing an edge
//!
//! are constant time. Removing nodes have linear
//! algorithmic complexity proportional to the number
//! of edges coming from/to that node. Each edge removal
//! in such a case has constant complexity. As such
//! the implementation is infinitely scalable without
//! performance degradation and is limited only by
//! the storage capacity.
template<typename Data, typename GraphType>
class GraphBase
{
public:
    //! The type representing the graph's edge.
    using Edge = Edge<GraphType, GraphBase>;

    //! The type representing teh graph's node.
    using Node = Node<GraphType, GraphBase>;

    //! Immutable iterator over the graph's nodes.
    using const_iterator = acore::ForwardIterator<Node, Node, const GraphBase>;

    //! Returns an const_iterator pointing to the
    //! first node in the graph based on its index
    //! (not necessarily the first created). If the
    //! graph is empty end() is returned.
    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator
    {
        return ++end();
    }

    //! Return \c true if the \a edge is part of
    //! the graph.
    [[nodiscard]] constexpr auto contains(const Edge &edge) const -> bool
    {
        return edge.graph() == this
            && edge.index() < acore::INVALID_INDEX
            && mData.edge(edgeToIndex(edge.index())).to > acore::INVALID_INDEX;
    }

    //! Return \c true if the \a node is part of
    //! the graph.
    [[nodiscard]] constexpr auto contains(const Node &node) const -> bool
    {
        return node.graph() == this
            && node.index() > acore::INVALID_INDEX
            && mData.node(node.index()).fromCount > acore::INVALID_INDEX;
    }

    //! Inserts an edge between the \a from and the
    //! \a to nodes. Both nodes must be part of the
    //! graph otherwise an exception of type #acore::Exception
    //! is thrown. The index assigned might be that
    //! of a previously removed edge. Returns the
    //! object of type #Edge representing
    //! the newly created edge.
    constexpr auto insertEdge(const Node &from, const Node &to) -> Edge
    {
        validateNodes(from, to);
        const acore::size_type edge = getFreeEdge();
        updateElementsWithNewEdge(from, to, edge);
        return Edge{edge, static_cast<const GraphType *>(this)};
    }

    //! Inserts a new node in the graph. The index
    //! assigned might be that of a previously removed
    //! node. Returns the object of type #Node
    //! representing the newly created node.
    constexpr auto insertNode() -> Node
    {
        const acore::size_type index = getFreeNode();
        mData.setNode(index, {acore::INVALID_INDEX, 0, acore::INVALID_INDEX, 0});
        mData.setNodeCount(mData.nodeCount() + 1);
        return Node{index, static_cast<const GraphType *>(this)};
    }

    //! Returns the #Edge representing
    //! the edge at the \a index if the \a index is
    //! part of the graph. Otherwise returns a default
    //! constructed (invalid) edge object.
    [[nodiscard]] constexpr auto edge(acore::size_type index) const -> Edge
    {
        const acore::size_type realIndex = edgeToIndex(index);

        if (index < acore::INVALID_INDEX
            && realIndex < mData.edgeCapacity()
            && mData.edge(realIndex).to > acore::INVALID_INDEX)
        {
            return Edge{index, static_cast<const GraphType *>(this)};
        }

        return Edge{acore::INVALID_INDEX, static_cast<const GraphType *>(this)};
    }

    //! Returns the total number of edges in the graph.
    [[nodiscard]] constexpr auto edgeCount() const -> acore::size_type
    {
        return mData.edgeCount();
    }

    //! Returns the invalid iterator serving as the
    //! end of the range of the graph's nodes.
    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator
    {
        return const_iterator{acore::INVALID_INDEX, this};
    }

    //! Returns the next available edge id that will
    //! be used on the next call to insertEdge().
    [[nodiscard]] constexpr auto nextEdgeIndex() const noexcept -> acore::size_type
    {
        if (mData.freeEdge() == acore::INVALID_INDEX)
        {
            return indexToEdge(mData.edgeCapacity());
        }

        return mData.freeEdge();
    }

    //! Returns the next available node id that will
    //! be used on the next call to insertNode().
    [[nodiscard]] constexpr auto nextNodeIndex() const noexcept -> acore::size_type
    {
        if (mData.freeNode() == acore::INVALID_INDEX)
        {
            return mData.nodeCapacity();
        }

        return mData.freeNode();
    }

    //! Returns the #Node representing
    //! the node at the \a index if the \a index is
    //! part of the graph. Otherwise returns a default
    //! constructed (invalid) node object.
    [[nodiscard]] constexpr auto node(acore::size_type index) const -> Node
    {
        if (index > acore::INVALID_INDEX
            && index < mData.nodeCapacity()
            && mData.node(index).fromCount > acore::INVALID_INDEX)
        {
            return Node{index, static_cast<const GraphType *>(this)};
        }

        return Node{acore::INVALID_INDEX, static_cast<const GraphType *>(this)};
    }

    //! Returns the total number of nodes in the graph.
    [[nodiscard]] constexpr auto nodeCount() const -> acore::size_type
    {
        return mData.nodeCount();
    }

    //! Removes the \a edge from the graph if it is
    //! part of the graph otherwise does nothing.
    constexpr auto removeEdge(const Edge &edge) -> void
    {
        if (contains(edge))
        {
            doRemoveEdge(edge.index());
        }
    }

    //! Removes the \a node from the graph if it is
    //! part of the graph otherwise does nothing.
    //! If the node has any outgoing and/or incoming
    //! edges they will be removed from the graph
    //! as well.
    //!
    //! \note Removing nodes might invalidate #Edge
    //! objects represnting the edges connected to the removed
    //! node.
    constexpr auto removeNode(const Node &node) -> void
    {
        if (contains(node))
        {
            acore::size_type edge = mData.node(node.index()).to;

            while (edge < acore::INVALID_INDEX)
            {
                doRemoveEdge(edge);
                edge = mData.node(node.index()).to;
            }

            edge = mData.node(node.index()).from;

            while (edge < acore::INVALID_INDEX)
            {
                doRemoveEdge(edge);
                edge = mData.node(node.index()).from;
            }

            updateRemovedNodeData(node.index(), mData.freeNode());
        }
    }

    //! Returns modifiable \c Data * pointing to the instance
    //! of \c Data template argument used by the GraphBase
    //! as the internal storage.
    [[nodiscard]] constexpr auto storage() noexcept -> Data *
    {
        return &mData;
    }

    //! Returns \c const Data * pointing to the instance
    //! of \c Data template argument used by the GraphBase
    //! as internal storage.
    [[nodiscard]] constexpr auto storage() const noexcept -> const Data *
    {
        return &mData;
    }

protected:
    //! Constructs the graph passing the expanded
    //! \a args to the constructor of the \c Data
    //! template argument.
    template<typename... Args>
    explicit constexpr GraphBase(Args... args) :
        mData{args...}
    {
    }

private:
    friend Edge;
    friend Node;
    friend const_iterator;

    [[nodiscard]] constexpr auto nextIndex(acore::size_type index) const -> acore::size_type
    {
        const acore::size_type count = mData.nodeCapacity();

        for (acore::size_type next = index + 1; next < count; next++)
        {
            if (mData.node(next).fromCount > acore::INVALID_INDEX)
            {
                return next;
            }
        }

        return acore::INVALID_INDEX;
    }

    [[nodiscard]] constexpr auto referenceAt(acore::size_type index) const -> Node
    {
        return Node{index, static_cast<const GraphType *>(this)};
    }

    constexpr auto addEdge(acore::size_type edge, EdgeData data) -> void
    {
        updateEdge(edge, data);
        mData.setEdgeCount(mData.edgeCount() + 1);
    }

    constexpr auto doRemoveEdge(acore::size_type edge) -> void
    {
        const EdgeData edgeData = mData.edge(edgeToIndex(edge));
        updateElementsWithRemovedEdge(edge, edgeData, mData.node(edgeData.from), mData.node(edgeData.to));
        updateRemovedEdgeData(edge, mData.freeEdge());
    }

    [[nodiscard]] constexpr auto getFreeEdge() -> acore::size_type
    {
        acore::size_type edge = 0;

        if (mData.freeEdge() == acore::INVALID_INDEX)
        {
            edge = indexToEdge(mData.edgeCapacity());
            mData.setEdgeCapacity(mData.edgeCapacity() + 1);
        }
        else
        {
            edge = mData.freeEdge();
            mData.setFreeEdge(mData.edge(edgeToIndex(edge)).from);
        }

        return edge;
    }

    [[nodiscard]] constexpr auto getFreeNode() -> acore::size_type
    {
        acore::size_type index = 0;

        if (mData.freeNode() == acore::INVALID_INDEX)
        {
            index = mData.nodeCapacity();
            mData.setNodeCapacity(index + 1);
        }
        else
        {
            index = mData.freeNode();
            mData.setFreeNode(mData.node(index).from);
        }

        return index;
    }

    constexpr auto saveNodeData(acore::size_type from, NodeData fromData, acore::size_type to, NodeData toData) -> void
    {
        if (from == to)
        {
            mData.setNode(from, NodeData{fromData.from, fromData.fromCount, toData.to, toData.toCount});
        }
        else
        {
            mData.setNode(from, fromData);
            mData.setNode(to, toData);
        }
    }

    constexpr auto setPreviousFrom(acore::size_type edge, acore::size_type previous, acore::size_type next) -> void
    {
        EdgeData previousEdgeData = mData.edge(edgeToIndex(previous));

        while (previousEdgeData.nextFrom != edge)
        {
            previous = previousEdgeData.nextFrom;
            previousEdgeData = mData.edge(edgeToIndex(previous));
        }

        previousEdgeData.nextFrom = next;
        mData.setEdge(edgeToIndex(previous), previousEdgeData);
    }

    constexpr auto setPreviousTo(acore::size_type edge, acore::size_type previous, acore::size_type next) -> void
    {
        EdgeData previousEdgeData = mData.edge(edgeToIndex(previous));

        while (previousEdgeData.nextTo != edge)
        {
            previous = previousEdgeData.nextTo;
            previousEdgeData = mData.edge(edgeToIndex(previous));
        }

        previousEdgeData.nextTo = next;
        mData.setEdge(edgeToIndex(previous), previousEdgeData);
    }

    constexpr auto updateRemovedEdgeData(acore::size_type edge, acore::size_type freeIndex) -> void
    {
        updateEdge(edge, EdgeData{freeIndex, acore::INVALID_INDEX, acore::INVALID_INDEX, acore::INVALID_INDEX});
        mData.setEdgeCount(mData.edgeCount() - 1);
        mData.setFreeEdge(edge);
    }

    constexpr auto updateRemovedNodeData(acore::size_type node, acore::size_type freeIndex) -> void
    {
        updateNode(node, NodeData{freeIndex, acore::INVALID_INDEX, acore::INVALID_INDEX, acore::INVALID_INDEX});
        mData.setNodeCount(mData.nodeCount() - 1);
        mData.setFreeNode(node);
    }

    constexpr auto updateEdge(acore::size_type edge, EdgeData data) -> void
    {
        mData.setEdge(edgeToIndex(edge), data);
    }

    constexpr auto updateElementsWithNewEdge(Node from, Node to, acore::size_type edge) -> void
    {
        NodeData fromData = mData.node(from.index());
        NodeData toData = mData.node(to.index());
        addEdge(edge, EdgeData{from.index(), fromData.from, to.index(), toData.to});
        updateNodesWithNewEdge(from.index(), fromData, to.index(), toData, edge);
    }

    constexpr auto updateElementsWithRemovedEdge(acore::size_type edge, EdgeData edgeData, NodeData fromData, NodeData toData) -> void
    {
        saveNodeData(edgeData.from,
                     updateFromWithRemovedEdge(edge, edgeData.nextFrom, fromData),
                     edgeData.to,
                     updateToWithRemovedEdge(edge, edgeData.nextTo, toData));
    }

    [[nodiscard]] constexpr auto updateFromWithRemovedEdge(acore::size_type edge, acore::size_type nextFrom, NodeData fromData) -> NodeData
    {
        if (fromData.from != edge)
        {
            setPreviousFrom(edge, fromData.from, nextFrom);
        }
        else
        {
            fromData.from = nextFrom;
        }

        --fromData.fromCount;
        return fromData;
    }

    constexpr auto updateNode(acore::size_type node, NodeData data) -> void
    {
        mData.setNode(node, data);
    }

    constexpr auto updateNodeData(NodeData *fromData, NodeData *toData, acore::size_type edge) -> void
    {
        fromData->from = edge;
        ++fromData->fromCount;
        toData->to = edge;
        ++toData->toCount;
    }

    constexpr auto updateNodesWithNewEdge(acore::size_type from, NodeData fromData, acore::size_type to, NodeData toData, acore::size_type edge) -> void
    {
        updateNodeData(&fromData, &toData, edge);
        saveNodeData(from, fromData, to, toData);
    }

    [[nodiscard]] constexpr auto updateToWithRemovedEdge(acore::size_type edge, acore::size_type nextTo, NodeData toData) -> NodeData
    {
        if (toData.to != edge)
        {
            setPreviousTo(edge, toData.to, nextTo);
        }
        else
        {
            toData.to = nextTo;
        }

        --toData.toCount;
        return toData;
    }

    constexpr auto validateNodes(Node from, Node to) const -> void
    {
        if (!contains(from))
        {
            throw acore::Exception{} << "Cannot create edge between nodes (" << from.index() << " and " << to.index() << "). Origin node (" << from.index() << ") is not part of the graph.";
        }

        if (!contains(to))
        {
            throw acore::Exception{} << "Cannot create edge between nodes (" << from.index() << " and " << to.index() << "). Desination node (" << to.index() << ") is not part of the graph.";
        }
    }

    Data mData;
};
}

#endif
