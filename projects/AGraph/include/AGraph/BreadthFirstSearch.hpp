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

#ifndef AGRAPH_BREADTHFIRSTSEARCH_HPP
#define AGRAPH_BREADTHFIRSTSEARCH_HPP

#include "AGraphModule.hpp"
#include "GraphSearch.hpp"

namespace agraph
{
template<typename GraphType, typename Handler>
class BreadthFirstSearch : public GraphSearch<GraphType, Handler, BreadthFirstSearch<GraphType, Handler>>
{
public:
    using GraphSearch<GraphType, Handler, BreadthFirstSearch<GraphType, Handler>>::GraphSearch;

private:
    friend class GraphSearch<GraphType, Handler, BreadthFirstSearch<GraphType, Handler>>;

    auto processEdgesDestinations() -> void
    {
        std::vector<acore::size_type> edges = this->takeAll();

        while (this->isGood() && !edges.empty())
        {
            this->processFromEdge(takeLast(&edges));
        }
    }

    auto processEdgesOrigins() -> void
    {
        std::vector<acore::size_type> edges = this->takeAll();

        while (this->isGood() && !edges.empty())
        {
            this->processToEdge(takeLast(&edges));
        }
    }

    auto processNodesFrom() -> void
    {
        std::vector<acore::size_type> nodes = this->takeAll();

        while (this->isGood() && !nodes.empty())
        {
            this->processFromNode(takeLast(&nodes));
        }
    }

    auto processNodesTo() -> void
    {
        std::vector<acore::size_type> nodes = this->takeAll();

        while (this->isGood() && !nodes.empty())
        {
            this->processToNode(takeLast(&nodes));
        }
    }

    constexpr auto processStackFrom() -> void
    {
        processNodesFrom();
        processEdgesDestinations();
    }

    constexpr auto processStackTo() -> void
    {
        processNodesTo();
        processEdgesOrigins();
    }

    [[nodiscard]] constexpr static auto takeLast(std::vector<acore::size_type> *vec) -> acore::size_type
    {
        const acore::size_type value = vec->back();
        vec->pop_back();
        return value;
    }
};
}

#endif
