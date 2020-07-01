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
    mFile(file),
    mIndexData(file),
    mData(file)
{
    mIndex = mFile->insert(FileIndex{mIndexData.fileIndex(), mData.fileIndex()});
}

PersistentDictionaryData::PersistentDictionaryData(File *file, acore::size_type index) :
    mFile(file),
    mIndex(index),
    mIdx(mFile->value<FileIndex>(mIndex)),
    mIndexData(mFile, mIdx.index),
    mData(mFile, mIdx.data),
    mCount(mIndexData.count())
{
    for (acore::size_type i = 0; i < mData.count(); i++)
    {
        if (mData[i] == acore::INVALID_INDEX)
        {
            mFree.push_back(i);
        }
    }
}

auto PersistentDictionaryData::indexes(acore::size_type hash) const -> std::vector<acore::size_type>
{
    return mIndexData.values(hash);
}

void PersistentDictionaryData::remove(acore::size_type index, acore::size_type hash)
{
    mFile->beginWAL();
    mIndexData.remove(hash, index);
    mFile->remove(mData[index]);

    if (index < (mData.size() - 1))
    {
        mData[index] = acore::INVALID_INDEX;
        mFree.push_back(index);
    }
    else
    {
        mData.removeLast();
    }

    mCount--;
    mFile->endWAL();
}

acore::Variant PersistentDictionaryData::value(acore::size_type index) const
{
    return mFile->value<acore::Variant>(mData[index], sizeof(acore::size_type));
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
    mIndexData.insert(hash, index);

    if (index == mData.size())
    {
        mData.append(mFile->insert(Value{1, value}));
    }
    else
    {
        mData[index] = mFile->insert(Value{1, value});
    }

    mCount++;
    mFile->endWAL();
}
}
