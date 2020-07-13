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

#include <utility>
#include <vector>

namespace afile
{
//! \cond IMPLEMENTAION_DETAIL
class PersistentDataIndexMapData
{
public:
    explicit PersistentDataIndexMapData(File *file);
    PersistentDataIndexMapData(File *file, acore::size_type index);

    PersistentDataIndexMapData(const PersistentDataIndexMapData &other) = delete;
    PersistentDataIndexMapData(PersistentDataIndexMapData &&other) noexcept = default;
    ~PersistentDataIndexMapData() = default;

    auto clear() -> void;

    [[nodiscard]] constexpr auto count() const -> acore::size_type
    {
        return mIndex.count;
    }

    [[nodiscard]] auto count(acore::size_type element) const -> acore::size_type;

    [[nodiscard]] constexpr auto file() const noexcept -> File *
    {
        return mFile;
    }

    [[nodiscard]] constexpr auto fileIndex() const noexcept -> acore::size_type
    {
        return mFileIndex;
    }

    auto insert(acore::size_type element, acore::size_type key, acore::size_type value) -> void;
    auto remove(acore::size_type element) -> void;
    auto remove(acore::size_type element, acore::size_type key) -> void;
    auto shrink_to_fit() -> void;

    [[nodiscard]] auto size() const noexcept -> acore::size_type
    {
        return mDataIndex.size();
    }

    [[nodiscard]] auto value(acore::size_type element, acore::size_type key) const -> acore::size_type;
    [[nodiscard]] auto values(acore::size_type element) const -> std::vector<acore::DataIndexMapElement>;

    auto operator=(const PersistentDataIndexMapData &other) -> PersistentDataIndexMapData & = delete;
    auto operator=(PersistentDataIndexMapData &&other) noexcept -> PersistentDataIndexMapData & = default;

private:
    struct FileIndex
    {
        acore::size_type index = acore::INVALID_INDEX;
        acore::size_type data = acore::INVALID_INDEX;
        acore::size_type free = acore::INVALID_INDEX;
        acore::size_type count = 0;
    };

    struct Value
    {
        acore::DataIndexMapElement element;
        acore::size_type next = acore::INVALID_INDEX;
    };

    template<typename Buffer>
    friend auto operator<<(acore::DataStreamBase<Buffer> &stream, const FileIndex &index) -> acore::DataStreamBase<Buffer> &
    {
        return stream << index.index << index.data << index.free << index.count;
    }

    template<typename Buffer>
    friend auto operator>>(acore::DataStreamBase<Buffer> &stream, FileIndex &index) -> acore::DataStreamBase<Buffer> &
    {
        return stream >> index.index >> index.data >> index.free >> index.count;
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

    [[nodiscard]] static auto beginInit(File *file) -> File *;
    [[nodiscard]] auto countToLast(acore::size_type index) const -> std::pair<acore::size_type, acore::size_type>;
    auto decrementCount(acore::size_type count) -> void;
    auto decrementCount() -> void;
    static auto endInit(File *file) -> void;
    [[nodiscard]] auto find(acore::size_type element, acore::size_type key) const -> acore::size_type;
    [[nodiscard]] auto freeIndex() -> acore::size_type;
    auto incrementCount() -> void;
    auto insertExisting(acore::size_type element, acore::size_type key, acore::size_type value) -> void;
    auto insertNew(acore::size_type element, acore::size_type key, acore::size_type value) -> void;
    [[nodiscard]] auto insertValue(acore::DataIndexMapElement element, acore::size_type next) -> acore::size_type;
    auto removeValue(acore::size_type element, const Value &value, acore::size_type dataIndex, acore::size_type previousIndex) -> void;
    auto removeValues(acore::size_type element, acore::size_type dataIndex, acore::size_type last, acore::size_type count) -> void;
    auto updateFree(acore::size_type value) -> void;

    File *mFile = nullptr;
    acore::size_type mFileIndex = acore::INVALID_INDEX;
    FileIndex mIndex;
    PersistentVector<acore::size_type> mDataIndex;
    PersistentVector<Value> mData;
};
//! \endcond
}

#endif
