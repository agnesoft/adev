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

#include "pch.hpp"

#include "PersistentGraphData.hpp"

namespace agraph
{
PersistentGraphData::PersistentGraphData(afile::File *file) :
    mFile{beginInit(file)},
    mNodes{file},
    mEdges{file}
{
    mFileIndex.nodesIndex = mNodes.fileIndex();
    mFileIndex.edgesIndex = mEdges.fileIndex();
    mIndex = mFile->insert(mFileIndex);
    endInit(file);
}

PersistentGraphData::PersistentGraphData(afile::File *file, acore::size_type index) :
    mFile{file},
    mIndex{index},
    mFileIndex{mFile->value<FileIndex>(mIndex)},
    mNodes{mFile, mFileIndex.nodesIndex},
    mEdges{mFile, mFileIndex.edgesIndex}

{
}

auto PersistentGraphData::edge(acore::size_type index) const -> EdgeData
{
    return mEdges[index];
}

auto PersistentGraphData::node(acore::size_type index) const -> NodeData
{
    return mNodes[index];
}

auto PersistentGraphData::setEdge(acore::size_type index, EdgeData edge) -> void
{
    mEdges[index] = edge;
}

auto PersistentGraphData::setEdgeCapacity(acore::size_type capacity) -> void
{
    mEdges.resize(capacity, EdgeData{});
}

auto PersistentGraphData::setEdgeCount(acore::size_type count) -> void
{
    mFileIndex.edgeCount = count;
    mFile->insert(mIndex, static_cast<acore::size_type>(offsetof(struct FileIndex, edgeCount)), mFileIndex.edgeCount);
}

auto PersistentGraphData::setFreeEdge(acore::size_type index) -> void
{
    mFileIndex.freeEdge = index;
    mFile->insert(mIndex, static_cast<acore::size_type>(offsetof(struct FileIndex, freeEdge)), mFileIndex.freeEdge);
}

auto PersistentGraphData::setFreeNode(acore::size_type index) -> void
{
    mFileIndex.freeNode = index;
    mFile->insert(mIndex, static_cast<acore::size_type>(offsetof(struct FileIndex, freeNode)), mFileIndex.freeNode);
}

auto PersistentGraphData::setNode(acore::size_type index, NodeData node) -> void
{
    mNodes[index] = node;
}

auto PersistentGraphData::setNodeCapacity(acore::size_type capacity) -> void
{
    mNodes.resize(capacity, NodeData{});
}

auto PersistentGraphData::setNodeCount(acore::size_type count) -> void
{
    mFileIndex.nodeCount = count;
    mFile->insert(mIndex, static_cast<acore::size_type>(offsetof(struct FileIndex, nodeCount)), mFileIndex.nodeCount);
}

auto PersistentGraphData::toGraphData() const -> GraphData
{
    GraphData data;

    data.setEdgeCapacity(edgeCapacity());
    data.setEdgeCount(edgeCount());
    data.setNodeCapacity(nodeCapacity());
    data.setNodeCount(nodeCount());
    data.setFreeEdge(freeEdge());
    data.setFreeNode(freeNode());

    for (acore::size_type i = 0; i < nodeCapacity(); i++)
    {
        data.setNode(i, node(i));
    }

    for (acore::size_type i = 0; i < edgeCapacity(); i++)
    {
        data.setEdge(i, edge(i));
    }

    return data;
}

auto PersistentGraphData::beginInit(afile::File *file) -> afile::File *
{
    file->beginWAL();
    return file;
}

auto PersistentGraphData::endInit(afile::File *file) -> void
{
    file->endWAL();
}
}
