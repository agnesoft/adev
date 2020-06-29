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

#include "FileData.hpp"

#include <algorithm>
#include <string>
#include <vector>

namespace afile
{
FileData::FileData(const char *filename) :
    mFile{filename}
{
    if (mFile.buffer().isOpen())
    {
        mBufferStream.buffer().data().reserve(BUFFER_SIZE);
    }
    else
    {
        throw acore::Exception() << "Could not open file '." << std::string{mFile.buffer().filename()} << '\'';
    }
}

auto FileData::append(acore::size_type remaining) -> void
{
    while (remaining > 0)
    {
        const std::vector<char> data = emptyData(remaining);
        remaining -= data.size();
        write(size(), data);
    }
}

auto FileData::beginWrite(acore::size_type offset) -> acore::DataStream &
{
    mOffset = offset;
    return mBufferStream;
}

auto FileData::bufferSize() const noexcept -> acore::size_type
{
    return mOffset + mBufferStream.buffer().size();
}

auto FileData::clear() -> void
{
    mFile.buffer().resize(static_cast<acore::size_type>(sizeof(acore::size_type)));
    mFile.reset();
    mFile << acore::size_type(0);
}

void FileData::endWrite(acore::size_type pos)
{
    write(pos + mOffset, mBufferStream.buffer().data());
    resetBuffer();
}

auto FileData::file() const noexcept -> FileStream &
{
    return mFile;
}

auto FileData::filename() const noexcept -> const char *
{
    return mFile.buffer().filename();
}

auto FileData::move(acore::size_type pos, acore::size_type offset, acore::size_type newOffset, acore::size_type size) -> void
{
    mOffset = newOffset;
    mBufferStream.buffer().resize(std::max(mBufferStream.buffer().size(), size));
    mFile.seek(pos + offset);
    mFile.read(mBufferStream.buffer().data().data(), size);
}

auto FileData::moveData(acore::size_type to, acore::size_type from, acore::size_type remainingSize) -> void
{
    while (remainingSize > 0)
    {
        const std::vector<char> data = read(from, remainingSize);
        from = pos();
        remainingSize -= data.size();
        write(to, data);
        to = pos();
    }
}

auto FileData::offset() const noexcept -> acore::size_type
{
    return mOffset;
}

auto FileData::pos() const noexcept -> acore::size_type
{
    return mFile.pos();
}

auto FileData::read(acore::size_type readPos, acore::size_type remainingSize) -> std::vector<char>
{
    mFile.seek(readPos);
    std::vector<char> data(std::min(MAX_STEP_SIZE, remainingSize));
    mFile.read(data.data(), data.size());
    return data;
}

auto FileData::resize(acore::size_type newSize) -> void
{
    mFile.buffer().resize(newSize);
}

auto FileData::seek(acore::size_type pos) const -> void
{
    mFile.seek(pos);
}

auto FileData::size() const noexcept -> acore::size_type
{
    return mFile.buffer().size();
}

auto FileData::write(acore::size_type pos, const std::vector<char> &data) -> void
{
    mFile.seek(pos);
    mFile.write(data.data(), data.size());
}

auto FileData::emptyData(acore::size_type size) -> std::vector<char>
{
    return std::vector<char>(std::min(MAX_STEP_SIZE, size));
}

auto FileData::resetBuffer() -> void
{
    if (mBufferStream.buffer().size() > BUFFER_SIZE)
    {
        mBufferStream.buffer().data().clear();
        mBufferStream.buffer().data().reserve(BUFFER_SIZE);
    }

    mBufferStream.buffer().resize(0);
    mBufferStream.reset();
}
}
