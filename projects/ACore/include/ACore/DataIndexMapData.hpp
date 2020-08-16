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

#ifndef ACORE_ACOREDATAINDEXMAPDATA_HPP
#define ACORE_ACOREDATAINDEXMAPDATA_HPP

#include "DataIndexMapElement.hpp"

namespace acore
{
//! \cond IMPLEMENTAION_DETAIL
class DataIndexMapData
{
public:
    void clear()
    {
        mData.clear();
        mCount = 0;
    }

    [[nodiscard]] constexpr auto count() const noexcept -> size_type
    {
        return mCount;
    }

    [[nodiscard]] auto count(size_type element) const -> size_type
    {
        return static_cast<size_type>(mData[element].size());
    }

    auto insert(size_type element, size_type key, size_type value) -> void
    {
        if (static_cast<size_type>(mData.size()) <= element)
        {
            mData.resize(element + 1);
        }

        const auto it = find(element, key);

        if (it != mData[element].end())
        {
            it->value = value;
        }
        else
        {
            mData[element].emplace_back(DataIndexMapElement{key, value});
            ++mCount;
        }
    }

    auto remove(size_type element) -> void
    {
        mCount -= mData[element].size();
        mData[element].clear();
    }

    auto remove(size_type element, size_type key) -> void
    {
        const auto it = find(element, key);

        if (it != mData[element].end())
        {
            --mCount;
            mData[element].erase(it);
        }
    }

    auto shrink_to_fit() -> void
    {
        auto i = static_cast<size_type>(mData.size());

        while (i > 0 && mData[i - 1].empty())
        {
            --i;
        }

        mData.resize(i);
    }

    [[nodiscard]] auto size() const noexcept -> size_type
    {
        return static_cast<size_type>(mData.size());
    }

    [[nodiscard]] auto value(size_type element, size_type key) const -> size_type
    {
        for (const DataIndexMapElement &e : mData[element])
        {
            if (e.key == key)
            {
                return e.value;
            }
        }

        throw Exception{} << "Element-key pair '" << element << "' : '" << key << "' must be in the map.";
    }

    [[nodiscard]] auto values(size_type element) const -> std::vector<DataIndexMapElement>
    {
        return mData[element];
    }

private:
    [[nodiscard]] auto find(size_type element, size_type key) -> std::vector<DataIndexMapElement>::iterator
    {
        return std::find_if(mData[element].begin(), mData[element].end(), [&](const DataIndexMapElement &e) {
            return e.key == key;
        });
    }

    std::vector<std::vector<DataIndexMapElement>> mData;
    size_type mCount = 0;
};
//! \endcond
}

#endif
