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

#ifndef AGRAPH_DEPTHFIRSTSEARCH_HPP
#define AGRAPH_DEPTHFIRSTSEARCH_HPP

#include "AGraphModule.hpp"
#include "GraphSearch.hpp"

namespace agraph
{
template<typename GraphType, typename Handler>
class DepthFirstSearch : public GraphSearch<GraphType, Handler, DepthFirstSearch<GraphType, Handler>>
{
public:
    using GraphSearch<GraphType, Handler, DepthFirstSearch<GraphType, Handler>>::GraphSearch;

private:
    friend class GraphSearch<GraphType, Handler, DepthFirstSearch<GraphType, Handler>>;

    constexpr auto processStackFrom() -> void
    {
        searchFromElement(this->takeLast());
    }

    constexpr auto processStackTo() -> void
    {
        searchToElement(this->takeLast());
    }

    constexpr auto searchFromElement(acore::size_type index) -> void
    {
        if (isNode(index))
        {
            this->processFromNode(index);
        }
        else
        {
            this->processFromEdge(index);
        }
    }

    constexpr auto searchToElement(acore::size_type index) -> void
    {
        if (isNode(index))
        {
            this->processToNode(index);
        }
        else
        {
            this->processToEdge(index);
        }
    }
};
}

#endif
