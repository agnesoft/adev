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

#ifndef AGRAPH_ELEMENT_HPP
#define AGRAPH_ELEMENT_HPP

#include "AGraphModule.hpp"

namespace agraph
{
template<typename ElementType, typename GraphType>
class Element
{
public:
    constexpr Element() noexcept = default;

    [[nodiscard]] constexpr auto graph() const noexcept -> const GraphType *
    {
        return mGraph;
    }

    [[nodiscard]] constexpr auto index() const noexcept -> acore::size_type
    {
        return mIndex;
    }

    [[nodiscard]] constexpr auto isValid() const noexcept -> bool
    {
        return mIndex != acore::INVALID_INDEX && mGraph && mGraph->contains(static_cast<const ElementType &>(*this));
    }

protected:
    constexpr Element(acore::size_type index, const GraphType *graph) noexcept :
        mIndex{index},
        mGraph{graph}
    {
    }

private:
    acore::size_type mIndex = acore::INVALID_INDEX;
    const GraphType *mGraph = nullptr;
};

template<typename ElementType, typename GraphType>
[[nodiscard]] constexpr auto operator==(Element<ElementType, GraphType> left, Element<ElementType, GraphType> right) noexcept -> bool
{
    return left.graph() == right.graph() && left.index() == right.index();
}

template<typename ElementType, typename GraphType>
[[nodiscard]] constexpr auto operator!=(Element<ElementType, GraphType> left, Element<ElementType, GraphType> right) noexcept -> bool
{
    return !(left == right);
}
}

#endif
