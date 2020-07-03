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

#ifndef AFILE_PERSISTENTVECTOR_HPP
#define AFILE_PERSISTENTVECTOR_HPP

#include "AFileModule.hpp"
#include "File.hpp"

#include <initializer_list>
#include <iterator>
#include <vector>

namespace afile
{
//! The PersistentVector<T> is a template
//! class that provides persistent dynamic array.
//!
//! This class uses the acore::VectorBase as
//! its base and thus has the same interface as
//! acore::Vector. As its underlying storage
//! a file based class is used storing the
//! data to the underlying file storage provided
//! by the File class.
//!
//! When the vector is first created (constructor
//! taking only the File is used) the vector
//! will be assigned an index. You can retrieve it
//! with fileIndex() for future reconstructing the
//! vector from the file.
//!
//! Reference and iterator semantics are facilitated
//! by the acore::RandomAccessIterator and
//! acore::Reference classes and mostly match the
//! behaviour of pointer iterators and normal references.
//!
//! \note Implicit conversions are limited with the
//! acore::Reference class. You can use the operator*
//! to retrieve the actual value and regain the full
//! implicit conversion capabilities.
//!
//! One exception is disabling of the \c operator&
//! of the acore::Reference since taking address of
//! things not stored in memory makes no sense.
template<typename T>
class PersistentVector
{
    using value_type = T;
    using size_type = acore::size_type;
    using difference_type = std::ptrdiff_t;
    using const_pointer = const T *;
    using pointer = T *;
    using reference = acore::Reference<T, PersistentVector>;
    using const_reference = acore::Reference<T, const PersistentVector>;
    using const_iterator = acore::RandomAccessIterator<const T, const_reference, const PersistentVector>;
    using iterator = acore::RandomAccessIterator<T, reference, PersistentVector>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
    //! Constructs a new vector in the \a file.
    explicit PersistentVector(File *file) :
        mFile{file},
        mIndex{mFile->insert(std::vector<T>{})}
    {
    }

    //! Constructs a vector from existing data in
    //! the \a file at the \a index.
    PersistentVector(File *file, acore::size_type index) :
        mFile{file},
        mIndex{index},
        mSize{mFile->value<acore::size_type>(mIndex, 0)}
    {
        if (!mFile->contains(mIndex))
        {
            throw acore::Exception() << "File '" << mFile->filename() << "' does not contain index '" << mIndex << "'";
        }

        if (mSize < 0)
        {
            throw acore::Exception() << "Data at index '" << mIndex << "' in file '" << mFile->filename() << "' is invalid";
        }
    }

    //! Constructs a new vector in the \a file initializing
    //! it to a given \a size of default constructed
    //! elements.
    PersistentVector(acore::size_type size, File *file) :
        mFile{file},
        mIndex{mFile->insert(std::vector<T>(size))},
        mSize{size}
    {
    }

    //! Constructs a new vector in the \a file initializing
    //! it to a given \a size of \a value.
    PersistentVector(acore::size_type size, const T &value, File *file) :
        mFile{file},
        mIndex{mFile->insert(std::vector<T>(size, value))},
        mSize{size}
    {
    }

    //! Constructs a new vector in the \a file initializing
    //! it to values from the range \a first to \a last.
    template<typename It, std::enable_if_t<!std::is_same_v<typename std::iterator_traits<It>::iterator_category, void>, int> = 0>
    PersistentVector(It first, It last, File *file) :
        mFile{file},
        mIndex{mFile->insert(std::vector<T>(first, last))},
        mSize{static_cast<acore::size_type>(std::distance(first, last))}
    {
    }

    //! Constructs a new vector in the \a file with
    //! values from the \a list.
    PersistentVector(std::initializer_list<T> list, File *file) :
        mFile{file},
        mIndex{mFile->insert(list)},
        mSize{static_cast<acore::size_type>(list.size())}
    {
    }

    //! Deleted copy constructor.
    PersistentVector(const PersistentVector &other) = delete;

    //! Move constructor.
    PersistentVector(PersistentVector &&other) noexcept = default;

    //! Destructor.
    ~PersistentVector() = default;

    template<class It, std::enable_if_t<!std::is_same_v<typename std::iterator_traits<It>::iterator_category, void>, int> = 0>
    auto assign(It first, It last) -> void
    {
        resize(0);
        insert(cbegin(), first, last);
    }

