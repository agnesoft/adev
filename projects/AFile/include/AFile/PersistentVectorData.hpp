#pragma once

#include "File.hpp"

namespace afile
{
template<typename T>
class PersistentVectorData
{
public:
    using const_pointer = const T *;
    using pointer = T *;
    using reference = acore::Reference<T, PersistentVectorData>;
    using const_reference = acore::Reference<T, const PersistentVectorData>;
    using const_iterator = acore::RandomAccessIterator<const T, const_reference, const PersistentVectorData>;
    using iterator = acore::RandomAccessIterator<T, reference, PersistentVectorData>;

    explicit constexpr PersistentVectorData(File *file) :
        mFile(file),
        mIndex(mFile->insert(acore::Vector<T>()))
    {
    }

    constexpr PersistentVectorData(size_type size, const T &value, File *file) :
        mFile(file),
        mIndex(mFile->insert(acore::Vector<T>(size, value))),
        mSize(size)
    {
    }

    constexpr PersistentVectorData([[maybe_unused]] size_type size, [[maybe_unused]] const T &value, File *file, size_type index) :
        mFile(file),
        mIndex(index)
    {
        if(!mFile->contains(mIndex))
        {
            throw acore::Exception() << "File '" << mFile->filename() << "' does not contain index '" << mIndex << "'";
        }

        mSize = mFile->value<size_type>(mIndex, 0);

        if(mSize < 0)
        {
            throw acore::Exception() << "Data at index '" << mIndex << "' in file '" << mFile->filename() << "' is corrupted or invalid";
        }
    }

    PersistentVectorData(const PersistentVectorData &other) noexcept = delete;

    constexpr PersistentVectorData(PersistentVectorData &&other) noexcept
    {
        *this = std::move(other);
    }

    ~PersistentVectorData() noexcept = default;

    constexpr size_type capacity() const
    {
        return capacityAt(mFile->size(mIndex));
    }

    constexpr iterator createIterator(size_type index) noexcept
    {
        return iterator(index, this);
    }

    constexpr const_iterator createIterator(size_type index) const noexcept
    {
        return const_iterator(index, this);
    }

    constexpr File *file() const noexcept
    {
        return mFile;
    }

    constexpr size_type fileIndex() const noexcept
    {
        return mIndex;
    }

    constexpr void moveLeft(size_type begin, size_type end, size_type to)
    {
        moveTo(begin, end, to);
    }

    constexpr void moveRight(size_type begin, size_type end, size_type to)
    {
        moveTo(begin, end, to);
    }

    constexpr void reallocate(size_type capacity)
    {
        mLastValue.index = acore::INVALID_INDEX;
        mFile->resize(mIndex, offset(capacity));
    }

    constexpr void resize(size_type size)
    {
        mFile->beginWAL();
        mFile->insert(mIndex, 0, size);

        for(size_type i = mSize; i < size; i++)
        {
            mFile->insert(mIndex, offset(i), T{});
        }

        mSize = size;
        mFile->endWAL();
    }

    constexpr void resize(size_type size, const T &value)
    {
        mFile->insert(mIndex, 0, size);

        for(size_type i = mSize; i < size; i++)
        {
            mFile->insert(mIndex, offset(i), value);
        }

        mSize = size;
    }

    constexpr void setValue(size_type index, const T &value)
    {
        mLastValue.index = index;
        mLastValue.value = value;
        mFile->insert(mIndex, offset(index), value);
    }

    constexpr size_type size() const noexcept
    {
        return mSize;
    }

    constexpr T value(size_type index) const
    {
        if(mLastValue.index != index)
        {
            mLastValue.index = index;
            mLastValue.value = mFile->value<T>(mIndex, offset(index));
        }

        return mLastValue.value;
    }

    PersistentVectorData &operator=(const PersistentVectorData &other) = delete;

    constexpr PersistentVectorData &operator=(PersistentVectorData &&other) noexcept
    {
        if(this != &other)
        {
            mIndex = other.mIndex;
            mFile = other.mFile;
            mSize = other.mSize;

            other.mIndex = 0;
            other.mFile = nullptr;
        }

        return *this;
    }

private:
    friend iterator;
    friend const_iterator;

    struct LastValue
    {
        size_type index = acore::INVALID_INDEX;
        T value;
    };

    constexpr size_type capacityAt(size_type size) const noexcept
    {
        return (size - static_cast<size_type>(sizeof(size_type))) / SIZE_T;
    }

    constexpr size_type distance(size_type left, size_type right) const noexcept
    {
        return left - right;
    }

    constexpr void moveTo(size_type begin, size_type end, size_type to)
    {
        const size_type off = offset(begin);
        mLastValue.index = acore::INVALID_INDEX;
        mFile->move(mIndex, off, offset(to), offset(end) - off);
    }

    constexpr size_type nextIndex(size_type index, size_type steps) const noexcept
    {
        return index + steps;
    }

    constexpr size_type offset(size_type index) const noexcept
    {
        return static_cast<size_type>(sizeof(size_type)) + index * SIZE_T;
    }

    constexpr size_type previousIndex(size_type index, size_type steps) const noexcept
    {
        return index - steps;
    }

    constexpr reference referenceAt(size_type index) noexcept
    {
        return reference(index, this);
    }

    constexpr const_reference referenceAt(size_type index) const noexcept
    {
        return const_reference(index, this);
    }

    static size_type serializedSize()
    {
        acore::DataStream stream;
        stream << T();
        return stream.buffer().size();
    }

    const size_type SIZE_T = serializedSize();
    File *mFile = nullptr;
    size_type mIndex = acore::INVALID_INDEX;
    size_type mSize = 0;
    mutable LastValue mLastValue;
};
}
