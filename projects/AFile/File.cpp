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

#include <algorithm>
#include <filesystem>
#include <memory>
#include <vector>

namespace afile
{

File::File(const char *filename) :
    mData{validateFileNotInUse(filename)},
    mWAL{&mData},
    mRecords{&mData, &mWAL}
{
    File::filesInUse()->emplace_back(filename);
}

File::~File() //NOLINT(bugprone-exception-escape)
{
    const auto it = std::find(File::filesInUse()->begin(), File::filesInUse()->end(), mData.filename());

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
    mData.clear();
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
    return mData.filename();
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
    mData.move(mRecords.pos(index), offset, newOffset, size);
    endWrite(index);
}

auto File::optimize() -> void
{
    mWAL.reset();
    mData.seek(static_cast<acore::size_type>(sizeof(acore::size_type)));

    for (acore::size_type idx : mRecords.sortedIndexes())
    {
        optimizeRecord(idx);
    }

    resize(mData.pos());
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
    return mData.size();
}

auto File::size(acore::size_type index) const -> acore::size_type
{
    if (contains(index))
    {
        return mRecords.size(index);
    }

    return 0;
}

auto File::beginRead(acore::size_type index, acore::size_type offset) const -> FileStream &
{
    validateReadInput(index, offset);
    mData.seek(mRecords.pos(index) + offset);
    return mData.file();
}

auto File::beginWrite(acore::size_type index, acore::size_type offset) -> acore::DataStream &
{
    validateWriteInput(index, offset);
    return mData.beginWrite(offset);
}

auto afile::File::bufferExtendsValue(acore::size_type index) const -> bool
{
    return mData.bufferSize() > mRecords.size(index);
}

auto File::endRead(acore::size_type index) const -> void
{
    validatePos(index, mData.pos());
}

auto File::endWrite(acore::size_type index) -> void
{
    if (bufferExtendsValue(index))
    {
        if (!mRecords.isLast(index))
        {
            moveRecordToEnd(index, mData.offset());
        }

        mRecords.setSize(index, mData.bufferSize());
        mRecords.updateIndex(FileRecords::Index{index, mRecords.size(index)});
    }

    mData.endWrite(mRecords.pos(index));
    mWAL.reset();
}

auto File::filesInUse() -> std::vector<std::filesystem::path> *
{
    static auto *files = std::make_unique<std::vector<std::filesystem::path>>().release();
    return files;
}

auto File::moveRecord(acore::size_type index, acore::size_type to, acore::size_type sizeToMove) -> void
{
    mRecords.updateIndex(to, FileRecords::Index{index, sizeToMove});
    const acore::size_type newPos = mData.pos();
    mData.moveData(newPos, mRecords.pos(index), sizeToMove);
    mRecords.setRecord(index, FileRecords::Index{newPos, sizeToMove});
}

auto File::moveRecordToEnd(acore::size_type index, acore::size_type size) -> void
{
    mRecords.invalidateIndex(index);
    moveRecord(index, mData.size(), std::min(size, mRecords.size(index)));

    if (mRecords.size(index) != size)
    {
        resizeAtEnd(index, size);
    }
}

auto File::optimizeRecord(acore::size_type index) -> void
{
    if (mRecords.isValid(index))
    {
        if (mData.pos() != mRecords.recordPos(index))
        {
            moveRecord(index, mData.pos(), mRecords.size(index));
        }
        else
        {
            mData.seek(mRecords.recordEnd(index));
        }
    }
}

auto File::removeData(acore::size_type idx) -> void
{
    if (mRecords.isLast(idx))
    {
        const acore::size_type pos = mRecords.recordPos(idx);
        mWAL.recordLog(pos, mData.size() - pos);
        mData.resize(pos);
    }
    else
    {
        mRecords.invalidateIndex(idx);
    }
}

auto File::resize(acore::size_type newSize) -> void
{
    mWAL.recordLog(newSize, mData.size() - newSize);
    mData.resize(newSize);
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
        resize(mData.size() - (oldSize - newSize));
    }
    else
    {
        mData.append(newSize - oldSize);
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
    validatePos(index, offset + size);
    validateSize(size);
}

auto File::validateOffset(acore::size_type index, acore::size_type offset) const -> void
{
    validateOffset(offset);
    validatePos(index, mRecords.pos(index) + offset);
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
    if (mRecords.endPos(index) < pos)
    {
        throw acore::Exception() << "Pos '" << pos << "' is out of bounds of record '" << index << "' (" << mRecords.pos(index) << '-' << mRecords.endPos(index) << ')';
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
    mData.write(pos, data);
}
}
