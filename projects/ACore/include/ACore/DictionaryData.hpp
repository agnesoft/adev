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

#ifndef ACORE_DICTIONARYDATA_HPP
#define ACORE_DICTIONARYDATA_HPP

#include "ACoreModule.hpp"
#include "Variant.hpp"

#include <iterator>
#include <unordered_map>
#include <utility>
#include <vector>

namespace acore
{
//! \cond IMPLEMENTAION_DETAIL
class DictionaryData
{
public:
    [[nodiscard]] auto capacity() const noexcept -> size_type
    {
        return static_cast<size_type>(mData.size());
    }

    void clear()
    {
        mIndex.clear();
        mData.clear();
        mFree.clear();
        mCount = 0;
    }

    [[nodiscard]] constexpr auto count() const noexcept -> size_type
    {
        return mCount;
    }

    [[nodiscard]] auto count(size_type index) const -> size_type
    {
        return static_cast<size_type>(mData[index].count);
    }

    [[nodiscard]] auto indexes(size_type hash) const -> std::vector<size_type>
    {
        std::vector<size_type> vals;
        const auto range = mIndex.equal_range(hash);
        vals.reserve(static_cast<std::size_t>(std::distance(range.first, range.second)));

        for (auto it = range.first; it != range.second; ++it)
        {
            vals.push_back(it->second);
        }

        return vals;
    }

    auto insert(size_type index) -> void
    {
        mData[index].count++;
    }

    auto insert(size_type hash, const Variant &value) -> size_type
    {
        size_type index = freeIndex();
        insert(index, hash, value);
        return index;
    }

    auto remove(size_type index, size_type hash) -> void
    {
        const auto range = mIndex.equal_range(hash);
        const auto it = std::find_if(range.first, range.second, [&](const auto val) {
            return val.second == index;
        });
        mIndex.erase(it);
        mData[index] = Value{};
        mFree.push_back(index);
        mCount--;
    }

    auto remove(size_type index) -> void
    {
        mData[index].count--;
    }

    [[nodiscard]] auto value(size_type index) const -> Variant
    {
        return mData[index].value;
    }

private:
    struct Value
    {
        size_type count = 0;
        Variant value;
    };

    auto insert(size_type index, size_type hash, const Variant &value) -> void
    {
        mIndex.insert(std::pair<size_type, size_type>{hash, index});

        if (index == static_cast<size_type>(mData.size()))
        {
            mData.emplace_back(Value{1, value});
        }
        else
        {
            mData[index] = Value{1, value};
        }

        mCount++;
    }

    [[nodiscard]] auto freeIndex() -> size_type
    {
        if (mFree.empty())
        {
            return count();
        }

        const size_type free = mFree.back();
        mFree.pop_back();
        return free;
    }

    size_type mCount = 0;
    std::unordered_multimap<size_type, size_type> mIndex;
    std::vector<Value> mData;
    std::vector<size_type> mFree;
};
//! \endcond
}

#endif
