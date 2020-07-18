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

#ifndef AGRAPH_PERSISTENTGRAPHDATA_HPP
#define AGRAPH_PERSISTENTGRAPHDATA_HPP

#include "AGraphModule.hpp"
#include "GraphData.hpp"

namespace agraph
{
//! \cond IMPLEMENTAION_DETAIL
class PersistentGraphData
{
public:
    explicit PersistentGraphData(afile::File *file);
    PersistentGraphData(afile::File *file, acore::size_type index);
    PersistentGraphData(const PersistentGraphData &other) = delete;
    PersistentGraphData(PersistentGraphData &&other) noexcept = default;
    ~PersistentGraphData() = default;

    [[nodiscard]] auto edge(acore::size_type index) const -> EdgeData;

    [[nodiscard]] constexpr auto edgeCapacity() const noexcept -> acore::size_type
    {
        return mEdges.size();
    }

    [[nodiscard]] constexpr auto edgeCount() const noexcept -> acore::size_type
    {
        return mFileIndex.edgeCount;
    }

    [[nodiscard]] constexpr auto file() const noexcept -> afile::File *
    {
        return mFile;
    }

    [[nodiscard]] constexpr auto fileIndex() const noexcept -> acore::size_type
    {
        return mIndex;
    }

    [[nodiscard]] constexpr auto freeEdge() const noexcept -> acore::size_type
    {
        return mFileIndex.freeEdge;
    }

    [[nodiscard]] constexpr auto freeNode() const noexcept -> acore::size_type
    {
        return mFileIndex.freeNode;
    }

    [[nodiscard]] auto node(acore::size_type index) const -> NodeData;

    [[nodiscard]] constexpr auto nodeCapacity() const noexcept -> acore::size_type
    {
        return mNodes.size();
    }

    [[nodiscard]] constexpr auto nodeCount() const noexcept -> acore::size_type
    {
        return mFileIndex.nodeCount;
    }

    auto setEdge(acore::size_type index, EdgeData edge) -> void;
    auto setEdgeCapacity(acore::size_type capacity) -> void;
    auto setEdgeCount(acore::size_type count) -> void;
    auto setFreeEdge(acore::size_type index) -> void;
    auto setFreeNode(acore::size_type index) -> void;
    auto setNode(acore::size_type index, NodeData node) -> void;
    auto setNodeCapacity(acore::size_type capacity) -> void;
    auto setNodeCount(acore::size_type count) -> void;
    [[nodiscard]] auto toGraphData() const -> GraphData;

    auto operator=(const PersistentGraphData &other) -> PersistentGraphData & = delete;
    auto operator=(PersistentGraphData &&other) noexcept -> PersistentGraphData & = default;

private:
    struct FileIndex
    {
        acore::size_type nodesIndex = acore::INVALID_INDEX;
        acore::size_type nodeCount = 0;
        acore::size_type edgesIndex = acore::INVALID_INDEX;
        acore::size_type edgeCount = 0;
        acore::size_type freeNode = acore::INVALID_INDEX;
        acore::size_type freeEdge = acore::INVALID_INDEX;
    };

    template<typename Buffer>
    friend constexpr auto operator<<(acore::DataStreamBase<Buffer> &stream, const FileIndex &index) -> acore::DataStreamBase<Buffer> &
    {
        return stream << index.nodesIndex << index.nodeCount << index.edgesIndex << index.edgeCount << index.freeNode << index.freeEdge;
    }

    template<typename Buffer>
    friend constexpr auto operator>>(acore::DataStreamBase<Buffer> &stream, FileIndex &index) -> acore::DataStreamBase<Buffer> &
    {
        return stream >> index.nodesIndex >> index.nodeCount >> index.edgesIndex >> index.edgeCount >> index.freeNode >> index.freeEdge;
    }

    [[nodiscard]] static auto beginInit(afile::File *file) -> afile::File *;
    static auto endInit(afile::File *file) -> void;

    afile::File *mFile = nullptr;
    acore::size_type mIndex = acore::INVALID_INDEX;
    FileIndex mFileIndex;
    afile::PersistentVector<NodeData> mNodes;
    afile::PersistentVector<EdgeData> mEdges;
};
//! \endcond
}

#endif
