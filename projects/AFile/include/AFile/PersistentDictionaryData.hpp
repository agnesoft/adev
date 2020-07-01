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

#ifndef AFILE_AFILEPERSISTENTDICTIONARYDATA_HPP
#define AFILE_AFILEPERSISTENTDICTIONARYDATA_HPP

#include "AFileModule.hpp"
#include "File.hpp"

#include <unordered_map>
#include <vector>

namespace afile
{
class PersistentDictionaryData
{
public:
    struct FileIndex
    {
        acore::size_type index = acore::INVALID_INDEX;
        acore::size_type data = acore::INVALID_INDEX;
    };

    struct Value
    {
        acore::size_type count = 0;
        acore::Variant value;
    };

    explicit PersistentDictionaryData(File *file);
    PersistentDictionaryData(File *file, acore::size_type index);
    PersistentDictionaryData(const PersistentDictionaryData &other) = delete;
    PersistentDictionaryData(PersistentDictionaryData &&other) noexcept = default;
    ~PersistentDictionaryData() noexcept = default;

    [[nodiscard]] constexpr acore::size_type capacity() const noexcept
    {
        return mData.count();
    }

    constexpr void clear()
    {
        mIndexData.clear();
        mData.clear();
        mFree.clear();
        mCount = 0;
    }

    [[nodiscard]] constexpr acore::size_type count() const noexcept
    {
        return mCount;
    }

    [[nodiscard]] constexpr acore::size_type count(acore::size_type index) const
    {
        return mFile->value<acore::size_type>(mData[index]);
    }

    [[nodiscard]] constexpr File *file() const noexcept
    {
        return mFile;
    }

    [[nodiscard]] constexpr acore::size_type fileIndex() const noexcept
    {
        return mIndex;
    }

    [[nodiscard]] std::vector<acore::size_type> indexes(acore::size_type hash) const;
    acore::size_type insert(acore::size_type hash, const acore::Variant &value);

    constexpr void insert(acore::size_type index)
    {
        const acore::size_type fileIndex = mData[index];
        const auto valueCount = mFile->value<acore::size_type>(fileIndex);
        mFile->insert(fileIndex, 0, valueCount + 1);
    }

    void remove(acore::size_type index, acore::size_type hash);

    constexpr void remove(acore::size_type index)
    {
        const acore::size_type idx = mData[index];
        const auto count = mFile->value<acore::size_type>(idx);
        mFile->insert(idx, 0, count - 1);
    }

    [[nodiscard]] acore::Variant value(acore::size_type index) const;

    PersistentDictionaryData &operator=(const PersistentDictionaryData &other) = delete;
    PersistentDictionaryData &operator=(PersistentDictionaryData &&other) noexcept = default;

private:
    void insert(acore::size_type index, acore::size_type hash, const acore::Variant &value);

    [[nodiscard]] constexpr acore::size_type freeIndex()
    {
        return mFree.empty() ? count() : mFree.takeLast();
    }

    File *mFile = nullptr;
    acore::size_type mIndex = acore::INVALID_INDEX;
    FileIndex mIdx;
    std::unordered_map<acore::size_type, acore::size_type> mIndexData;
    PersistentVector<acore::size_type> mData;
    std::vector<acore::size_type> mFree;
    acore::size_type mCount = 0;
};

template<typename Buffer>
constexpr acore::DataStreamBase<Buffer> &operator<<(acore::DataStreamBase<Buffer> &stream, const PersistentDictionaryData::FileIndex &index)
{
    return stream << index.index << index.data;
}

template<typename Buffer>
constexpr acore::DataStreamBase<Buffer> &operator>>(acore::DataStreamBase<Buffer> &stream, PersistentDictionaryData::FileIndex &index)
{
    return stream >> index.index >> index.data;
}

template<typename Buffer>
constexpr acore::DataStreamBase<Buffer> &operator<<(acore::DataStreamBase<Buffer> &stream, const PersistentDictionaryData::Value &value)
{
    return stream << value.count << value.value;
}
}

#endif
