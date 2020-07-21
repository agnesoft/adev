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
#include "GraphSearchBase.hpp"
#include "GraphSearchControl.hpp"

#include <vector>

namespace agraph
{
template<typename GraphType, typename Handler, typename SearchType>
class GraphSearch : public GraphSearchBase<GraphType, Handler>
{
public:
    using GraphSearchBase<GraphType, Handler>::GraphSearchBase;

    [[nodiscard]] auto from(const typename GraphType::Node &node) -> std::vector<acore::size_type>
    {
        this->validate(node);
        reset();
        searchFrom(node.index());
        return mElements;
    }

    [[nodiscard]] auto to(const typename GraphType::Node &node) -> std::vector<acore::size_type>
    {
        this->validate(node);
        reset();
        searchTo(node.index());
        return mElements;
    }

protected:
    [[nodiscard]] constexpr auto isGood() const noexcept -> bool
    {
        return mGood;
    }

    constexpr auto processFromEdge(acore::size_type index) -> void
    {
        processEdge(index, &GraphSearch::addDestinationNode);
    }

    constexpr auto processFromNode(acore::size_type index) -> void
    {
        if (!this->isVisited(index))
        {
            processNode(index, &GraphSearch::addEdgesFromNode);
        }
    }

    constexpr auto processToEdge(acore::size_type index) -> void
    {
        processEdge(index, &GraphSearch::addOriginNode);
    }

    constexpr auto processToNode(acore::size_type index) -> void
    {
        if (!this->isVisited(index))
        {
            processNode(index, &GraphSearch::addEdgesToNode);
        }
    }

    [[nodiscard]] constexpr auto takeLast() -> acore::size_type
    {
        mDistance = mDistances[mStack.size() - 1];
        mDistances.pop_back();
        return mStack.takeLast();
    }

    [[nodiscard]] auto takeAll() -> std::vector<acore::size_type>
    {
        mDistance = mDistances[mStack.size() - 1];
        mDistances.clear();
        std::vector<acore::size_type> stack;
        stack.swap(mStack);
        return stack;
    }

private:
    constexpr auto addDestinationNode(acore::size_type index) -> void
    {
        mStack.append(this->graph()->edge(index).to().index());
        mDistances.append(mDistance + 1);
    }

    constexpr auto addEdgesFromNode(acore::size_type index) -> void
    {
        for (const typename GraphType::Edge &edge : this->graph()->node(index))
        {
            mStack.append(edge.index());
            mDistances.append(mDistance + 1);
        }
    }

    constexpr auto addEdgesToNode(acore::size_type index) -> void
    {
        const typename GraphType::Node node = this->graph()->node(index);

        for (auto it = node.rbegin(); it != node.rend(); ++it)
        {
            mStack.append((*it).index());
            mDistances.append(mDistance + 1);
        }
    }

    constexpr auto addOriginNode(acore::size_type index) -> void
    {
        mStack.append(this->graph()->edge(index).from().index());
        mDistances.append(mDistance + 1);
    }

    constexpr auto addResultElement(acore::size_type index) -> void
    {
        mElements.append(index);
    }

    constexpr auto processEdge(acore::size_type index, void (SearchType::*proc)(acore::size_type)) -> void
    {
        processElement(index, proc);
    }

    constexpr auto processElement(acore::size_type index, void (SearchType::*proc)(acore::size_type)) -> void
    {
        switch (this->getHandler()(index, mDistance))
        {
        case SearchControl::Continue:
            addResultElement(index);
            [[fallthrough]];
        case SearchControl::Skip:
            (static_cast<SearchType *>(this)->*proc)(index);
            [[fallthrough]];
        case SearchControl::Stop:
            break;
        case SearchControl::Finish:
            mGood = false;
            break;
        }
    }

    constexpr auto processNode(acore::size_type index, void (SearchType::*proc)(acore::size_type)) -> void
    {
        this->setVisited(index);
        processElement(index, proc);
    }

    constexpr auto reset() -> void
    {
        GraphSearchBase<GraphType, Handler>::reset();

        mGood = true;
        mElements = std::vector<acore::size_type>();
        mStack = std::vector<acore::size_type>();
        mDistances = std::vector<acore::size_type>();
    }

    constexpr auto searchFrom(acore::size_type index) -> void
    {
        mStack.append(index);
        mDistances.append(0);

        while (mGood && !mStack.isEmpty())
        {
            static_cast<SearchType *>(this)->processStackFrom();
        }
    }

    constexpr auto searchFromEdge(acore::size_type index) -> void
    {
        searchFrom(this->graph()->edge(index).to().index());
    }

    constexpr auto searchTo(acore::size_type index) -> void
    {
        mStack.append(index);
        mDistances.append(0);

        while (mGood && !mStack.isEmpty())
        {
            static_cast<SearchType *>(this)->processStackTo();
        }
    }

    constexpr auto searchToEdge(acore::size_type index) -> void
    {
        searchTo(this->graph()->edge(index).from().index());
    }

    std::vector<acore::size_type> mElements;
    std::vector<acore::size_type> mStack;
    std::vector<acore::size_type> mDistances;
    acore::size_type mDistance = 0;
    bool mGood = false;
};
}

#endif
