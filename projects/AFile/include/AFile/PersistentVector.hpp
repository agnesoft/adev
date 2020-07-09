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

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

namespace afile
{
//! The PersistentVector<T> is a template
//! class that provides persistent dynamic array.
//! It mirrors the functionality of std::vector
//! but uses the File class as its storage.
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
public:
    //! Synonym to T template argument.
    using value_type = T;
    //! Type used for index based access.
    using size_type = acore::size_type;
    //! Synonym to <a href="http://en.cppreference.com/w/cpp/types/ptrdiff_t">ptrdiff_t</a>
    using difference_type = std::ptrdiff_t;
    //! Immutable pointer to T type. Not used.
    using const_pointer = const T *;
    //! Mutable pointer to T type. Not used.
    using pointer = T *;
    //! Mutable reference type.
    using reference = acore::Reference<T, PersistentVector>;
    //! Immutable reference type.
    using const_reference = acore::Reference<T, const PersistentVector>;
    //! Immutable iterator type.
    using const_iterator = acore::RandomAccessIterator<const T, const_reference, const PersistentVector>;
    //! Mutable iterator type.
    using iterator = acore::RandomAccessIterator<T, reference, PersistentVector>;
    //! Mutable reverse iterator type.
    using reverse_iterator = std::reverse_iterator<iterator>;
    //! Immutable reverse iterator type.
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

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

    //! Overwrites the current data with the elements
    //! in the range \a first - \a last resizing
    //! (and possibly reallocating) the vector if necessary.
    template<class It, std::enable_if_t<!std::is_same_v<typename std::iterator_traits<It>::iterator_category, void>, int> = 0>
    auto assign(It first, It last) -> void
    {
        clear();
        insert(cbegin(), first, last);
    }

    //! Overwrites the current data with the elements from
    //! the \a list resizing (and possibly reallocating)
    //! the vector if necessary.
    auto assign(std::initializer_list<T> values) -> void
    {
        *this = values;
    }

    //! Returns a #reference to the element at the
    //! \a index. The Exception is thrown if the
    //! \a index is out of bounds.
    [[nodiscard]] auto at(acore::size_type pos) -> reference
    {
        checkBounds(pos);
        return (*this)[pos];
    }

    //! Returns a #const_reference to the element at
    //! \a index. The Exception is thrown if the
    //! \a index is out of bounds.
    [[nodiscard]] auto at(acore::size_type pos) const -> const_reference
    {
        checkBounds(pos);
        return (*this)[pos];
    }

    //! Returns a #reference to the last element of
    //! the vector. The vector must not be empty.
    [[nodiscard]] auto back() -> reference
    {
        return (*this)[size() - 1];
    }

    //! Returns a #const_reference to the last element
    //! of the vector. The vector must not be empty.
    [[nodiscard]] auto back() const -> const_reference
    {
        return (*this)[size() - 1];
    }

    //! Returns an #iterator pointing to first element
    //! in the vector or end() if the vector is empty.
    [[nodiscard]] auto begin() noexcept -> iterator
    {
        return iterator{0, this};
    }

