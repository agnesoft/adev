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

#include "pch.hpp"

#include "PersistentDictionaryData.hpp"

#include <algorithm>
#include <iterator>
#include <vector>

namespace afile
{
PersistentDictionaryData::PersistentDictionaryData(File *file) :
    mFile{file},
    mPersistentData{file}
{
}

PersistentDictionaryData::PersistentDictionaryData(File *file, acore::size_type index) :
    mFile{file},
    mPersistentData{mFile, index},
    mDataIndex{mFile->value<std::vector<Index>>(index)}
{
    loadData();
}

auto PersistentDictionaryData::clear() -> void
{
    mHashToIndex.clear();
    mPersistentData.clear();
    mFree.clear();
    mCount = 0;
}

auto PersistentDictionaryData::count(acore::size_type index) const -> acore::size_type
{
    return mDataIndex[index].count;
}

auto PersistentDictionaryData::indexes(acore::size_type hash) const -> std::vector<acore::size_type>
{
    std::vector<acore::size_type> values;
    const auto range = mHashToIndex.equal_range(hash);
    values.reserve(std::distance(range.first, range.second));

    for (auto it = range.first; it != range.second; ++it)
    {
        values.push_back(it->second);
    }

    return values;
}

auto PersistentDictionaryData::insert(acore::size_type index) -> void
{
    mDataIndex[index].count++;
    mPersistentData[index] = mDataIndex[index];
}

auto PersistentDictionaryData::insert(acore::size_type hash, const acore::Variant &value) -> acore::size_type
{
    acore::size_type index = freeIndex();
    insert(index, hash, value);
    return index;
}

auto PersistentDictionaryData::remove(acore::size_type index) -> void
{
    mDataIndex[index].count--;
    mPersistentData[index] = mDataIndex[index];
}

auto PersistentDictionaryData::remove(acore::size_type index, acore::size_type hash) -> void
{
    mFile->beginWAL();
    removeHashMapping(index, hash);
    mFile->remove(mDataIndex[index].valueIndex);
    removeDataIndex(index);
    --mCount;
    mFile->endWAL();
}

auto PersistentDictionaryData::value(acore::size_type index) const -> acore::Variant
{
    return mFile->value<acore::Variant>(mDataIndex[index].valueIndex);
}

auto PersistentDictionaryData::freeIndex() -> acore::size_type
{
    if (mFree.empty())
    {
        return count();
    }

    const acore::size_type index = mFree.back();
    mFree.pop_back();
    return index;
}

auto PersistentDictionaryData::insert(acore::size_type index, acore::size_type hash, const acore::Variant &value) -> void
{
    mFile->beginWAL();
    mHashToIndex.insert({hash, index});
    const acore::size_type valueIndex = mFile->insert(value);
    insertDataIndex(index, valueIndex);
    ++mCount;
    mFile->endWAL();
}

auto PersistentDictionaryData::insertDataIndex(acore::size_type index, acore::size_type valueIndex) -> void
{
    if (index == mPersistentData.size())
    {
        mDataIndex.push_back(Index{valueIndex, 1});
        mPersistentData.push_back(mDataIndex.back());
    }
    else
    {
        mDataIndex[index] = (Index{valueIndex, 1});
        mPersistentData[index] = mDataIndex.back();
    }
}

auto PersistentDictionaryData::isLast(acore::size_type index) const -> bool
{
    return index < (static_cast<acore::size_type>(mDataIndex.size()) - 1);
}

auto PersistentDictionaryData::loadData() -> void
{
    for (acore::size_type i = 0; i < static_cast<acore::size_type>(mDataIndex.size()); ++i)
    {
        loadIndex(i, mDataIndex[i].valueIndex);
    }
}

void PersistentDictionaryData::loadIndex(acore::size_type index, acore::size_type valueIndex)
{
    if (valueIndex == acore::INVALID_INDEX)
    {
        mFree.push_back(index);
    }
    else
    {
        const auto val = mFile->value<acore::Variant>(valueIndex);
        mHashToIndex.insert({acore::dictionaryValueHash(val), index});
        ++mCount;
    }
}

auto PersistentDictionaryData::removeDataIndex(acore::size_type index) -> void
{
    if (isLast(index))
    {
        mDataIndex[index] = Index{acore::INVALID_INDEX, 0};
        mPersistentData[index] = mDataIndex[index];
        mFree.push_back(index);
    }
    else
    {
        mDataIndex.pop_back();
        mPersistentData.pop_back();
    }
}

auto PersistentDictionaryData::removeHashMapping(acore::size_type index, acore::size_type hash) -> void
{
    const auto range = mHashToIndex.equal_range(hash);
    const auto it = std::find_if(range.first, range.second, [&](const auto &val) {
        return val.second == index;
    });
    mHashToIndex.erase(it);
}
}
