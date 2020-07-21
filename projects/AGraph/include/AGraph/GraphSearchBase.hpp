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

#ifndef AGRAPH_GRAPHSEARCHBASE_HPP
#define AGRAPH_GRAPHSEARCHBASE_HPP

#include "Common.hpp"

#include <vector>

namespace agraph
{
template<typename GraphType, typename Handler>
class GraphSearchBase
{
public:
    constexpr GraphSearchBase(const GraphType *graph, const Handler &handler) :
        mGraph{graph},
        mHandler{handler}
    {
    }

    [[nodiscard]] constexpr auto graph() const noexcept -> const GraphType *
    {
        return mGraph;
    }

    [[nodiscard]] constexpr auto handler() const noexcept -> const Handler &
    {
        return mHandler;
    }

protected:
    [[nodiscard]] constexpr auto getHandler() noexcept -> Handler &
    {
        return mHandler;
    }

    [[nodiscard]] constexpr auto isVisited(acore::size_type index) const -> bool
    {
        return mVisited[index];
    }

    constexpr auto reset() -> void
    {
        mVisited = std::vector<bool>(graph()->storage()->nodeCapacity(), false);
    }

    constexpr auto setVisited(acore::size_type index) -> void
    {
        mVisited[index] = true;
    }

    constexpr auto validate(const typename GraphType::Node &node) const -> void
    {
        if (!node.isValid())
        {
            throw acore::Exception{} << "Invalid node (" << node.index() << ") used for graph search";
        }
    }

private:
    const GraphType *mGraph = nullptr;
    Handler mHandler;
    std::vector<bool> mVisited;
};
}

#endif
