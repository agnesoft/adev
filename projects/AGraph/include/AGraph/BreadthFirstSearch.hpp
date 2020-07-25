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

#include <vector>

namespace agraph
{
template<typename GraphType>
class BreadthFirstSearch : public GraphSearch<GraphType, BreadthFirstSearch<GraphType>>
{
public:
    using GraphSearch<GraphType, BreadthFirstSearch>::GraphSearch;

private:
    using Base = GraphSearch<GraphType, BreadthFirstSearch>;
    friend Base;

    constexpr BreadthFirstSearch() = default;

    constexpr auto processStack(std::vector<typename Base::Index> *stack) -> void
    {
        for (auto index : takeStack(stack))
        {
            handleIndex(index);
        }
    }

    [[nodiscard]] auto takeStack(std::vector<typename Base::Index> *stack) -> std::vector<typename Base::Index>
    {
        std::vector<typename GraphSearch<GraphType, BreadthFirstSearch>::Index> currentStack;
        currentStack.swap(*stack);
        return currentStack;
    }
};
}

#endif
