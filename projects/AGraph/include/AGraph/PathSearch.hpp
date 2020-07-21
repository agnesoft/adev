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

#include "BreadthFirstSearch.hpp"
#include "GraphSearchBase.hpp"
#include "PathSearchHandler.hpp"

namespace agraph
{
template<typename GraphType, typename Handler = PathSearchHandler>
class PathSearch : public GraphSearchBase<GraphType, Handler>
{
public:
    using GraphSearchBase<GraphType, Handler>::GraphSearchBase;

    explicit constexpr PathSearch(const GraphType *graph) :
        GraphSearchBase<GraphType, Handler>(graph, Handler())
    {
    }

    [[nodiscard]] auto path(const typename GraphType::Node &from, const typename GraphType::Node &to) -> std::vector<acore::size_type>
    {
        reset(from, to);

        while (!isFinished())
        {
            processPaths();
        }

        return mElements;
    }

private:
    struct Path
    {
        std::vector<acore::size_type> elements;
        acore::size_type cost = 0;
    };

    constexpr auto expandEdges(const typename GraphType::Node &node) -> void
    {
        for (const typename GraphType::Edge &edge : node)
        {
            expand<Path>(edge.index());
        }
    }

    constexpr auto expandPath(acore::size_type index, acore::size_type cost, Path &&path) -> void
    {
        mPaths.append(std::move(path));
        mPaths.last().elements << index;
        mPaths.last().cost += cost;
    }

    constexpr auto finish() -> void
    {
        mElements = mCurrentPath.elements;
    }

    [[nodiscard]] constexpr bool isDestination(acore::size_type index) const noexcept
    {
        return index == mTo;
    }

    [[nodiscard]] constexpr auto isFinished() const noexcept -> bool
    {
        return !mElements.empty() || mPaths.empty();
    }

    constexpr auto handleEdge(acore::size_type index) -> void
    {
        expand<Path>(this->graph()->edge(index).to().index());
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
        acore::size_type cost = this->getHandler()(index, mCurrentPath.elements.size());

        if (cost != 0)
        {
            expandPath(index, cost, T{mCurrentPath});
        }
    }

    constexpr auto processIndex(acore::size_type index) -> void
    {
        if (isNode(index))
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
        if (!this->isVisited(index))
        {
            this->setVisited(index);
            expandEdges(this->graph()->node(index));
        }
    }

    constexpr auto processLastPath() -> void
    {
        mCurrentPath = std::move(mPaths.last());
        mPaths.resize(mPaths.size() - 1);
        processIndex(mCurrentPath.elements.last());
    }

    constexpr auto processPaths() -> void
    {
        sortPaths();
        processLastPath();
    }

    constexpr auto reset(const typename GraphType::Node &from, const typename GraphType::Node &to) -> void
    {
        validateNodes(from, to);
        GraphSearchBase<GraphType, Handler>::reset();
        mPaths.append({{from.index()}, 0});
        mTo = to.index();
    }

    constexpr auto sortPaths() -> void
    {
        std::sort(mPaths.begin(), mPaths.end(), [](const Path &left, const Path &right) {
            return left.cost > right.cost;
        });
    }

    constexpr auto validateNodes(const typename GraphType::Node &from, const typename GraphType::Node &to) const -> void
    {
        this->validate(from);
        this->validate(to);
    }

    std::vector<Path> mPaths;
    std::vector<acore::size_type> mElements;
    acore::size_type mTo = acore::INVALID_INDEX;
    Path mCurrentPath;
};
}

#endif
