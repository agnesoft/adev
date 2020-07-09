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

#ifndef AFILE_PERSISTENTDATAINDEXMAPDATA_HPP
#define AFILE_PERSISTENTDATAINDEXMAPDATA_HPP

#include "AFileModule.hpp"
#include "File.hpp"
#include "PersistentVector.hpp"

#include <vector>

namespace afile
{
class PersistentDataIndexMapData
{
public:
    explicit PersistentDataIndexMapData(File *file) :
        mFile{file},
        mDataIndex{file},
        mData{file}
    {
        mFileIndex.index = mDataIndex.fileIndex();
        mFileIndex.data = mData.fileIndex();
        mIndex = mFile->insert(mFileIndex);
    }

    PersistentDataIndexMapData(File *file, acore::size_type index) :
        mFile{file},
        mFileIndex{mFile->value<FileIndex>(index)},
        mDataIndex{file, mFileIndex.index},
        mData{file, mFileIndex.data}
    {
        //TODO count the values, build free list
    }

    PersistentDataIndexMapData(const PersistentDataIndexMapData &other) = delete;
    PersistentDataIndexMapData(PersistentDataIndexMapData &&other) noexcept = default;
    ~PersistentDataIndexMapData() = default;

    void clear()
    {
        mData.clear();
    }

    [[nodiscard]] constexpr auto count() const -> acore::size_type
    {
        return mCount;
    }

    [[nodiscard]] auto count(acore::size_type element) const -> acore::size_type
    {
        acore::size_type result = 0;
        Value val = mData[element];

        while (isValid(val))
        {
            ++result;
            val = next(val);
        }

        return result;
    }

    [[nodiscard]] constexpr auto file() const noexcept -> File *
    {
        return mFile;
    }

    [[nodiscard]] constexpr auto fileIndex() const noexcept -> acore::size_type
    {
        return mIndex;
    }

    auto insert(acore::size_type element, acore::size_type key, acore::size_type value) -> void
    {
        if (mData.size() <= element)
        {
            mData.resize(element + 1);
        }

        const auto it = find(element, key);

        if (it != mData.end())
        {
            *it = Value{{key, value}, (**it).next};
        }
        else
        {
            ++mCount;
        }
    }

    auto remove(acore::size_type element) -> void
    {
        (void)element;
        //TODO
    }

    auto remove(acore::size_type element, acore::size_type key) -> void
    {
        (void)element;
        (void)key;
        //TODO
        --mCount;
    }

    auto shrink_to_fit() -> void
    {
        mData.shrink_to_fit();
    }

    [[nodiscard]] auto size() const noexcept -> acore::size_type
    {
        return mData.size();
    }

    [[nodiscard]] auto value(acore::size_type element, acore::size_type key) const -> acore::size_type
    {
        Value val = mData[element];

        while (isValid(val) && val.element.key != key)
        {
            val = next(val);
        }

        return val.element.value;
    }

    [[nodiscard]] auto values(acore::size_type element) const -> std::vector<acore::DataIndexMapElement>
    {
        std::vector<acore::DataIndexMapElement> elements;
        Value val = mData[element];

        while (isValid(val))
        {
            elements.push_back(val.element);
            val = next(val);
        }

        return elements;
    }

    PersistentDataIndexMapData &operator=(const PersistentDataIndexMapData &other) = delete;
    PersistentDataIndexMapData &operator=(PersistentDataIndexMapData &&other) noexcept = default;

private:
    struct FileIndex
    {
        acore::size_type index = acore::INVALID_INDEX;
        acore::size_type data = acore::INVALID_INDEX;
    };

    struct Value
    {
        acore::DataIndexMapElement element;
        acore::size_type next = acore::INVALID_INDEX;
    };

    template<typename Buffer>
    friend auto operator<<(acore::DataStreamBase<Buffer> &stream, const FileIndex &index) -> acore::DataStreamBase<Buffer> &
    {
        return stream << index.index << index.data;
    }

    template<typename Buffer>
    friend auto operator>>(acore::DataStreamBase<Buffer> &stream, FileIndex &index) -> acore::DataStreamBase<Buffer> &
    {
        return stream >> index.index >> index.data;
    }

    template<typename Buffer>
    friend auto operator<<(acore::DataStreamBase<Buffer> &stream, const Value &value) -> acore::DataStreamBase<Buffer> &
    {
        return stream << value.element << value.next;
    }

    template<typename Buffer>
    friend auto operator>>(acore::DataStreamBase<Buffer> &stream, Value &value) -> acore::DataStreamBase<Buffer> &
    {
        return stream >> value.element >> value.next;
    }

    [[nodiscard]] auto find(acore::size_type element, acore::size_type key) -> PersistentVector<Value>::iterator
    {
        acore::size_type index = element;
        Value val = mData[index];

        while (isValid(val))
        {
            if (val.element.key == key)
            {
                auto it = mData.begin();
                std::advance(it, index);
                return it;
            }

            index = val.next;
            val = next(val);
        }

        return mData.end();
    }

    [[nodiscard]] auto isValid(const Value &value) const noexcept -> bool
    {
        return value.element != acore::DataIndexMapElement{} && value.next != acore::INVALID_INDEX;
    }

    [[nodiscard]] auto last(acore::size_type element) -> acore::size_type
    {
        acore::size_type index = element;
        Value val = mData[index];

        while (isValid(val) && val.next != acore::INVALID_INDEX)
        {
            index = val.next;
            val = mData[index];
        }

        return index;
    }

    [[nodiscard]] auto next(const Value &value) const -> Value
    {
        if (value.next != acore::INVALID_INDEX)
        {
            return mData[value.next];
        }

        return {};
    }

    acore::size_type mCount = 0;
    acore::size_type mFree = acore::INVALID_INDEX;
    acore::size_type mIndex = acore::INVALID_INDEX;
    File *mFile = nullptr;
    FileIndex mFileIndex;
    PersistentVector<acore::size_type> mDataIndex;
    PersistentVector<Value> mData;
};
}

#endif