    [[nodiscard]] auto at(acore::size_type pos) -> reference
    {
        checkBounds(pos);
        return (*this)[pos];
    }

    [[nodiscard]] auto at(acore::size_type pos) const -> const_reference
    {
        checkBounds(pos);
        return (*this)[pos];
    }

    auto assign(std::initializer_list<T> values) -> void
    {
        *this = values;
    }

    [[nodiscard]] auto back() -> reference
    {
        return (*this)[size() - 1];
    }

    [[nodiscard]] auto back() const -> const_reference
    {
        return (*this)[size() - 1];
    }

    [[nodiscard]] auto begin() noexcept -> iterator
    {
        return createIterator(0);
    }

    [[nodiscard]] auto begin() const noexcept -> const_iterator
    {
        return createIterator(0);
    }

    //! Returns the currently allocated capacity of
    //! the vector. The vector will typicially
    //! allocate space for more elements than is needed
    //! for its size() to avoid frequent reallocations
    //! when inserting new elements.
    auto capacity() const noexcept -> acore::size_type
    {
        return (mFile->size(mIndex) - static_cast<acore::size_type>(sizeof(acore::size_type))) / serializedSize();
    }

    [[nodiscard]] auto cbegin() const noexcept -> const_iterator
    {
        return begin();
    }

    [[nodiscard]] auto cend() const noexcept -> const_iterator
    {
        return end();
    }

    auto clear() noexcept -> void
    {
        resize(0);
    }

    [[nodiscard]] auto crbegin() const noexcept -> const_reverse_iterator
    {
        return rbegin();
    }

    [[nodiscard]] auto crend() const noexcept -> const_reverse_iterator
    {
        return rend();
    }

    auto emplace(const_iterator pos, T &&value) -> iterator
    {
        return insert(pos, std::move(value));
    }

    auto emplace_back(T &&value) -> reference
    {
        return *insert(cend(), std::move(value));
    }

    [[nodiscard]] constexpr auto empty() const noexcept -> bool
    {
        return mSize == 0;
    }

    [[nodiscard]] auto end() noexcept -> iterator
    {
        return createIterator(size());
    }

    [[nodiscard]] auto end() const noexcept -> const_iterator
    {
        return createIterator(size());
    }

    auto erase(const_iterator pos) -> iterator
    {
        return erase(pos, pos + 1);
    }

    auto erase(const_iterator first, const_iterator last) -> iterator
    {
        moveLeft(std::distance<decltype(last)>(cbegin(), last), size(), std::distance<decltype(first)>(cbegin(), first));
        resize(size() - std::distance(first, last));
        return begin() + std::distance<decltype(first)>(cbegin(), first);
    }

    //! Returns File* holding the data.
    [[nodiscard]] constexpr auto file() const noexcept -> File *
    {
        return mFile;
    }

    //! Returns \c index to the file's value holding the data.
    [[nodiscard]] constexpr auto fileIndex() const noexcept -> acore::size_type
    {
        return mIndex;
    }

    [[nodiscard]] auto front() -> reference
    {
        return (*this)[0];
    }

    [[nodiscard]] auto front() const -> const_reference
    {
        return (*this)[0];
    }

    auto insert(const_iterator pos, const T &value) -> iterator
    {
        return insert(pos, std::move(T{value}));
    }

    auto insert(const_iterator pos, T &&value) -> iterator
    {
        acore::size_type i = static_cast<acore::size_type>(std::distance<decltype(pos)>(cbegin(), pos));
        resizeRealloc(size() + 1);
        iterator it = begin();
        std::advance(it, i);
        moveRight(i, size() - 1, i + 1);
        *it = std::move(value);
        return it;
    }

    auto insert(const_iterator pos, size_type count, const T &value) -> iterator
    {
        acore::size_type i = static_cast<acore::size_type>(std::distance<decltype(pos)>(cbegin(), pos));
        resizeRealloc(size() + count);
        iterator it = begin();
        std::advance(it, i);
        moveRight(i, size() - count, i + count);
        iterator last = it;
        std::advance(last, count);
        std::fill(it, last, value);
        return it;
    }

