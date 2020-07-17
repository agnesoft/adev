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

#include "Common.hpp"
#include "Edge.hpp"
#include "Node.hpp"

namespace agraph
{
template<typename Data, typename GraphType>
class GraphBase
{
public:
    using Edge = Edge<GraphType, GraphBase>;
    using Node = Node<GraphType, GraphBase>;
    using const_iterator = acore::ForwardIterator<Node, Node, const GraphBase>;

    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator
    {
        return ++end();
    }

    [[nodiscard]] constexpr auto contains(const Edge &edge) const -> bool
    {
        return edge.graph() == this
            && edge.index() < acore::INVALID_INDEX
            && mData.edge(edgeToIndex(edge.index())).to > acore::INVALID_INDEX;
    }

    [[nodiscard]] constexpr auto contains(const Node &node) const -> bool
    {
        return node.graph() == this
            && node.index() > acore::INVALID_INDEX
            && mData.node(node.index()).fromCount > acore::INVALID_INDEX;
    }

    constexpr auto insertEdge(const Node &from, const Node &to) -> Edge
    {
        validateNodes(from, to);
        const acore::size_type edge = getFreeEdge();
        updateElementsWithNewEdge(from, to, edge);
        return Edge{edge, static_cast<const GraphType *>(this)};
    }

    constexpr auto insertNode() -> Node
    {
        const acore::size_type index = getFreeNode();
        mData.setNode(index, {acore::INVALID_INDEX, 0, acore::INVALID_INDEX, 0});
        mData.setNodeCount(mData.nodeCount() + 1);
        return Node{index, static_cast<const GraphType *>(this)};
    }

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

    [[nodiscard]] constexpr auto edgeCount() const -> acore::size_type
    {
        return mData.edgeCount();
    }

    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator
    {
        return const_iterator{acore::INVALID_INDEX, this};
    }

    [[nodiscard]] constexpr auto nextEdgeIndex() const noexcept -> acore::size_type
    {
        if (mData.freeEdge() == acore::INVALID_INDEX)
        {
            return indexToEdge(mData.edgeCapacity());
        }

        return mData.freeEdge();
    }

    [[nodiscard]] constexpr auto nextNodeIndex() const noexcept -> acore::size_type
    {
        if (mData.freeNode() == acore::INVALID_INDEX)
        {
            return mData.nodeCapacity();
        }

        return mData.freeNode();
    }

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

    [[nodiscard]] constexpr auto nodeCount() const -> acore::size_type
    {
        return mData.nodeCount();
    }

    constexpr auto removeEdge(const Edge &edge) -> void
    {
        if (contains(edge))
        {
            doRemoveEdge(edge.index());
        }
    }

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

    [[nodiscard]] constexpr auto storage() noexcept -> Data *
    {
        return &mData;
    }

    [[nodiscard]] constexpr auto storage() const noexcept -> const Data *
    {
        return &mData;
    }

protected:
    template<typename... Args>
    explicit constexpr GraphBase(Args... args) :
        mData(args...)
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

    constexpr auto addEdge(acore::size_type edge, acore::size_type from, acore::size_type nextFrom, acore::size_type to, acore::size_type nextTo) -> void
    {
        updateEdge(edge, from, nextFrom, to, nextTo);
        mData.setEdgeCount(mData.edgeCount() + 1);
    }

    constexpr auto doRemoveEdge(acore::size_type edge) -> void
    {
        EdgeData edgeData = mData.edge(edgeToIndex(edge));
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

    template<typename NodeDataType>
    constexpr auto saveNodeData(acore::size_type from, NodeDataType *fromData, acore::size_type to, NodeDataType *toData) -> void
    {
        if (from == to)
        {
            mData.setNode(from, {fromData->from, fromData->fromCount, toData->to, toData->toCount});
        }
        else
        {
            mData.setNode(from, *fromData);
            mData.setNode(to, *toData);
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
        updateEdge(edge, freeIndex, acore::INVALID_INDEX, acore::INVALID_INDEX, acore::INVALID_INDEX);
        mData.setEdgeCount(mData.edgeCount() - 1);
        mData.setFreeEdge(edge);
    }

    constexpr auto updateRemovedNodeData(acore::size_type node, acore::size_type freeIndex) -> void
    {
        updateNode(node, freeIndex, acore::INVALID_INDEX, acore::INVALID_INDEX, acore::INVALID_INDEX);
        mData.setNodeCount(mData.nodeCount() - 1);
        mData.setFreeNode(node);
    }

    constexpr auto updateEdge(acore::size_type edge, acore::size_type from, acore::size_type nextFrom, acore::size_type to, acore::size_type nextTo) -> void
    {
        mData.setEdge(edgeToIndex(edge), {from, nextFrom, to, nextTo});
    }

    constexpr auto updateElementsWithNewEdge(const Node &from, const Node &to, acore::size_type edge) -> void
    {
        NodeData fromData = mData.node(from.index());
        NodeData toData = mData.node(to.index());
        addEdge(edge, from.index(), fromData.from, to.index(), toData.to);
        updateNodesWithNewEdge(from.index(), &fromData, to.index(), &toData, edge);
    }

    template<typename NodeDataType, typename EdgeDataType>
    constexpr auto updateElementsWithRemovedEdge(acore::size_type edge, EdgeDataType edgeData, NodeDataType fromData, NodeDataType toData) -> void
    {
        updateFromWithRemovedEdge(edge, edgeData.nextFrom, &fromData);
        updateToWithRemovedEdge(edge, edgeData.nextTo, &toData);
        saveNodeData(edgeData.from, &fromData, edgeData.to, &toData);
    }

    template<typename NodeDataType>
    constexpr auto updateFromWithRemovedEdge(acore::size_type edge, acore::size_type nextFrom, NodeDataType *fromData) -> void
    {
        if (fromData->from != edge)
        {
            setPreviousFrom(edge, fromData->from, nextFrom);
        }
        else
        {
            fromData->from = nextFrom;
        }

        fromData->fromCount--;
    }

    constexpr auto updateNode(acore::size_type node, acore::size_type from, acore::size_type fromCount, acore::size_type to, acore::size_type toCount) -> void
    {
        mData.setNode(node, {from, fromCount, to, toCount});
    }

    template<typename NodeDataType>
    constexpr auto updateNodeData(NodeDataType *fromData, NodeDataType *toData, acore::size_type edge) -> void
    {
        fromData->from = edge;
        fromData->fromCount++;
        toData->to = edge;
        toData->toCount++;
    }

    template<typename NodeDataType>
    constexpr auto updateNodesWithNewEdge(acore::size_type from, NodeDataType *fromData, acore::size_type to, NodeDataType *toData, acore::size_type edge) -> void
    {
        updateNodeData(fromData, toData, edge);
        saveNodeData(from, fromData, to, toData);
    }

    template<typename NodeDataType>
    constexpr auto updateToWithRemovedEdge(acore::size_type edge, acore::size_type nextTo, NodeDataType *toData) -> void
    {
        if (toData->to != edge)
        {
            setPreviousTo(edge, toData->to, nextTo);
        }
        else
        {
            toData->to = nextTo;
        }

        toData->toCount--;
    }

    constexpr auto validateNodes(const Node &from, const Node &to) const -> void
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