    //! Returns a #const_iterator pointing to first element
    //! in the vector or end() if the vector is empty.
    [[nodiscard]] auto begin() const noexcept -> const_iterator
    {
        return const_iterator{0, this};
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

    //! Same as begin() const.
    [[nodiscard]] auto cbegin() const noexcept -> const_iterator
    {
        return begin();
    }

    //! Same as end() const.
    [[nodiscard]] auto cend() const noexcept -> const_iterator
    {
        return end();
    }

    //! Removes all the data from the vector.
    //!
    //! \note This method does not free any space.
    //! Use shrink_to_fit() if you want to free
    //! vector's unused space.
    auto clear() noexcept -> void
    {
        mFile->insert(mIndex, 0, acore::size_type{0});
        mSize = 0;
    }

    //! Same as rbegin() const.
    [[nodiscard]] auto crbegin() const noexcept -> const_reverse_iterator
    {
        return rbegin();
    }

    //! Same as rend() const.
    [[nodiscard]] auto crend() const noexcept -> const_reverse_iterator
    {
        return rend();
    }

    //! Same as insert().
    auto emplace(const_iterator pos, T &&value) -> iterator
    {
        return insert(pos, std::move(value));
    }

    //! Same as push_back() but returns a #reference
    //! to the inserted element.
    auto emplace_back(T &&value) -> reference
    {
        return *insert(cend(), std::move(value));
    }

    //! Returns \c true if the vector is empty.
    [[nodiscard]] constexpr auto empty() const noexcept -> bool
    {
        return mSize == 0;
    }

    //! Returns a valid but not dereferancable
    //! #iterator pointing to the element past the
    //! last one in the vector.
    [[nodiscard]] auto end() noexcept -> iterator
    {
        return iterator{size(), this};
    }

    //! Returns a valid but not dereferancable
    //! #const_iterator pointing to the element past the
    //! last one in the vector.
    [[nodiscard]] auto end() const noexcept -> const_iterator
    {
        return const_iterator{size(), this};
    }

    //! Removes the element pointed to by the \a pos
    //! from the vector and returns an #iterator
    //! pointing to the next element. This method
    //! never reallocates and is thus save to be
    //! used in a loop.
    auto erase(const_iterator pos) -> iterator
    {
        return erase(pos, pos + 1);
    }

    //! Removes the elements from the range \a first
    //! and \a last and returns an #iterator pointing
    //! to the next element after the removed range.
    //! This method never reallocates the vector
    //! and is thus save to be used in a loop.
    auto erase(const_iterator first, const_iterator last) -> iterator
    {
        moveLeft(std::distance<decltype(last)>(cbegin(), last), size(), std::distance<decltype(first)>(cbegin(), first));
        resize(size() - std::distance(first, last));
        auto it = begin();
        std::advance(it, std::distance<decltype(first)>(cbegin(), first));
        return it;
    }

    //! Returns File * holding the data.
    [[nodiscard]] constexpr auto file() const noexcept -> File *
    {
        return mFile;
    }

    //! Returns \c index to the file's value holding the data.
    [[nodiscard]] constexpr auto fileIndex() const noexcept -> acore::size_type
    {
        return mIndex;
    }

    //! Returns a #reference to the first value in the
    //! vector. The vector must not be empty.
    [[nodiscard]] auto front() -> reference
    {
        return (*this)[0];
    }

    //! Returns a #const_reference to the first value in the
    //! vector. The vector must not be empty.
    [[nodiscard]] auto front() const -> const_reference
    {
        return (*this)[0];
    }

    //! Copy constructs the \a value in front of
    //! \a pos. The \a pos must be a valid
    //! #iterator but does not need to be derefarancable
    //! (i.e. end()). Returns #iterator pointing to
    //! the inserted element.
    auto insert(const_iterator pos, const T &value) -> iterator
    {
        return insert(pos, std::move(T{value}));
    }

    //! Move constructs the \a value in place in
    //! front of \a pos. The \a pos must be
    //! a valid #iterator but does not need to be
    //! derefarancable (i.e. end()). Returns #iterator
    //! pointing to the inserted element.
    auto insert(const_iterator pos, T &&value) -> iterator
    {
        const acore::size_type i = static_cast<acore::size_type>(std::distance<decltype(pos)>(cbegin(), pos));
        resizeRealloc(size() + 1);
        iterator it = begin();
        std::advance(it, i);
        moveRight(i, size() - 1, i + 1);
        *it = std::move(value);
        return it;
    }

    //! Inserts \a count instances of \a value
    //! in front of \a pos. The \a pos must
    //! a valid #iterator but does not need to be
    //! derederancable (i.e. end()). Returns #iterator
    //! pointing to the first inserted element.
    auto insert(const_iterator pos, size_type count, const T &value) -> iterator
    {
        const acore::size_type i = static_cast<acore::size_type>(std::distance<decltype(pos)>(cbegin(), pos));
        resizeRealloc(size() + count);
        iterator it = begin();
        std::advance(it, i);
        moveRight(i, size() - count, i + count);
        iterator last = it;
        std::advance(last, count);
        std::fill(it, last, value);
        return it;
    }

    //! Inserts a range of elements from \a first
    //! to \a last in front of \a pos. The \a pos
    //! must a valid #iterator but does not need to be
    //! derederancable (i.e. end()). Returns #iterator
    //! pointing to the first inserted element.
    template<class It, std::enable_if_t<!std::is_same_v<typename std::iterator_traits<It>::iterator_category, void>, int> = 0>
    auto insert(const_iterator pos, It first, It last) -> iterator
    {
        const acore::size_type i = std::distance<decltype(pos)>(cbegin(), pos);
        resizeRealloc(size() + std::distance(first, last));
        iterator it = begin();
        std::advance(it, i);
        moveRight(i, size() - std::distance(first, last), i + std::distance(first, last));
        std::copy(first, last, it);
        return it;
    }

    //! Inserts a range of elements from the \a list
    //! in front of the \a pos. The \a pos
    //! must a valid #iterator but does not need to be
    //! derederancable (i.e. end()). Returns #iterator
    //! pointing to the first inserted element.
    auto insert(const_iterator pos, std::initializer_list<T> values) -> iterator
    {
        const acore::size_type i = std::distance<decltype(pos)>(cbegin(), pos);
        resizeRealloc(size() + values.size());
        iterator it = begin();
        std::advance(it, i);
        moveRight(i, size() - values.size(), i + values.size());
        std::copy(values.begin(), values.end(), it);
        return it;
    }

    //! Returns the last index of \a value or acore::INVALID_INDEX
    //! if the \a value is not in the vector.
    constexpr size_type max_size() const noexcept
    {
        return std::numeric_limits<acore::size_type>::max();
    }

    //! Removes last element from the vector. The
    //! vector must not be empty.
    auto pop_back() -> void
    {
        erase(cend() - 1);
    }

    //! Copy constructs the \a value at the end of
    //! the vector. If \c (capacity() == size() + 1)
    //! the vector will re-allocate.
    auto push_back(const T &value) -> void
    {
        insert(cend(), value);
    }

    //! Move constructs the \a value in place at the
    //! end of the vector. If \c (capacity() == size() + 1)
    //! the vector will re-allocate.
    auto push_back(T &&value) -> void
    {
        insert(cend(), std::move(value));
    }

    //! Returns a #reverse_iterator pointing to the
    //! last element in the vector or rend() if the
    //! vector is empty.
    [[nodiscard]] auto rbegin() noexcept -> reverse_iterator
    {
        return reverse_iterator{end()};
    }

    //! Returns a #const_reverse_iterator pointing to the
    //! last element in the vector or rend() if the
    //! vector is empty.
    [[nodiscard]] auto rbegin() const noexcept -> const_reverse_iterator
    {
        return const_reverse_iterator{end()};
    }

    //! Returns a #const_reverse_iterator pointing to the
    //! past the first element of the vector or rend()
    //! if the vector is empty.
    [[nodiscard]] auto rend() noexcept -> reverse_iterator
    {
        return reverse_iterator{begin()};
    }

    //! Returns a #const_reverse_iterator pointing to the
    //! past the first element of the vector or rend()
    //! if the vector is empty.
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

    //! Changes the size of the vector to the \a count.
    //! If the vector grows the new elements will
    //! be default initialized. This function will
    //! only reallocate if the new size is > capacity().
    auto resize(acore::size_type count) -> void
    {
        resize(count, T{});
    }

    //! Changes the size of the vector to the \a count.
    //! If the vector grows the new elements will
    //! be initialized to \a value. This function will
    //! only reallocate if the new size is > capacity().
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

    //! Frees any allocated space not used by the
    //! elements. Call this method after you have
    //! finished inserting and/or erasing elements
    //! to free unused space.
    auto shrink_to_fit() -> void
    {
        mFile->resize(mIndex, offset(size()));
    }

    //! Returns the \c number of elements in the vector.
    [[nodiscard]] constexpr auto size() const noexcept -> acore::size_type
    {
        return mSize;
    }

    //! Returns a #reference to an element at \a index.
    //! The index must be valid (0 <= index < size()).
    [[nodiscard]] auto operator[](acore::size_type pos) -> reference
    {
        return reference{pos, this};
    }

    //! Returns a #const_reference to an element at \a index.
    //! The index must be valid (0 <= index < size()).
    [[nodiscard]] auto operator[](acore::size_type pos) const -> const_reference
    {
        return const_reference{pos, this};
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

//! Returns \c true if the values of \a left are equal
//! to those of the \a right. Vectors of different sizes
//! are not equal.
template<typename T>
[[nodiscard]] constexpr bool operator==(const PersistentVector<T> &left, const PersistentVector<T> &right) noexcept
{
    return std::equal(left.cbegin(), left.cend(), right.begin(), right.end());
}

//! Returns \c true if the values of \a left are not
//! equal to those of the \a right. Vectors of different
//! sizes are not equal.
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