    template<class It, std::enable_if_t<!std::is_same_v<typename std::iterator_traits<It>::iterator_category, void>, int> = 0>
    auto insert(const_iterator pos, It first, It last) -> iterator
    {
        acore::size_type i = std::distance<decltype(pos)>(cbegin(), pos);
        resizeRealloc(size() + std::distance(first, last));
        iterator it = begin() + i;
        moveRight(i, size() - std::distance(first, last), i + std::distance(first, last));
        std::copy(first, last, it);
        return it;
    }

    auto insert(const_iterator pos, std::initializer_list<T> values) -> iterator
    {
        acore::size_type i = std::distance<decltype(pos)>(cbegin(), pos);
        resizeRealloc(size() + values.size());
        iterator it = begin() + i;
        moveRight(i, size() - values.size(), i + values.size());
        std::copy(values.begin(), values.end(), it);
        return it;
    }

    auto pop_back() -> void
    {
        erase(cend() - 1);
    }

    auto push_back(const T &value) -> void
    {
        insert(cend(), value);
    }

    auto push_back(T &&value) -> void
    {
        insert(cend(), std::move(value));
    }

    constexpr size_type max_size() const noexcept
    {
        return std::numeric_limits<acore::size_type>::max();
    }

    [[nodiscard]] auto rbegin() noexcept -> reverse_iterator
    {
        return reverse_iterator{end()};
    }

    [[nodiscard]] auto rbegin() const noexcept -> const_reverse_iterator
    {
        return const_reverse_iterator{end()};
    }

    [[nodiscard]] auto rend() noexcept -> reverse_iterator
    {
        return reverse_iterator{begin()};
    }

    [[nodiscard]] auto rend() const noexcept -> const_reverse_iterator
    {
        return const_reverse_iterator{begin()};
    }

    //! Resizes the space of the vector in the file
    //! to accomodate \a size elements. If the \a size
    //! is <= capacity() this method does nothing. Call
    //! this method when you know ahead of time how many
    //! elements the vector will hold to avoid the unnecessary
    //! reallocations.
    auto reserve(acore::size_type size) -> void
    {
        if (capacity() < size)
        {
            mFile->resize(mIndex, static_cast<acore::size_type>(sizeof(acore::size_type)) + size * serializedSize());
        }
    }

    auto resize(acore::size_type count) -> void
    {
        resize(count, T{});
    }

    auto resize(acore::size_type count, const value_type &value) -> void
    {
        mFile->beginWAL();
        reserve(count);

        mFile->insert(mIndex, 0, count);

        for (acore::size_type i = mSize; i < count; i++)
        {
            mFile->insert(mIndex, offset(i), value);
        }

        mSize = count;
        mFile->endWAL();
    }

    auto shrink_to_fit() -> void
    {
        mFile->resize(mIndex, offset(size()));
    }

    [[nodiscard]] constexpr auto size() const noexcept -> acore::size_type
    {
        return mSize;
    }

    [[nodiscard]] auto operator[](acore::size_type pos) -> reference
    {
        return *createIterator(pos);
    }

    [[nodiscard]] auto operator[](acore::size_type pos) const -> const_reference
    {
        return *createIterator(pos);
    }

    //! Deleted copy assignment operator.
    auto operator=(const PersistentVector &other) -> PersistentVector & = delete;

    //! Move assignment operator.
    auto operator=(PersistentVector &&other) noexcept -> PersistentVector & = default;

    //! Replace the current content of the vector with \a values;
    auto operator=(std::initializer_list<T> values) -> PersistentVector &
    {
        mFile->insert(mIndex, 0, values);
        mSize = values.size();
        return *this;
    }

private:
    friend iterator;
    friend const_iterator;
    friend reference;
    friend const_reference;

    constexpr auto checkBounds(size_type index) const -> void
    {
        if (index < 0 || size() <= index)
        {
            throw acore::Exception{} << "Index out of bounds: " << index;
        }
    }

    [[nodiscard]] constexpr auto createIterator(acore::size_type index) noexcept -> iterator
    {
        return iterator{index, this};
    }

    [[nodiscard]] constexpr auto createIterator(acore::size_type index) const noexcept -> const_iterator
    {
        return const_iterator{index, this};
    }

