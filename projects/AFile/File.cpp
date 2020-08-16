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

namespace afile
{
File::File(const char *filename) :
    mData{validateFileNotInUse(filename)},
    mRecords{&mData}
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
    mData.beginWAL();
}

auto File::clear() -> void
{
    mRecords.clear();
    mData.reset();
}

auto File::contains(acore::size_type index) const noexcept -> bool
{
    return mRecords.contains(index);
}

auto File::copy(acore::size_type index, acore::size_type offset, acore::size_type newOffset, acore::size_type size) -> void
{
    validateCopyInput(index, offset, newOffset, size);
    beginWAL();
    write(index, newOffset, mData.read(mRecords.pos(index) + offset, size));
    endWAL();
}

auto File::count() const noexcept -> acore::size_type
{
    return mRecords.count();
}

auto File::endWAL() -> void
{
    mData.endWAL();
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

auto File::optimize() -> void
{
    beginWAL();
    auto pos = static_cast<acore::size_type>(sizeof(acore::size_type));

    for (acore::size_type index : mRecords.sortedIndexes())
    {
        pos = optimizeRecord(index, pos);
    }

    mData.resize(pos);
    endWAL();
}

auto File::remove(acore::size_type index) -> void
{
    if (contains(index))
    {
        beginWAL();
        removeData(index);
        mRecords.remove(index);
        endWAL();
    }
}

auto File::resize(acore::size_type index, acore::size_type size) -> void
{
    validateIndex(index);
    validateSize(size);

    if (mRecords.size(index) != size)
    {
        beginWAL();
        resizeAt(index, size);
        endWAL();
    }
}

auto File::size() const noexcept -> acore::size_type
{
    return mData.size();
}

auto File::size(acore::size_type index) const noexcept -> acore::size_type
{
    if (contains(index))
    {
        return mRecords.size(index);
    }

    return 0;
}

auto File::extendsValue(acore::size_type index, acore::size_type offset, acore::size_type valueSize) const noexcept -> bool
{
    return (offset + valueSize) > mRecords.size(index);
}

auto File::filesInUse() -> std::vector<std::filesystem::path> *
{
    static std::vector<std::filesystem::path> files;
    return &files;
}

auto File::moveRecord(acore::size_type index, acore::size_type to, acore::size_type sizeToMove) -> void
{
    mRecords.update(to, FileRecords::Index{index, sizeToMove});
    const acore::size_type newPos = mData.pos();
    mData.write(newPos, mData.read(mRecords.pos(index), sizeToMove));
    mRecords.set(index, FileRecords::Index{newPos, sizeToMove});
}

auto File::moveRecordToEnd(acore::size_type index, acore::size_type size) -> void
{
    mRecords.invalidate(index);
    moveRecord(index, mData.size(), std::min(size, mRecords.size(index)));

    if (mRecords.size(index) != size)
    {
        resizeAtEnd(index, size);
    }
}

auto File::optimizeRecord(acore::size_type index, acore::size_type pos) -> acore::size_type
{
    if (mRecords.isValid(index))
    {
        if (pos != mRecords.recordPos(index))
        {
            moveRecord(index, pos, mRecords.size(index));
        }

        return mRecords.endPos(index);
    }

    return pos;
}

auto File::removeData(acore::size_type index) -> void
{
    if (mRecords.isLast(index))
    {
        const acore::size_type pos = mRecords.recordPos(index);
        mData.resize(pos);
    }
    else
    {
        mRecords.invalidate(index);
    }
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

auto File::resizeAtEnd(acore::size_type index, acore::size_type newSize) -> void
{
    acore::size_type oldSize = mRecords.size(index);

    if (newSize < oldSize)
    {
        mData.resize(mData.size() - (oldSize - newSize));
    }
    else
    {
        mData.write(mData.size(), std::vector<char>(newSize - oldSize));
    }

    mRecords.update(FileRecords::Index{index, newSize});
}

auto File::validateCopyInput(acore::size_type index, acore::size_type offset, acore::size_type newOffset, acore::size_type size) const -> void
{
    validateIndex(index);
    validateOffset(index, offset);
    validateOffset(newOffset);
    validatePos(index, offset + size);
    validateSize(size);
}

auto File::validateFileNotInUse(const char *filename) -> const char *
{
    const std::filesystem::path file{filename};

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

auto File::validateReadInput(acore::size_type index, acore::size_type offset) const -> void
{
    validateIndex(index);
    validateOffset(index, offset);
}

auto File::validateSize(acore::size_type size) -> void
{
    if (size < 0)
    {
        throw acore::Exception() << "Invalid size '" << size << "'";
    }
}

auto File::validateWriteInput(acore::size_type index, acore::size_type offset) const -> void
{
    validateIndex(index);
    validateOffset(offset);
}

auto File::write(acore::size_type index, acore::size_type offset, const std::vector<char> &data) -> void
{
    if (extendsValue(index, offset, static_cast<acore::size_type>(data.size())))
    {
        if (!mRecords.isLast(index))
        {
            moveRecordToEnd(index, offset);
        }

        mRecords.update(FileRecords::Index{index, offset + static_cast<acore::size_type>(data.size())});
    }

    mData.write(mRecords.pos(index) + offset, data);
}
}
