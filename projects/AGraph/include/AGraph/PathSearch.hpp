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
template<typename GraphType>
class PathSearch
{
    using HandlerCaller = auto (*)(const void *handler, acore::size_type, acore::size_type) -> acore::size_type;

public:
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
