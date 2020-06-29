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

#ifndef AFILE_FILEDATA_HPP
#define AFILE_FILEDATA_HPP

#include "AFileModule.hpp"
#include "FileStream.hpp"

#include <algorithm>
#include <vector>

namespace afile
{
//! \cond IMPLEMENTAION_DETAIL
class FileData
{
public:
    explicit FileData(const char *filename);

    auto append(acore::size_type remaining) -> void;
    auto beginWrite(acore::size_type offset) -> acore::DataStream &;
    [[nodiscard]] auto bufferSize() const noexcept -> acore::size_type;
    auto clear() -> void;
    auto endWrite(acore::size_type pos) -> void;
    [[nodiscard]] auto file() const noexcept -> FileStream &;
    [[nodiscard]] auto filename() const noexcept -> const char *;
    auto move(acore::size_type pos, acore::size_type offset, acore::size_type newOffset, acore::size_type size) -> void;
    auto moveData(acore::size_type to, acore::size_type from, acore::size_type remainingSize) -> void;
    [[nodiscard]] auto offset() const noexcept -> acore::size_type;
    [[nodiscard]] auto pos() const noexcept -> acore::size_type;
    [[nodiscard]] auto read(acore::size_type readPos, acore::size_type remainingSize) -> std::vector<char>;
    auto resize(acore::size_type newSize) -> void;
    auto seek(acore::size_type pos) const -> void;
    [[nodiscard]] auto size() const noexcept -> acore::size_type;
    auto write(acore::size_type pos, const std::vector<char> &data) -> void;

private:
    [[nodiscard]] static auto emptyData(acore::size_type size) -> std::vector<char>;
    auto resetBuffer() -> void;

    static constexpr acore::size_type BUFFER_SIZE = 16384;
    static constexpr acore::size_type MAX_STEP_SIZE = 2147483648;
    acore::size_type mOffset = 0;
    acore::DataStream mBufferStream;
    mutable FileStream mFile;
};
//! \endcond
}

#endif
