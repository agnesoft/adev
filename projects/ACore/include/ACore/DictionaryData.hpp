#pragma once

#include "Variant.hpp"

#include <unordered_map>

namespace acore
{
class DictionaryData
{
public:
    [[nodiscard]] size_type capacity() const noexcept
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

    [[nodiscard]] constexpr size_type count() const noexcept
    {
        return mCount;
    }

    [[nodiscard]] size_type count(size_type index) const
    {
        return mData[index].count;
    }

    [[nodiscard]] std::vector<size_type> indexes(size_type hash) const
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

    void insert(size_type index)
    {
        mData[index].count++;
    }

    size_type insert(size_type hash, const Variant &value)
    {
        size_type index = freeIndex();
        insert(index, hash, value);
        return index;
    }

    void remove(size_type index, size_type hash)
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

    void remove(size_type index)
    {
        mData[index].count--;
    }

    [[nodiscard]] Variant value(size_type index) const
    {
        return mData[index].value;
    }

private:
    struct Value
    {
        size_type count = 0;
        Variant value;
    };

    void insert(size_type index, size_type hash, const Variant &value)
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

    [[nodiscard]] size_type freeIndex()
    {
        if (mFree.empty())
        {
            return count();
        }

        const size_type free = mFree.back();
        mFree.pop_back();
        return free;
    }

    std::unordered_multimap<size_type, size_type> mIndex;
    std::vector<Value> mData;
    std::vector<size_type> mFree;
    size_type mCount = 0;
};
}
