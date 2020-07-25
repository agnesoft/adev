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

#ifndef AGRAPH_GRAPHSEARCH_HPP
#define AGRAPH_GRAPHSEARCH_HPP

#include "AGraphModule.hpp"

#include <utility>
#include <vector>

namespace agraph
{
enum class SearchControl : acore::size_type
{
    Continue = 0,
    Finish = 1,
    Skip = 2,
    Stop = 3
};

template<typename GraphType, typename SearchType>
class GraphSearch
{
    using EdgeRange = std::pair<typename GraphType::Node::const_iterator, typename GraphType::Node::const_iterator>;
    using GetEdgeRange = auto (*)(const typename GraphType::Node &node) -> EdgeRange;
    using GetDestination = auto (*)(const typename GraphType::Edge &edge) -> acore::size_type;
    using HandlerCaller = auto (*)(const void *handler, acore::size_type, acore::size_type) -> SearchControl;

public:
    template<typename Handler>
    [[nodiscard]] static auto from(const typename GraphType::Node &node, const Handler &handler) -> std::vector<acore::size_type>
    {
        SearchType searchImpl;
        searchImpl.search(
            node,
            handler,
            [](const typename GraphType::Node &node) {
                return EdgeRange{node.begin(), node.end()};
            },
            [](const typename GraphType::Edge &edge) {
                return edge.to().index();
            });
        return searchImpl.mElements;
    }

    template<typename Handler>
    [[nodiscard]] static auto to(const typename GraphType::Node &node, const Handler &handler) -> std::vector<acore::size_type>
    {
        SearchType searchImpl;
        searchImpl.search(
            node,
            handler,
            [](const typename GraphType::Node &node) {
                return EdgeRange{node.rbegin(), node.rend()};
            },
            [](const typename GraphType::Edge &edge) {
                return edge.from().index();
            });
        return searchImpl.mElements;
    }

protected:
    struct Index
    {
        acore::size_type value = acore::INVALID_INDEX;
        acore::size_type distance = acore::INVALID_INDEX;
    };

    constexpr GraphSearch() = default;

    constexpr auto handleIndex(Index index)
    {
        switch (mHandlerCaller(mHandler, index.value, index.distance))
        {
        case SearchControl::Continue:
            mElements.push_back(index.value);
            [[fallthrough]];
        case SearchControl::Skip:
            expandIndex(index);
            [[fallthrough]];
        case SearchControl::Stop:
            break;
        case SearchControl::Finish:
            mGood = false;
            break;
        }
    }

private:
    constexpr auto expandEdge(acore::size_type destination, acore::size_type distance) -> void
    {
        if (!mVisited[destination])
        {
            mVisited[destination] = true;
            mStack.push_back(Index{destination, distance});
        }
    }

    constexpr auto expandIndex(Index index) -> void
    {
        if (agraph::isNode(index.value))
        {
            const auto node = mGraph->node(index.value);
            expandNode(mGetEdgeRange(node), index.distance + 1);
        }
        else
        {
            const auto edge = mGraph->edge(index.value);
            expandEdge(mGetDestination(edge), index.distance + 1);
        }
    }

    constexpr auto expandNode(std::pair<typename GraphType::Node::const_iterator, typename GraphType::Node::const_iterator> range, acore::size_type distance) -> void
    {
        for (auto it = range.first; it != range.second; ++it)
        {
            mStack.push_back(Index{(*it).index(), distance});
        }
    }

    template<typename Handler>
    constexpr auto reset(const typename GraphType::Node &node, const Handler &handler, GetEdgeRange getEdgeRange, GetDestination getDestination) -> void
    {
        mElements.clear();
        mGetEdgeRange = getEdgeRange;
        mGetDestination = getDestination;
        mGood = true;
        mGraph = node.graph();
        mHandler = &handler;
        mHandlerCaller = [](const void *handler, acore::size_type index, acore::size_type distance) {
            return (*static_cast<const Handler *>(handler))(index, distance);
        };
        mStack = std::vector<Index>{Index{node.index(), 0}};
        mVisited = std::vector<bool>(mGraph->storage()->nodeCapacity(), false);
        mVisited[node.index()] = true;
    }

    constexpr auto search() -> void
    {
        while (mGood && !mStack.empty())
        {
            static_cast<SearchType *>(this)->processStack(&mStack);
        }
    }

    template<typename Handler>
    constexpr auto search(const typename GraphType::Node &node, const Handler &handler, GetEdgeRange getEdgeRange, GetDestination getDestination) -> void
    {
        validateNode(node);
        reset(node, handler, getEdgeRange, getDestination);
        search();
    }

    constexpr static auto validateNode(const typename GraphType::Node &node) -> void
    {
        if (!node.isValid())
        {
            throw acore::Exception{} << "Invalid node (" << node.index() << ") used for graph search";
        }
    }

    const GraphType *mGraph = nullptr;
    const void *mHandler = nullptr;
    HandlerCaller mHandlerCaller;
    GetEdgeRange mGetEdgeRange;
    GetDestination mGetDestination;
    std::vector<bool> mVisited;
    std::vector<Index> mStack;
    std::vector<acore::size_type> mElements;
    bool mGood = true;
};
}

#endif
