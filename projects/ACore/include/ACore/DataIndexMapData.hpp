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
    using const_iterator = std::unordered_multimap<size_type, DataIndexMapElement>::const_iterator;

    [[nodiscard]] const_iterator begin() const
    {
        return mData.begin();
    }

    void clear()
    {
        mData.clear();
    }

    [[nodiscard]] size_type count() const noexcept
    {
        return mData.size();
    }

    [[nodiscard]] size_type count(size_type element) const
    {
        return mData.count(element);
    }

    [[nodiscard]] const_iterator end() const
    {
        return mData.end();
    }

    [[nodiscard]] const_iterator find(size_type element, size_type key) const
    {
        const auto range = mData.equal_range(element);

        for (auto it = range.first; it != range.second; ++it)
        {
            if (it->second.key == key)
            {
                return it;
            }
        }

        return end();
    }

    void insert(size_type element, size_type key, size_type value)
    {
        mData.insert(std::pair<size_type, DataIndexMapElement>{element, {key, value}});
    }

    void remove(size_type element)
    {
        mData.erase(element);
    }

    void remove(size_type element, size_type key)
    {
        const auto it = find(element, key);

        if (it != end())
        {
            mData.erase(it);
        }
    }

    void setValue(size_type element, size_type key, size_type value)
    {
        auto it = find(element, key);

        if (it != end())
        {
            it->second.value = value;
        }
    }

    [[nodiscard]] size_type value(size_type element, size_type key) const
    {
        const auto it = find(element, key);

        if (it != end())
        {
            return it->second.value;
        }

        return INVALID_INDEX;
    }

    [[nodiscard]] std::vector<DataIndexMapElement> values(size_type element) const
    {
        const auto range = mData.equal_range(element);
        std::vector<DataIndexMapElement> vals;
        vals.reserve(std::distance(range.first, range.second));

        for (auto it = range.first; it != range.second; ++it)
        {
            vals.push_back(it->second);
        }

        return vals;
    }

private:
    [[nodiscard]] std::unordered_multimap<size_type, DataIndexMapElement>::iterator find(size_type element, size_type key)
    {
        const auto range = mData.equal_range(element);

        for (auto it = range.first; it != range.second; ++it)
        {
            if (it->second.key == key)
            {
                return it;
            }
        }

        return mData.end();
    }

    std::unordered_multimap<size_type, DataIndexMapElement> mData;
};
}

#endif
