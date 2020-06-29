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

#ifndef AFILE_FILE_RECORDS_HPP
#define AFILE_FILE_RECORDS_HPP

#include "AFileModule.hpp"
#include "FileData.hpp"
#include "FileStream.hpp"
#include "WAL.hpp"

#include <vector>

namespace afile
{
//! \cond IMPLEMENTAION_DETAIL
class FileRecords
{
public:
    struct Index
    {
        acore::size_type pos = acore::INVALID_INDEX;
        acore::size_type size = acore::INVALID_INDEX;
    };

    explicit FileRecords(FileData *data, WAL *wal);

    auto clear() -> void;
    [[nodiscard]] auto contains(acore::size_type index) const -> bool;
    [[nodiscard]] auto count() const noexcept -> acore::size_type;
    [[nodiscard]] auto endPos(acore::size_type index) const noexcept -> acore::size_type;
    [[nodiscard]] auto indexes() const -> std::vector<acore::size_type>;
    auto invalidateIndex(acore::size_type idx) -> void;
    [[nodiscard]] auto isLast(acore::size_type idx) const -> bool;
    [[nodiscard]] auto isValid(acore::size_type idx) const noexcept -> bool;
    [[nodiscard]] static auto isValid(Index idx) noexcept -> bool;
    [[nodiscard]] auto newIndex() -> acore::size_type;
    [[nodiscard]] auto pos(acore::size_type index) const noexcept -> acore::size_type;
    [[nodiscard]] auto recordPos(acore::size_type index) const noexcept -> acore::size_type;
    [[nodiscard]] auto recordEnd(acore::size_type index) const noexcept -> acore::size_type;
    auto remove(acore::size_type recordIndex) -> void;
    auto setRecord(acore::size_type index, Index record) -> void;
    auto setSize(acore::size_type index, acore::size_type size) -> void;
    [[nodiscard]] auto size(acore::size_type index) const noexcept -> acore::size_type;
    [[nodiscard]] auto sortedIndexes() -> std::vector<acore::size_type>;
    auto updateIndex(FileRecords::Index index) -> void;
    auto updateIndex(acore::size_type pos, FileRecords::Index index) -> void;

private:
    template<typename Buffer>
    friend constexpr auto operator<<(acore::DataStreamBase<Buffer> &stream, const Index &recordIndex) -> acore::DataStreamBase<Buffer> &
    {
        return stream << recordIndex.pos << recordIndex.size;
    }

    template<typename Buffer>
    friend constexpr auto operator>>(acore::DataStreamBase<Buffer> &stream, Index &recordIndex) -> acore::DataStreamBase<Buffer> &
    {
        return stream >> recordIndex.pos >> recordIndex.size;
    }

    auto buildFreeList() -> void;
    auto createIndex() -> void;
    auto initialize() -> void;
    auto loadIndex() -> void;
    auto loadRecords() -> void;
    [[nodiscard]] static auto logicalRecordPos(acore::size_type pos) noexcept -> acore::size_type;
    auto processIndex(Index index) -> void;
    [[nodiscard]] auto recordEnd(Index index) -> acore::size_type;
    [[nodiscard]] auto readIndex() -> Index;
    [[nodiscard]] auto recordIndex(const Index *idx) const noexcept -> acore::size_type;
    auto saveRecordsCount(acore::size_type count) -> void;
    auto updateRemovedIndex(acore::size_type recordIndex) -> void;

    acore::size_type mCount = 0;
    acore::size_type mFreeIndex = acore::INVALID_INDEX;
    FileData *mData = nullptr;
    WAL *mWAL = nullptr;
    std::vector<Index> mRecords;
};
//! \endcond
}

#endif
