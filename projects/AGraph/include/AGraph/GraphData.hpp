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

#ifndef AGRAPH_GRAPHDATA_HPP
#define AGRAPH_GRAPHDATA_HPP

#include "Common.hpp"

namespace agraph
{
class GraphData
{
public:
    [[nodiscard]] auto edge(acore::size_type index) const -> EdgeData
    {
        return mEdges[index];
    }

    [[nodiscard]] auto edgeCapacity() const -> acore::size_type
    {
        return mEdges.size();
    }

    [[nodiscard]] constexpr auto edgeCount() const noexcept -> acore::size_type
    {
        return mEdgeCount;
    }

    [[nodiscard]] constexpr auto freeEdge() const noexcept -> acore::size_type
    {
        return mFreeEdge;
    }

    [[nodiscard]] constexpr auto freeNode() const noexcept -> acore::size_type
    {
        return mFreeNode;
    }

    [[nodiscard]] auto node(acore::size_type index) const -> NodeData
    {
        return mNodes[index];
    }

    [[nodiscard]] auto nodeCapacity() const noexcept -> acore::size_type
    {
        return mNodes.size();
    }

    [[nodiscard]] constexpr auto nodeCount() const noexcept -> acore::size_type
    {
        return mNodeCount;
    }

    auto setEdge(acore::size_type index, EdgeData edge) -> void
    {
        mEdges[index] = edge;
    }

    auto setEdgeCapacity(acore::size_type capacity) -> void
    {
        mEdges.resize(capacity);
    }

    constexpr auto setEdgeCount(acore::size_type count) noexcept -> void
    {
        mEdgeCount = count;
    }

    constexpr auto setFreeEdge(acore::size_type index) noexcept -> void
    {
        mFreeEdge = index;
    }

    constexpr auto setFreeNode(acore::size_type index) noexcept -> void
    {
        mFreeNode = index;
    }

    auto setNode(acore::size_type index, NodeData node) -> void
    {
        mNodes[index] = node;
    }

    auto setNodeCapacity(acore::size_type capacity) -> void
    {
        mNodes.resize(capacity);
    }

    constexpr auto setNodeCount(acore::size_type count) noexcept -> void
    {
        mNodeCount = count;
    }

private:
    acore::size_type mEdgeCount = 0;
    acore::size_type mFreeEdge = acore::INVALID_INDEX;
    acore::size_type mFreeNode = acore::INVALID_INDEX;
    acore::size_type mNodeCount = 0;
    std::vector<NodeData> mNodes;
    std::vector<EdgeData> mEdges;
};
}

#endif
