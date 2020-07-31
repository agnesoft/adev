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

#ifndef ADB_ELEMENT_HPP
#define ADB_ELEMENT_HPP

#include "ADbModule.hpp"
#include "KeyValue.hpp"

#include <vector>

namespace adb
{
class Element
{
public:
    Element() = default;

    explicit Element(acore::size_type index) :
        mIndex{index}
    {
    }

    Element(acore::size_type index, std::vector<KeyValue> data) :
        mIndex{index},
        mData{std::move(data)}
    {
    }

    [[nodiscard]] constexpr auto data() const noexcept -> const std::vector<KeyValue> &
    {
        return mData;
    }

    [[nodiscard]] constexpr auto index() const noexcept -> acore::size_type
    {
        return mIndex;
    }

private:
    acore::size_type mIndex = acore::INVALID_INDEX;
    std::vector<KeyValue> mData;
};

[[nodiscard]] constexpr auto operator==(const Element &left, const Element &right) -> bool
{
    return left.index() == right.index() && left.data() == right.data();
}

[[nodiscard]] constexpr auto operator!=(const Element &left, const Element &right) -> bool
{
    return !(left == right);
}
}

#endif
