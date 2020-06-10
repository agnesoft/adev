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

#include <unordered_map>

namespace acore
{
class DataIndexMapData
{
public:
    void clear()
    {
        mData.clear();
        mCount = 0;
    }

    [[nodiscard]] constexpr size_type count() const noexcept
    {
        return mCount;
    }

    [[nodiscard]] size_type count(size_type element) const
    {
        if (isValid(element))
        {
            return static_cast<size_type>(mData[element].size());
        }

        return 0;
    }

    void insert(size_type element, size_type key, size_type value)
    {
        if (element >= static_cast<size_type>(mData.size()))
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

    void remove(size_type element)
    {
        if (isValid(element))
        {
            mCount -= mData[element].size();
            mData[element].clear();
        }
    }

    void remove(size_type element, size_type key)
    {
        if (isValid(element))
        {
            const auto it = find(element, key);

            if (it != mData[element].end())
            {
                --mCount;
                mData[element].erase(it);
            }
        }
    }

    [[nodiscard]] auto size() const noexcept
    {
        return mData.size();
    }

    [[nodiscard]] size_type value(size_type element, size_type key) const
    {
        if (isValid(element))
        {
            for (const DataIndexMapElement &e : mData[element])
            {
                if (e.key == key)
                {
                    return e.value;
                }
            }
        }

        return INVALID_INDEX;
    }

    [[nodiscard]] auto values(size_type element) const -> std::vector<DataIndexMapElement>
    {
        if (isValid(element))
        {
            return mData[element];
        }

        return {};
    }

private:
    [[nodiscard]] auto isValid(size_type element) const noexcept -> bool
    {
        return element < static_cast<size_type>(mData.size());
    }

    [[nodiscard]] auto find(size_type element, size_type key) -> std::vector<DataIndexMapElement>::iterator
    {
        return std::find_if(mData[element].begin(), mData[element].end(), [&](const DataIndexMapElement &e) {
            return e.key == key;
        });
    }

    [[nodiscard]] auto find(size_type element, size_type key) const -> std::vector<DataIndexMapElement>::const_iterator
    {
        return std::find_if(mData[element].begin(), mData[element].end(), [&](const DataIndexMapElement &e) {
            return e.key == key;
        });
    }

    std::vector<std::vector<DataIndexMapElement>> mData;
    size_type mCount = 0;
};
}

#endif
