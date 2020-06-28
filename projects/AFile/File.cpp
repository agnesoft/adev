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
    mWAL{&mFile}
{
    if (mFile.buffer().isOpen())
    {
        mBufferStream.buffer().data().reserve(BUFFER_SIZE);
        mWAL.initialize();
        initialize();
    }
    else
    {
        throw acore::Exception() << "File '" << filename << "' could not be opened";
    }
}

File::~File() //NOLINT(bugprone-exception-escape)
{
    mWAL.process();
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
    mCount = 0;
    mFreeIndex = acore::INVALID_INDEX;
    mRecords.clear();

    mFile.buffer().resize(static_cast<acore::size_type>(sizeof(acore::size_type)));
    mFile.reset();
    mFile << acore::size_type(0);
}

auto File::contains(acore::size_type index) const -> bool
{
    if (index < static_cast<acore::size_type>(mRecords.size()))
    {
        return 0 <= mRecords[index].size;
    }

    return false;
}

auto File::count() const noexcept -> acore::size_type
{
    return mCount;
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
    std::vector<acore::size_type> idxs;
    idxs.reserve(mCount);

    for (const Index &idx : mRecords)
    {
        if (isValid(idx))
        {
            idxs.push_back(static_cast<acore::size_type>(std::distance(mRecords.data(), &idx)));
        }
    }

    return idxs;
}

auto File::isEmpty() const noexcept -> bool
{
    return mCount == 0;
}

auto File::move(acore::size_type index, acore::size_type offset, acore::size_type newOffset, acore::size_type size) -> void
{
    validateMoveInput(index, offset, newOffset, size);
    mOffset = newOffset;
    mBufferStream.buffer().resize(std::max(mBufferStream.buffer().size(), size));
    mFile.seek(mRecords[index].pos + offset);
    mFile.read(mBufferStream.buffer().data().data(), size);
    endWrite(index);
}

auto File::optimize() -> void
{
    mWAL.reset();
    mFile.seek(static_cast<acore::size_type>(sizeof(acore::size_type)));

    for (acore::size_type idx : sortedIndexes())
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
        removeIndex(index);
        mWAL.reset();
    }
}

auto File::resize(acore::size_type index, acore::size_type size) -> void
{
    validateIndex(index);
    validateSize(size);

    if (mRecords[index].size != size)
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
        return mRecords[index].size;
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
    mFile.seek(mRecords[index].pos + offset);
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
    return (offset + mBufferStream.buffer().size()) > mRecords[index].size;
}

auto File::buildFreeList() -> void
{
    for (Index &index : mRecords)
    {
        if (index.size < 0)
        {
            index.pos = mFreeIndex;
            mFreeIndex = recordIndex(&index);
        }
    }
}

auto File::createIndex() -> void
{
    acore::size_type count = 0;
    mFile.reset();
    mFile >> count;

    if (count < 0)
    {
        throw acore::Exception() << "Index corrupted";
    }

    mRecords.resize(count);
}

auto File::emptyValue(acore::size_type size) -> std::vector<char>
{
    return std::vector<char>(std::min(MAX_STEP_SIZE, size));
}

auto File::endPos(acore::size_type index) const noexcept -> acore::size_type
{
    return mRecords[index].pos + mRecords[index].size;
}

auto File::endRead(acore::size_type index) const -> void
{
    validatePos(index, mFile.pos());
}

auto File::endWrite(acore::size_type index) -> void
{
    if (bufferExtendsValue(index, mOffset))
    {
        if (!isLast(index))
        {
            moveRecordToEnd(index, mOffset);
        }

        mRecords[index].size = mOffset + mBufferStream.buffer().size();
        updateIndex(recordPos(index), Index{index, mRecords[index].size});
    }

    write(mRecords[index].pos + mOffset, mBufferStream.buffer().data());
    resetBuffer();
    mWAL.reset();
}

auto File::filesInUse() -> std::vector<std::filesystem::path> *
{
    static auto *files = std::make_unique<std::vector<std::filesystem::path>>().release();
    return files;
}

auto File::initialize() -> void
{
    if (mFile.buffer().size() != 0)
    {
        loadFile();
    }
    else
    {
        mFile << acore::size_type(0);
    }

    File::filesInUse()->emplace_back(mFile.buffer().filename());
}

auto File::invalidateIndex(acore::size_type idx) -> void
{
    updateIndex(recordPos(idx), Index{acore::INVALID_INDEX, -mRecords[idx].size});
}

auto File::isLast(acore::size_type idx) const -> bool
{
    return mFile.buffer().size() == endPos(idx);
}

auto File::isValid(File::Index idx) noexcept -> bool
{
    return 0 < idx.size;
}

auto File::loadFile() -> void
{
    try
    {
        createIndex();
        loadIndex();
        buildFreeList();
    }
    catch (...)
    {
        throw acore::Exception() << "Could not load file: invalid or corrupted file data";
    }
}

auto File::loadIndex() -> void
{
    while (mFile.pos() != mFile.buffer().size())
    {
        processIndex(readIndex());
    }
}

