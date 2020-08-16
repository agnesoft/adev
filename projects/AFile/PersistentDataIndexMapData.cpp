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

#include "PersistentDataIndexMapData.hpp"

namespace afile
{
PersistentDataIndexMapData::PersistentDataIndexMapData(File *file) :
    mFile{beginInit(file)},
    mDataIndex{file},
    mData{file}
{
    mIndex.index = mDataIndex.fileIndex();
    mIndex.data = mData.fileIndex();
    mFileIndex = mFile->insert(mIndex);
    endInit(file);
}

PersistentDataIndexMapData::PersistentDataIndexMapData(File *file, acore::size_type index) :
    mFile{file},
    mFileIndex{index},
    mIndex{mFile->value<FileIndex>(index)},
    mDataIndex{file, mIndex.index},
    mData{file, mIndex.data}
{
}

auto PersistentDataIndexMapData::clear() -> void
{
    mDataIndex.clear();
    mData.clear();
    mIndex.free = acore::INVALID_INDEX;
    mIndex.count = 0;
    mFile->insert(mFileIndex, 0, mIndex);
}

auto PersistentDataIndexMapData::count(acore::size_type element) const -> acore::size_type
{
    acore::size_type result = 0;
    acore::size_type dataIndex = mDataIndex[element];

    while (dataIndex != acore::INVALID_INDEX)
    {
        ++result;
        dataIndex = (*mData[dataIndex]).next;
    }

    return result;
}

auto PersistentDataIndexMapData::insert(acore::size_type element, acore::size_type key, acore::size_type value) -> void
{
    mFile->beginWAL();

    if (mDataIndex.size() <= element)
    {
        insertNew(element, key, value);
    }
    else
    {
        insertExisting(element, key, value);
    }

    mFile->endWAL();
}

auto PersistentDataIndexMapData::remove(acore::size_type element) -> void
{
    const acore::size_type index = mDataIndex[element];

    if (index == acore::INVALID_INDEX)
    {
        return;
    }

    const auto last = countToLast(index);
    removeValues(element, index, last.first, last.second);
}

auto PersistentDataIndexMapData::remove(acore::size_type element, acore::size_type key) -> void
{
    acore::size_type dataIndex = mDataIndex[element];
    acore::size_type previousIndex = acore::INVALID_INDEX;

    while (dataIndex != acore::INVALID_INDEX)
    {
        const Value value = mData[dataIndex];

        if (value.element.key == key)
        {
            removeValue(element, value, dataIndex, previousIndex);
            return;
        }

        previousIndex = dataIndex;
        dataIndex = value.next;
    }
}

auto PersistentDataIndexMapData::shrink_to_fit() -> void
{
    acore::size_type i = mDataIndex.size();

    while (i > 0 && mDataIndex[i - 1] == acore::INVALID_INDEX)
    {
        --i;
    }

    mDataIndex.resize(i);
}

auto PersistentDataIndexMapData::value(acore::size_type element, acore::size_type key) const -> acore::size_type
{
    const acore::size_type index = find(element, key);

    if (index != acore::INVALID_INDEX)
    {
        return (*mData[index]).element.value;
    }

    throw acore::Exception{} << "Element-key pair '" << element << "' : '" << key << "' must be in the map.";
}

auto PersistentDataIndexMapData::values(acore::size_type element) const -> std::vector<acore::DataIndexMapElement>
{
    std::vector<acore::DataIndexMapElement> elements;
    acore::size_type dataIndex = mDataIndex[element];

    while (dataIndex != acore::INVALID_INDEX)
    {
        const Value value = mData[dataIndex];
        elements.push_back(value.element);
        dataIndex = value.next;
    }

    return elements;
}

auto PersistentDataIndexMapData::beginInit(File *file) -> File *
{
    file->beginWAL();
    return file;
}

auto PersistentDataIndexMapData::countToLast(acore::size_type index) const -> std::pair<acore::size_type, acore::size_type>
{
    std::pair<acore::size_type, acore::size_type> last{index, 0};
    acore::size_type next = index;

    while (next != acore::INVALID_INDEX)
    {
        last.first = next;
        ++last.second;
        next = (*mData[next]).next;
    }

    return last;
}

auto PersistentDataIndexMapData::decrementCount(acore::size_type count) -> void
{
    mIndex.count -= count;
    mFile->insert(mFileIndex, 0, mIndex);
}

auto PersistentDataIndexMapData::decrementCount() -> void
{
    decrementCount(1);
}

auto PersistentDataIndexMapData::endInit(File *file) -> void
{
    file->endWAL();
}

auto PersistentDataIndexMapData::find(acore::size_type element, acore::size_type key) const -> acore::size_type
{
    acore::size_type dataIndex = mDataIndex[element];

    while (dataIndex != acore::INVALID_INDEX)
    {
        const Value value = mData[dataIndex];

        if (value.element.key == key)
        {
            return dataIndex;
        }

        dataIndex = value.next;
    }

    return dataIndex;
}

auto PersistentDataIndexMapData::freeIndex() -> acore::size_type
{
    if (mIndex.free == acore::INVALID_INDEX)
    {
        return mData.size();
    }

    const acore::size_type index = mIndex.free;
    updateFree((*mData[index]).next);
    return index;
}

auto PersistentDataIndexMapData::incrementCount() -> void
{
    ++mIndex.count;
    mFile->insert(mFileIndex, 0, mIndex);
}

auto PersistentDataIndexMapData::insertExisting(acore::size_type element, acore::size_type key, acore::size_type value) -> void
{
    const acore::size_type index = find(element, key);

    if (index == acore::INVALID_INDEX)
    {
        mDataIndex[element] = insertValue(acore::DataIndexMapElement{key, value}, mDataIndex[element]);
        incrementCount();
    }
    else
    {
        mData[index] = Value{{key, value}, (*mData[index]).next};
    }
}

auto PersistentDataIndexMapData::insertNew(acore::size_type element, acore::size_type key, acore::size_type value) -> void
{
    mDataIndex.resize(element + 1, acore::INVALID_INDEX);
    mDataIndex[element] = insertValue(acore::DataIndexMapElement{key, value}, acore::INVALID_INDEX);
    incrementCount();
}

auto PersistentDataIndexMapData::insertValue(acore::DataIndexMapElement element, acore::size_type next) -> acore::size_type
{
    const acore::size_type index = freeIndex();

    if (index == mData.size())
    {
        mData.push_back(Value{element, next});
    }
    else
    {
        mData[index] = Value{element, next};
    }

    return index;
}

auto PersistentDataIndexMapData::removeValue(acore::size_type element, const PersistentDataIndexMapData::Value &value, acore::size_type dataIndex, acore::size_type previousIndex) -> void
{
    mFile->beginWAL();

    if (previousIndex != acore::INVALID_INDEX)
    {
        mData[previousIndex] = Value{(*mData[previousIndex]).element, value.next};
    }
    else
    {
        mDataIndex[element] = value.next;
    }

    mData[dataIndex] = Value{{}, mIndex.free};
    updateFree(dataIndex);
    decrementCount();
    mFile->endWAL();
}

auto PersistentDataIndexMapData::removeValues(acore::size_type element, acore::size_type dataIndex, acore::size_type last, acore::size_type count) -> void
{
    mFile->beginWAL();
    mData[last] = Value{{}, mIndex.free};
    updateFree(dataIndex);
    mDataIndex[element] = acore::INVALID_INDEX;
    decrementCount(count);
    mFile->endWAL();
}

auto PersistentDataIndexMapData::updateFree(acore::size_type value) -> void
{
    mIndex.free = value;
    mFile->insert(mFileIndex, 0, mIndex);
}
}
