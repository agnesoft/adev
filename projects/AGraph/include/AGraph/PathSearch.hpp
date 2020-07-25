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

#ifndef AGRAPH_PATHSEARCH_HPP
#define AGRAPH_PATHSEARCH_HPP

#include "AGraphModule.hpp"

namespace agraph
{
//! The PathSearch<GraphType> is a template graph
//! search algorithm implementing A* shortest path
//! search algorithm.
//!
//! The A* algorithm works by traversing the graph and
//! expanding edges of each node based on the current cost
//! of the path that is being examined. The handler
//! is required to determine the cost of each index.
//! The weights can also be negative to decrease the
//! overall cost of a given path.
//!
//! The only special value is \c 0. If a handler returns
//! \c 0 the current path will be abandoned and will not
//! be considered as a valid path. Note that this can
//! lead to a situation where no path is found even
//! though a path in fact exists in the graph - just not
//! such a path that could be satisfied.
//!
//! \warning While the costs are held using the acore::size_type
//! type with very large cost values it might be possible
//! to accidentally overflow it which would lead to
//! undefined behaviour.
//!
//! Example using a simplest graph forming a triangle:
//!
//! \snippet PathSearchTest.cpp [Graph]
//!
//! Searching using a handler that assigns weight of 1
//! to all elements but prohibits going directly
//! from node1 to node3:
//!
//! \snippet PathSearchTest.cpp [Usage]
//!
//! As we see the handler prevents the search to go
//! through the direct connection between the node1 (0)
//! and the node3 (2) represented by the edge (-3) as
//! it returns \c 0 when that index is handled. The
//! search thus needs to find another way and it goes
//! through node2 (1) to reach the destination.
template<typename GraphType>
class PathSearch
{
    using HandlerCaller = auto (*)(const void *handler, acore::size_type, acore::size_type) -> acore::size_type;

public:
    //! Searches for the shortest route between
    //! \a from and \a to nodes on a graph. It is
    //! determined by `weight` of each path. The
    //! handler is used to determine the \c weight
    //! of the current index. The weight is cumulative
    //! for each path. When the destination is
    //! reached the path with the lowest weight is
    //! returned. The handler must be callable with
    //! following signature:
    //!
    //! <tt> auto (acore::size_type index, acore::size_type distance) -> agraph::size_type </tt>
    template<typename Handler>
    [[nodiscard]] static auto path(const typename GraphType::Node &from, const typename GraphType::Node &to, const Handler &handler) -> std::vector<acore::size_type>
    {
        validateNodes(from, to);

        PathSearch searchImpl;
        searchImpl.reset(from, to, handler);

        while (!searchImpl.isFinished())
        {
            searchImpl.processPaths();
        }

        return searchImpl.mElements;
    }

private:
    struct Path
    {
        std::vector<acore::size_type> elements;
        acore::size_type cost = 0;
    };

    constexpr PathSearch() = default;

    constexpr auto expandEdges(const typename GraphType::Node &node) -> void
    {
        for (const typename GraphType::Edge &edge : node)
        {
            expand<Path>(edge.index());
        }
    }

    constexpr auto expandPath(acore::size_type index, acore::size_type cost, Path &&path) -> void
    {
        mPaths.emplace_back(std::move(path));
        mPaths.back().elements.push_back(index);
        mPaths.back().cost += cost;
    }

    constexpr auto finish() -> void
    {
        mElements = mCurrentPath.elements;
    }

    [[nodiscard]] constexpr auto isDestination(acore::size_type index) const noexcept -> bool
    {
        return index == mTo;
    }

    [[nodiscard]] constexpr auto isFinished() const noexcept -> bool
    {
        return !mElements.empty() || mPaths.empty();
    }

    constexpr auto handleEdge(acore::size_type index) -> void
    {
        expand<Path>(mGraph->edge(index).to().index());
    }

    constexpr auto handleNode(acore::size_type index) -> void
    {
        if (isDestination(index))
        {
            finish();
        }
        else
        {
            processNode(index);
        }
    }

    template<typename T>
    constexpr auto expand(acore::size_type index) -> void
    {
        acore::size_type cost = mHandlerCaller(mHandler, index, mCurrentPath.elements.size());

        if (cost != 0)
        {
            expandPath(index, cost, T{mCurrentPath});
        }
    }

    constexpr auto processIndex(acore::size_type index) -> void
    {
        if (agraph::isNode(index))
        {
            handleNode(index);
        }
        else
        {
            handleEdge(index);
        }
    }

    constexpr auto processNode(acore::size_type index) -> void
    {
        if (!mVisited[index])
        {
            mVisited[index] = true;
            expandEdges(mGraph->node(index));
        }
    }

    constexpr auto processLastPath() -> void
    {
        mCurrentPath = std::move(mPaths.back());
        mPaths.resize(mPaths.size() - 1);
        processIndex(mCurrentPath.elements.back());
    }

    constexpr auto processPaths() -> void
    {
        sortPaths();
        processLastPath();
    }

    template<typename Handler>
    constexpr auto reset(const typename GraphType::Node &from, const typename GraphType::Node &to, const Handler &handler) -> void
    {
        mGraph = from.graph();
        mHandler = &handler;
        mHandlerCaller = [](const void *handler, acore::size_type index, acore::size_type distance) {
            return (*static_cast<const Handler *>(handler))(index, distance);
        };
        mVisited = std::vector<bool>(mGraph->storage()->nodeCapacity(), false);
        mPaths.emplace_back(Path{{from.index()}, 0});
        mTo = to.index();
    }

    constexpr auto sortPaths() -> void
    {
        std::sort(mPaths.begin(), mPaths.end(), [](const Path &left, const Path &right) {
            return left.cost > right.cost;
        });
    }

    constexpr static auto validate(const typename GraphType::Node &node) -> void
    {
        if (!node.isValid())
        {
            throw acore::Exception{} << "Invalid node (" << node.index() << ") used for graph search";
        }
    }

    constexpr static auto validateNodes(const typename GraphType::Node &from, const typename GraphType::Node &to) -> void
    {
        validate(from);
        validate(to);
    }

    const GraphType *mGraph = nullptr;
    const void *mHandler = nullptr;
    HandlerCaller mHandlerCaller;
    std::vector<bool> mVisited;
    std::vector<Path> mPaths;
    std::vector<acore::size_type> mElements;
    acore::size_type mTo = acore::INVALID_INDEX;
    Path mCurrentPath;
};
}

#endif
