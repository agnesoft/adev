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

#include "FileStreamBuffer.hpp"

namespace afile
{
FileStreamBuffer::FileStreamBuffer(const char *filename) :
    mName{filename},
    mFile{filename, openMode(filename)},
    mSize{static_cast<acore::size_type>(std::filesystem::file_size(filename))}
{
}

auto FileStreamBuffer::filename() const noexcept -> const char *
{
    return mName.c_str();
}

auto FileStreamBuffer::read(acore::size_type index, char *buffer, acore::size_type count) -> void
{
    mFile.seekg(index);
    mFile.read(buffer, count);
}

auto FileStreamBuffer::write(acore::size_type index, const char *buffer, acore::size_type count) -> void
{
    mFile.seekp(index);
    mFile.write(buffer, count);
}

auto FileStreamBuffer::openMode(const std::string &filename) -> std::ios_base::openmode
{
    if (std::filesystem::exists(filename))
    {
        return static_cast<std::ios_base::openmode>(OpenMode::Binary | OpenMode::In | OpenMode::Out | OpenMode::AtEnd);
    }

    return static_cast<std::ios_base::openmode>(OpenMode::Binary | OpenMode::In | OpenMode::Out | OpenMode::Truncate);
}

auto operator|(FileStreamBuffer::OpenMode left, FileStreamBuffer::OpenMode right) noexcept -> FileStreamBuffer::OpenMode
{
    return static_cast<FileStreamBuffer::OpenMode>(static_cast<std::underlying_type<FileStreamBuffer::OpenMode>::type>(left)
                                                   | static_cast<std::underlying_type<FileStreamBuffer::OpenMode>::type>(right));
}
}
