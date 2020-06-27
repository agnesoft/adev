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

#ifndef AFILE_FILESTREAMBUFFER_HPP
#define AFILE_FILESTREAMBUFFER_HPP

#include "AFileModule.hpp"

#include <filesystem>
#include <fstream>
#include <string>

namespace afile
{
//! \cond IMPLEMENTAION_DETAIL
class FileStreamBuffer
{
public:
    enum class OpenMode : size_t
    {
        Binary = std::ios_base::binary,
        In = std::ios_base::in,
        Out = std::ios_base::out,
        AtEnd = std::ios_base::ate,
        Truncate = std::ios_base::trunc
    };

    explicit FileStreamBuffer(const char *filename);
    FileStreamBuffer(const FileStreamBuffer &other) = delete;
    FileStreamBuffer(FileStreamBuffer &&other) = default;

    [[nodiscard]] auto filename() const noexcept -> const char *;
    auto read(acore::size_type index, char *buffer, acore::size_type count) -> void;

    constexpr auto resize(acore::size_type size) noexcept -> void
    {
        mSize = size;
    }

    [[nodiscard]] constexpr auto size() const noexcept -> acore::size_type
    {
        return mSize;
    }

    auto write(acore::size_type index, const char *buffer, acore::size_type count) -> void;

    auto operator=(const FileStreamBuffer &other) -> FileStreamBuffer & = delete;
    auto operator=(FileStreamBuffer &&other) -> FileStreamBuffer & = default;

private:
    [[nodiscard]] static std::ios_base::openmode openMode(const std::string &filename);

    std::string mName;
    std::fstream mFile;
    acore::size_type mSize = 0;
};

[[nodiscard]] FileStreamBuffer::OpenMode operator|(FileStreamBuffer::OpenMode left, FileStreamBuffer::OpenMode right) noexcept;
//! \endcond
}

#endif
