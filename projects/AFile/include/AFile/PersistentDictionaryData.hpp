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
#include "PersistentVector.hpp"

#include <unordered_map>
#include <vector>

namespace afile
{
//! \cond IMPLEMENTAION_DETAIL
class PersistentDictionaryData
{
public:
    explicit PersistentDictionaryData(File *file);
    PersistentDictionaryData(File *file, acore::size_type index);
    PersistentDictionaryData(const PersistentDictionaryData &other) = delete;
    PersistentDictionaryData(PersistentDictionaryData &&other) = default;
    ~PersistentDictionaryData() = default;

    [[nodiscard]] constexpr auto capacity() const noexcept -> acore::size_type
    {
        return mPersistentData.size();
    }

    auto clear() -> void;

    [[nodiscard]] constexpr auto count() const noexcept -> acore::size_type
    {
        return mCount;
    }

    [[nodiscard]] auto count(acore::size_type index) const -> acore::size_type;

    [[nodiscard]] constexpr auto file() const noexcept -> File *
    {
        return mFile;
    }

    [[nodiscard]] constexpr auto fileIndex() const noexcept -> acore::size_type
    {
        return mPersistentData.fileIndex();
    }

    [[nodiscard]] auto indexes(acore::size_type hash) const -> std::vector<acore::size_type>;
    auto insert(acore::size_type index) -> void;
    auto insert(acore::size_type hash, const acore::Variant &value) -> acore::size_type;
    auto remove(acore::size_type index) -> void;
    auto remove(acore::size_type index, acore::size_type hash) -> void;
    [[nodiscard]] auto value(acore::size_type index) const -> acore::Variant;

    auto operator=(const PersistentDictionaryData &other) -> PersistentDictionaryData & = delete;
    auto operator=(PersistentDictionaryData &&other) noexcept -> PersistentDictionaryData & = default;

private:
    struct Index
    {
        acore::size_type valueIndex = acore::INVALID_INDEX;
        acore::size_type count = 0;
    };

    template<typename Buffer>
    friend constexpr auto operator<<(acore::DataStreamBase<Buffer> &stream, const Index &index) -> acore::DataStreamBase<Buffer> &
    {
        return stream << index.valueIndex << index.count;
    }

    template<typename Buffer>
    friend constexpr auto operator>>(acore::DataStreamBase<Buffer> &stream, Index &index) -> acore::DataStreamBase<Buffer> &
    {
        return stream >> index.valueIndex >> index.count;
    }

    [[nodiscard]] auto freeIndex() -> acore::size_type;
    auto insert(acore::size_type index, acore::size_type hash, const acore::Variant &value) -> void;
    auto insertDataIndex(acore::size_type index, acore::size_type valueIndex) -> void;
    [[nodiscard]] auto isLast(acore::size_type index) const -> bool;
    auto loadData() -> void;
    auto loadIndex(acore::size_type index, acore::size_type valueIndex) -> void;
    auto removeHashMapping(acore::size_type index, acore::size_type hash) -> void;
    auto removeDataIndex(acore::size_type index) -> void;

    File *mFile = nullptr;
    PersistentVector<Index> mPersistentData;
    std::vector<Index> mDataIndex;
    std::unordered_multimap<acore::size_type, acore::size_type> mHashToIndex;
    std::vector<acore::size_type> mFree;
    acore::size_type mCount = 0;
};
//! \endcond
}

#endif