    [[nodiscard]] constexpr auto distance(size_type left, size_type right) const noexcept -> acore::size_type
    {
        return left - right;
    }

    constexpr auto moveLeft(size_type begin, size_type end, size_type to) -> void
    {
        moveTo(begin, end, to);
    }

    constexpr auto moveRight(size_type begin, size_type end, size_type to) -> void
    {
        moveTo(begin, end, to);
    }

    constexpr auto moveTo(size_type begin, size_type end, size_type to) -> void
    {
        const acore::size_type off = offset(begin);
        mFile->copy(mIndex, off, offset(to), offset(end) - off);
    }

    [[nodiscard]] constexpr auto nextIndex(size_type index, size_type steps) const noexcept -> acore::size_type
    {
        return index + steps;
    }

    [[nodiscard]] constexpr auto offset(acore::size_type index) const noexcept -> acore::size_type
    {
        return static_cast<acore::size_type>(sizeof(acore::size_type)) + index * serializedSize();
    }

    [[nodiscard]] constexpr auto previousIndex(acore::size_type index, acore::size_type steps) const noexcept -> acore::size_type
    {
        return index - steps;
    }

    constexpr auto reallocate(size_type capacity) -> void
    {
        mFile->resize(mIndex, offset(capacity));
    }

    [[nodiscard]] constexpr auto referenceAt(acore::size_type index) noexcept -> reference
    {
        return reference{index, this};
    }

    [[nodiscard]] constexpr auto referenceAt(acore::size_type index) const noexcept -> const_reference
    {
        return const_reference{index, this};
    }

    constexpr auto resizeRealloc(size_type newSize) -> void
    {
        if (capacity() < newSize)
        {
            reallocate(acore::nextPowerOfTwo(newSize));
        }

        resize(newSize);
    }

    [[nodiscard]] static auto serializedSize() -> acore::size_type
    {
        static const acore::size_type SIZE_T = []() {
            acore::DataStream stream;
            stream << T{};
            return stream.buffer().size();
        }();

        return SIZE_T;
    }

    constexpr auto setValue(size_type index, const T &value) -> void
    {
        mFile->insert(mIndex, offset(index), value);
    }

    [[nodiscard]] constexpr auto value(size_type index) const -> T
    {
        return mFile->value<T>(mIndex, offset(index));
    }

    File *mFile = nullptr;
    acore::size_type mIndex = acore::INVALID_INDEX;
    acore::size_type mSize = 0;
};

template<typename T>
[[nodiscard]] constexpr bool operator==(const PersistentVector<T> &left, const PersistentVector<T> &right) noexcept
{
    return std::equal(left.cbegin(), left.cend(), right.begin(), right.end());
}

//! Returns \c true if the values of \a left are not
//! equal to those of the \c Container \a right. This
//! function can compare the vector with any \c Container
//! that implements the %begin() and %end() methods.
//! //! Containers of different sizes are not equal.
template<typename T>
[[nodiscard]] constexpr bool operator!=(const PersistentVector<T> &left, const PersistentVector<T> &right) noexcept
{
    return !(left == right);
}

//! Returns \c true if \a left is lexicographically
//! less than the \a right.
//! E.g. {1,2,3} < {2,2,3}
template<typename T>
[[nodiscard]] constexpr bool operator<(const PersistentVector<T> &left, const PersistentVector<T> &right)
{
    return std::lexicographical_compare(left.begin(), left.end(), right.begin(), right.end());
}

//! Returns \c true if \a left is lexicographically
//! less than or equal to the \a right.
template<typename T>
[[nodiscard]] constexpr bool operator<=(const PersistentVector<T> &left, const PersistentVector<T> &right)
{
    return left < right || left == right;
}

//! Returns \c true if the\a left is lexicographically
//! greater than the \a right.
//! E.g. {2,2,3} > {1,2,3}
template<typename T>
[[nodiscard]] constexpr bool operator>(const PersistentVector<T> &left, const PersistentVector<T> &right)
{
    return right < left;
}

//! Returns \c true if the\a left is lexicographically
//! greater than or equal to the \a right.
template<typename T>
[[nodiscard]] constexpr bool operator>=(const PersistentVector<T> &left, const PersistentVector<T> &right)
{
    return left > right || left == right;
}
}

#endif