auto File::logicalRecordPos(acore::size_type pos) noexcept -> acore::size_type
{
    return pos + static_cast<acore::size_type>(sizeof(Index));
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
    updateIndex(to, Index{index, sizeToMove});
    acore::size_type newPos = mFile.pos();
    moveData(newPos, mRecords[index].pos, sizeToMove);
    mRecords[index] = Index{newPos, sizeToMove};
}

auto File::moveRecordToEnd(acore::size_type index, acore::size_type size) -> void
{
    invalidateIndex(index);
    moveRecord(index, mFile.buffer().size(), std::min(size, mRecords[index].size));

    if (mRecords[index].size != size)
    {
        resizeAtEnd(index, size);
    }
}

auto File::newIndex() -> acore::size_type
{
    acore::size_type index = mFreeIndex;
    Index recordIndex{logicalRecordPos(mFile.buffer().size()), 0};

    if (index == acore::INVALID_INDEX)
    {
        index = static_cast<acore::size_type>(mRecords.size());
        mRecords.push_back(recordIndex);
        saveRecordsCount(static_cast<acore::size_type>(mRecords.size()));
    }
    else
    {
        mFreeIndex = mRecords[index].pos;
        mRecords[index] = recordIndex;
    }

    updateIndex(mFile.buffer().size(), Index{index, 0});
    mCount++;
    return index;
}

auto File::optimizeRecord(acore::size_type index) -> void
{
    if (isValid(mRecords[index]))
    {
        if (mFile.pos() != recordPos(index))
        {
            moveRecord(index, mFile.pos(), mRecords[index].size);
        }
        else
        {
            seekToEnd(index);
        }
    }
}

auto File::processIndex(Index index) -> void
{
    if (isValid(index))
    {
        mRecords[index.pos] = Index{mFile.pos(), index.size};
        mCount++;
    }

    seekToEnd({mFile.pos(), index.size});
}

auto File::read(acore::size_type readPos, acore::size_type remainingSize) -> std::vector<char>
{
    mFile.seek(readPos);
    std::vector<char> data(std::min(MAX_STEP_SIZE, remainingSize));
    mFile.read(data.data(), data.size());
    return data;
}

auto File::readIndex() -> Index
{
    Index index;
    mFile >> index;
    return index;
}

auto File::recordIndex(const File::Index *idx) const noexcept -> acore::size_type
{
    return std::distance(mRecords.data(), idx);
}

auto File::recordPos(acore::size_type index) const noexcept -> acore::size_type
{
    return mRecords[index].pos - static_cast<acore::size_type>(sizeof(Index));
}

auto File::removeData(acore::size_type idx) -> void
{
    if (isLast(idx))
    {
        const acore::size_type pos = recordPos(idx);
        mWAL.recordLog(pos, mFile.buffer().size() - pos);
        mFile.buffer().resize(pos);
    }
    else
    {
        invalidateIndex(idx);
    }
}

auto File::removeIndex(acore::size_type index) -> void
{
    updateRemovedIndex(index);
    mCount--;
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
    if (!isLast(index))
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
    acore::size_type oldSize = mRecords[idx].size;

    if (newSize < oldSize)
    {
        resize(mFile.buffer().size() - (oldSize - newSize));
    }
    else
    {
        append(newSize - oldSize);
    }

    mRecords[idx].size = newSize;
    updateIndex(recordPos(idx), {idx, newSize});
}

auto File::saveRecordsCount(acore::size_type count) -> void
{
    mWAL.recordLog(0, static_cast<acore::size_type>(count));
    mFile.reset();
    mFile << static_cast<acore::size_type>(mRecords.size());
}

auto File::seekToEnd(acore::size_type idx) -> void
{
    seekToEnd(mRecords[idx]);
}

auto File::seekToEnd(File::Index index) -> void
{
    mFile.seek(index.pos + (0 < index.size ? index.size : -index.size));
}

auto File::sortedIndexes() -> std::vector<acore::size_type>
{
    std::vector<acore::size_type> indexes(mRecords.size());
    std::iota(indexes.begin(), indexes.end(), 0);

    std::sort(indexes.begin(), indexes.end(), [&records = mRecords](acore::size_type left, acore::size_type right) {
        return records[left].pos < records[right].pos;
    });

    return indexes;
}

auto File::updateIndex(acore::size_type pos, Index index) -> void
{
    mWAL.recordLog(pos, static_cast<acore::size_type>(sizeof(Index)));
    mFile.seek(pos);
    mFile << index;
}

auto File::updateRemovedIndex(acore::size_type recordIndex) -> void
{
    mRecords[recordIndex] = {mFreeIndex, acore::INVALID_INDEX};
    mFreeIndex = recordIndex;
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
    validatePos(index, offset + size);
    validateSize(size);
}

auto File::validateOffset(acore::size_type index, acore::size_type offset) const -> void
{
    validateOffset(offset);
    validatePos(index, mRecords[index].pos + offset);
}

auto File::validateOffset(acore::size_type offset) -> void
{
    if (offset < 0)
    {
        throw acore::Exception() << "Invalid offset: '" << offset << "'";
    }
}

auto File::validatePos(acore::size_type index, acore::size_type pos) const -> void
{
    if (endPos(index) < pos)
    {
        throw acore::Exception() << "Pos '" << pos << "' is out of bounds of record '" << index << "' (" << mRecords[index].pos << '-' << endPos(index) << ')';
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
