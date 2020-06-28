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

#include "File.hpp"

#include <filesystem>
#include <iostream>
#include <numeric>

namespace afile
{

File::File(const char *filename) :
    mFile{validateFileNotInUse(filename)},
    mWAL{&mFile},
    mRecords{&mFile, &mWAL}
{
    mBufferStream.buffer().data().reserve(BUFFER_SIZE);
    File::filesInUse()->emplace_back(filename);
}

File::~File() //NOLINT(bugprone-exception-escape)
{
    const auto it = std::find(File::filesInUse()->begin(), File::filesInUse()->end(), mFile.buffer().filename());

    if (it != File::filesInUse()->end())
    {
        File::filesInUse()->erase(it);
    }
}

auto File::beginWAL() noexcept -> void
{
    mWAL.begin();
}

auto File::clear() -> void
{
    mWAL.reset();
    mRecords.clear();
    mFile.buffer().resize(static_cast<acore::size_type>(sizeof(acore::size_type)));
    mFile.reset();
    mFile << acore::size_type(0);
}

auto File::contains(acore::size_type index) const -> bool
{
    return mRecords.contains(index);
}

auto File::count() const noexcept -> acore::size_type
{
    return mRecords.count();
}

auto File::endWAL() -> void
{
    mWAL.end();
}

auto File::filename() const noexcept -> const char *
{
    return mFile.buffer().filename();
}

auto File::indexes() const -> std::vector<acore::size_type>
{
    return mRecords.indexes();
}

auto File::isEmpty() const noexcept -> bool
{
    return mRecords.count() == 0;
}

auto File::move(acore::size_type index, acore::size_type offset, acore::size_type newOffset, acore::size_type size) -> void
{
    validateMoveInput(index, offset, newOffset, size);
    mOffset = newOffset;
    mBufferStream.buffer().resize(std::max(mBufferStream.buffer().size(), size));
    mFile.seek(mRecords.pos(index) + offset);
    mFile.read(mBufferStream.buffer().data().data(), size);
    endWrite(index);
}

auto File::optimize() -> void
{
    mWAL.reset();
    mFile.seek(static_cast<acore::size_type>(sizeof(acore::size_type)));

    for (acore::size_type idx : mRecords.sortedIndexes())
    {
        optimizeRecord(idx);
    }

    resize(mFile.pos());
    mWAL.reset();
}

auto File::remove(acore::size_type index) -> void
{
    if (contains(index))
    {
        removeData(index);
        mRecords.remove(index);
        mWAL.reset();
    }
}

auto File::resize(acore::size_type index, acore::size_type size) -> void
{
    validateIndex(index);
    validateSize(size);

    if (mRecords.size(index) != size)
    {
        resizeAt(index, size);
        mWAL.reset();
    }
}

auto File::size() const noexcept -> acore::size_type
{
    return mFile.buffer().size();
}

auto File::size(acore::size_type index) const -> acore::size_type
{
    if (contains(index))
    {
        return mRecords.size(index);
    }

    return 0;
}

auto File::append(acore::size_type remaining) -> void
{
    while (remaining > 0)
    {
        const std::vector<char> data = emptyValue(remaining);
        remaining -= data.size();
        write(mFile.buffer().size(), data);
    }
}

auto File::beginRead(acore::size_type index, acore::size_type offset) const -> FileStream &
{
    validateReadInput(index, offset);
    mFile.seek(mRecords.pos(index) + offset);
    return mFile;
}

auto File::beginWrite(acore::size_type index, acore::size_type offset) -> acore::DataStream &
{
    validateWriteInput(index, offset);
    mOffset = offset;
    return mBufferStream;
}

auto afile::File::bufferExtendsValue(acore::size_type index, acore::size_type offset) const -> bool
{
    return (offset + mBufferStream.buffer().size()) > mRecords.size(index);
}

auto File::emptyValue(acore::size_type size) -> std::vector<char>
{
    return std::vector<char>(std::min(MAX_STEP_SIZE, size));
}

auto File::endRead(acore::size_type index) const -> void
{
    mRecords.validatePos(index, mFile.pos());
}

auto File::endWrite(acore::size_type index) -> void
{
    if (bufferExtendsValue(index, mOffset))
    {
        if (!mRecords.isLast(index))
        {
            moveRecordToEnd(index, mOffset);
        }

        mRecords.setSize(index, mOffset + mBufferStream.buffer().size());
        mRecords.updateIndex(FileRecords::Index{index, mRecords.size(index)});
    }

    write(mRecords.pos(index) + mOffset, mBufferStream.buffer().data());
    resetBuffer();
    mWAL.reset();
}

auto File::filesInUse() -> std::vector<std::filesystem::path> *
{
    static auto *files = std::make_unique<std::vector<std::filesystem::path>>().release();
    return files;
}

auto File::moveData(acore::size_type to, acore::size_type from, acore::size_type remainingSize) -> void
{
    while (remainingSize > 0)
    {
        const std::vector<char> data = read(from, remainingSize);
        from = mFile.pos();
        remainingSize -= data.size();
        write(to, data);
        to = mFile.pos();
    }
}

auto File::moveRecord(acore::size_type index, acore::size_type to, acore::size_type sizeToMove) -> void
{
    mRecords.updateIndex(to, FileRecords::Index{index, sizeToMove});
    acore::size_type newPos = mFile.pos();
    moveData(newPos, mRecords.pos(index), sizeToMove);
    mRecords.setRecord(index, FileRecords::Index{newPos, sizeToMove});
}

auto File::moveRecordToEnd(acore::size_type index, acore::size_type size) -> void
{
    mRecords.invalidateIndex(index);
    moveRecord(index, mFile.buffer().size(), std::min(size, mRecords.size(index)));

    if (mRecords.size(index) != size)
    {
        resizeAtEnd(index, size);
    }
}

auto File::optimizeRecord(acore::size_type index) -> void
{
    if (mRecords.isValid(index))
    {
        if (mFile.pos() != mRecords.recordPos(index))
        {
            moveRecord(index, mFile.pos(), mRecords.size(index));
        }
        else
        {
            mFile.seek(mRecords.recordEnd(index));
        }
    }
}

auto File::read(acore::size_type readPos, acore::size_type remainingSize) -> std::vector<char>
{
    mFile.seek(readPos);
    std::vector<char> data(std::min(MAX_STEP_SIZE, remainingSize));
    mFile.read(data.data(), data.size());
    return data;
}

auto File::removeData(acore::size_type idx) -> void
{
    if (mRecords.isLast(idx))
    {
        const acore::size_type pos = mRecords.recordPos(idx);
        mWAL.recordLog(pos, mFile.buffer().size() - pos);
        mFile.buffer().resize(pos);
    }
    else
    {
        mRecords.invalidateIndex(idx);
    }
}

auto File::resetBuffer() -> void
{
    if (mBufferStream.buffer().size() > BUFFER_SIZE)
    {
        mBufferStream.buffer().data().clear();
        mBufferStream.buffer().data().reserve(BUFFER_SIZE);
    }

    mBufferStream.buffer().resize(0);
    mBufferStream.reset();
}

auto File::resize(acore::size_type newSize) -> void
{
    mWAL.recordLog(newSize, mFile.buffer().size() - newSize);
    mFile.buffer().resize(newSize);
}

auto File::resizeAt(acore::size_type index, acore::size_type newSize) -> void
{
    if (!mRecords.isLast(index))
    {
        moveRecordToEnd(index, newSize);
    }
    else
    {
        resizeAtEnd(index, newSize);
    }
}

auto File::resizeAtEnd(acore::size_type idx, acore::size_type newSize) -> void
{
    acore::size_type oldSize = mRecords.size(idx);

    if (newSize < oldSize)
    {
        resize(mFile.buffer().size() - (oldSize - newSize));
    }
    else
    {
        append(newSize - oldSize);
    }

    mRecords.setSize(idx, newSize);
    mRecords.updateIndex(FileRecords::Index{idx, newSize});
}

auto File::validateFileNotInUse(const char *filename) -> const char *
{
    const std::filesystem::path file(filename);

    if (std::find(File::filesInUse()->begin(), File::filesInUse()->end(), file) != File::filesInUse()->end())
    {
        throw acore::Exception() << "Cannot use the file '" << file << "'. The file is already in use.";
    }

    return filename;
}

auto File::validateIndex(acore::size_type index) const -> void
{
    if (!contains(index))
    {
        throw acore::Exception() << "Invalid index '" << index << "'";
    }
}

auto File::validateReadInput(acore::size_type index, acore::size_type offset) const -> void
{
    validateIndex(index);
    validateOffset(index, offset);
}

auto File::validateWriteInput(acore::size_type index, acore::size_type offset) const -> void
{
    validateIndex(index);
    validateOffset(offset);
}

auto File::validateMoveInput(acore::size_type index, acore::size_type offset, acore::size_type newOffset, acore::size_type size) const -> void
{
    validateIndex(index);
    validateOffset(index, offset);
    validateOffset(newOffset);
    mRecords.validatePos(index, offset + size);
    validateSize(size);
}

auto File::validateOffset(acore::size_type index, acore::size_type offset) const -> void
{
    validateOffset(offset);
    mRecords.validatePos(index, mRecords.pos(index) + offset);
}

auto File::validateOffset(acore::size_type offset) -> void
{
    if (offset < 0)
    {
        throw acore::Exception() << "Invalid offset: '" << offset << "'";
    }
}

auto File::validateSize(acore::size_type size) -> void
{
    if (size < 0)
    {
        throw acore::Exception() << "Invalid size '" << size << "'";
    }
}

auto File::write(acore::size_type pos, const std::vector<char> &data) -> void
{
    mWAL.recordLog(pos, data.size());
    mFile.seek(pos);
    mFile.write(data.data(), data.size());
}
}
