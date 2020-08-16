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

#ifndef AGRAPH_PERSISTENTGRAPH_HPP
#define AGRAPH_PERSISTENTGRAPH_HPP

#include "Graph.hpp"
#include "PersistentGraphData.hpp"

namespace agraph
{
//! The PersistentGraph provides file based directional
//! graph implementation.
//!
//! The class is a specialization of the GraphBase
//! with data using #afile::File as the internal
//! storage. The functionally this class is identical
//! to #agraph::Graph.
//!
//! It is possible to get the in-memory #agraph::Graph
//! with toGraph() method.
class PersistentGraph : public GraphBase<PersistentGraphData, PersistentGraph>
{
public:
    //! Constructs the graph in \a file.
    explicit PersistentGraph(afile::File *file) :
        GraphBase<PersistentGraphData, PersistentGraph>{file}
    {
    }

    //! Constructs the graph using \a file and
    //! the \a index. The \a index must exist in
    //! the \a file and must contain valid graph data.
    PersistentGraph(afile::File *file, acore::size_type index) :
        GraphBase<PersistentGraphData, PersistentGraph>{file, index}
    {
    }

    //! Deleted copy constructor.
    PersistentGraph(const PersistentGraph &other) = delete;

    //! Move constructor.
    PersistentGraph(PersistentGraph &&other) noexcept = default;

    //! Default destructor.
    ~PersistentGraph() = default;

    //! Returns the underlying file.
    [[nodiscard]] auto file() const noexcept -> afile::File *
    {
        return storage()->file();
    }

    //! Returns \c index to the file's value holding the
    //! data.
    [[nodiscard]] auto fileIndex() const noexcept -> acore::size_type
    {
        return storage()->fileIndex();
    }

    //! Converts the persistent graph to the in-memory
    //! #agraph::Graph.
    [[nodiscard]] auto toGraph() const -> Graph;

    //! Deleted copy assignment operator.
    auto operator=(const PersistentGraph &other) -> PersistentGraph & = delete;

    //! Move assignment operator.
    auto operator=(PersistentGraph &&other) noexcept -> PersistentGraph & = default;
};
}

#endif
