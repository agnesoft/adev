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

namespace afile
{
PersistentDictionaryData::PersistentDictionaryData(File *file) :
    mFile{file},
    mPersistentData{file}
{
}

PersistentDictionaryData::PersistentDictionaryData(File *file, acore::size_type index) :
    mFile{file},
    mPersistentData{mFile, index}
{
    loadData();
}

auto PersistentDictionaryData::indexes(acore::size_type hash) const -> std::vector<acore::size_type>
{
    std::vector<acore::size_type> values;
    const auto range = mIndexData.equal_range(hash);
    values.reserve(std::distance(range.first, range.second));

    for (auto it = range.first; it != range.second; ++it)
    {
        values.push_back(it->second);
    }

    return values;
}

void PersistentDictionaryData::remove(acore::size_type index, acore::size_type hash)
{
    mFile->beginWAL();

    const auto range = mIndexData.equal_range(hash);
    for (auto it = range.first; it != range.second; ++it)
    {
        if (it->second == index)
        {
            mIndexData.erase(it);
            break;
        }
    }

    mFile->remove(mPersistentData[index]);

    if (index < (mPersistentData.size() - 1))
    {
        mPersistentData[index] = acore::INVALID_INDEX;
        mFree.push_back(index);
    }
    else
    {
        mPersistentData.pop_back();
    }

    mCount--;
    mFile->endWAL();
}

acore::Variant PersistentDictionaryData::value(acore::size_type index) const
{
    return mFile->value<acore::Variant>(mPersistentData[index], sizeof(acore::size_type));
}

acore::size_type PersistentDictionaryData::insert(acore::size_type hash, const acore::Variant &value)
{
    acore::size_type index = freeIndex();
    insert(index, hash, value);
    return index;
}

void PersistentDictionaryData::insert(acore::size_type index, acore::size_type hash, const acore::Variant &value)
{
    mFile->beginWAL();
    mIndexData.insert({hash, index});

    if (index == mPersistentData.size())
    {
        mPersistentData.push_back(mFile->insert(Value{1, value}));
    }
    else
    {
        mPersistentData[index] = mFile->insert(Value{1, value});
    }

    mCount++;
    mFile->endWAL();
}

[[nodiscard]] auto doHash(const acore::Variant &value) -> acore::size_type
{
    const std::vector<char> &val{value.value<const std::vector<char> &>()};
    return static_cast<acore::size_type>(std::hash<std::string_view>{}(std::string_view(val.data(), static_cast<std::size_t>(val.size()))));
}

auto PersistentDictionaryData::loadData() -> void
{
    for (acore::size_type i = 0; i < mPersistentData.size(); ++i)
    {
        const acore::size_type index = mPersistentData[i];

        if (index == acore::INVALID_INDEX)
        {
            mFree.push_back(i);
        }
        else
        {
            const acore::Variant val = mFile->value<acore::Variant>(index, static_cast<acore::size_type>(sizeof(acore::size_type)));
            mIndexData.insert({doHash(val), i});
        }
    }
}
}
