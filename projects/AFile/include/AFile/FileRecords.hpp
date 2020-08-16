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

#include "FileData.hpp"

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

    explicit FileRecords(FileData *data);

    auto clear() -> void;
    [[nodiscard]] auto contains(acore::size_type index) const noexcept -> bool;
    [[nodiscard]] auto count() const noexcept -> acore::size_type;
    [[nodiscard]] auto endPos(acore::size_type index) const noexcept -> acore::size_type;
    [[nodiscard]] auto indexes() const -> std::vector<acore::size_type>;
    auto invalidate(acore::size_type index) -> void;
    [[nodiscard]] auto isLast(acore::size_type index) const -> bool;
    [[nodiscard]] auto isValid(acore::size_type index) const noexcept -> bool;
    [[nodiscard]] auto newIndex() -> acore::size_type;
    [[nodiscard]] auto pos(acore::size_type index) const noexcept -> acore::size_type;
    [[nodiscard]] auto recordPos(acore::size_type index) const noexcept -> acore::size_type;
    auto remove(acore::size_type recordIndex) -> void;
    auto set(acore::size_type index, Index record) -> void;
    [[nodiscard]] auto size(acore::size_type index) const noexcept -> acore::size_type;
    [[nodiscard]] auto sortedIndexes() -> std::vector<acore::size_type>;
    auto update(Index index) -> void;
    auto update(acore::size_type pos, Index index) -> void;

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
    [[nodiscard]] static auto isValid(Index index) noexcept -> bool;
    auto loadIndex() -> void;
    auto loadRecords() -> void;
    [[nodiscard]] static auto logicalRecordPos(acore::size_type pos) noexcept -> acore::size_type;
    [[nodiscard]] auto processIndex(Index index) -> acore::size_type;
    [[nodiscard]] static auto recordEnd(Index index) -> acore::size_type;
    [[nodiscard]] auto recordIndex(const Index *idx) const noexcept -> acore::size_type;
    auto saveRecordsCount() -> void;
    auto updateRemovedIndex(acore::size_type recordIndex) -> void;

    acore::size_type mCount = 0;
    acore::size_type mFreeIndex = acore::INVALID_INDEX;
    FileData *mData = nullptr;
    std::vector<Index> mRecords;
};
//! \endcond
}

#endif
