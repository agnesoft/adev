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

#ifndef ACORE_RANDOMACCESSITERATOR_HPP
#define ACORE_RANDOMACCESSITERATOR_HPP

#include "ACoreModule.hpp"

#include <cstddef>
#include <iterator>

namespace acore
{
//! The RandomAccessIterator is a template class that
//! provides STL style random access iterator.
//!
//! It lets you iterate over the elements of any \c Iterable
//! that implements the following methods:
//!
//! - <tt> acore::size_type %distance(acore::size_type left, acore::size_type right) const </tt>
//! - <tt> acore::size_type %nextIndex(acore::size_type index, acore::size_type steps) const </tt>
//! - <tt> acore::size_type %previousIndex(acore::size_type index, acore::size_type steps) const </tt>
//! - <tt> ReferenceType %referenceAt(acore::size_type index) </tt>
//!
//! The first method is used to get the distance
//! between two indexes. The second one is to increment
//! the iterator by \c steps amount. The third is
//! to decrement the iterator by \c steps amount.
//! The fourth is used to dereference the iterator
//! and to obtain the \c ReferenceType at the
//! current index.
template<typename ValueType, typename ReferenceType, typename Iterable>
class RandomAccessIterator
{
public:
    //! Synonym for the \c ValueType template argument.
    using value_type = ValueType;

    //! Synonym for the #value_type*.
    using pointer = value_type *;

    //! Synonym for the \c ReferenceType template argument.
    using reference = ReferenceType;

    //! Synonym for the <a href="http://en.cppreference.com/w/cpp/types/ptrdiff_t">ptrdiff_t</a>.
    using difference_type = std::ptrdiff_t;

    //! Synonym for the <a href="http://en.cppreference.com/w/cpp/iterator/iterator_tags">std::random_access_iterator_tag</a>.
    using iterator_category = std::random_access_iterator_tag;

    //! Constructs an invalid iterator that points
    //! to index acore::INVALID_INDEX and no \c iterable.
    constexpr RandomAccessIterator() noexcept = default;

    //! Constructs an iterator with \a index
    //! and \a iterable.
    constexpr RandomAccessIterator(size_type index, Iterable *data) noexcept :
        mIndex(index),
        mIterable(data)
    {
    }

    //! Returns the \c index this iterator points to.
    [[nodiscard]] constexpr auto index() const noexcept -> size_type
    {
        return mIndex;
    }

    //! Returns the \c iterable this iterator operators on.
    [[nodiscard]] constexpr auto iterable() const noexcept -> Iterable *
    {
        return mIterable;
    }

    //! Returns copy of itself searched forward
    //! by \a index steps.
    [[nodiscard]] constexpr auto operator+(size_type index) const -> RandomAccessIterator
    {
        auto it = *this;
        it += index;
        return it;
    }

    //! Searches forward by \a index steps for the
    //! next valid index in the underlying \c iterable
    //! and returns a \c reference to itself.
    constexpr auto operator+=(size_type index) -> RandomAccessIterator &
    {
        mIndex = mIterable->nextIndex(mIndex, index);
        return *this;
    }

    //! Searches forward for the position of the
    //! next valid index in the underlying \c iterable
    //! and returns a \c reference to itself.
    constexpr auto operator++() -> RandomAccessIterator &
    {
        return (*this += 1);
    }

    //! Same as operator++() but returns the copy
    //! of itself before the search.
    [[nodiscard]] constexpr auto operator++(int) & -> RandomAccessIterator
    {
        auto it = *this;
        ++(*this);
        return it;
    }

    //! Returns the \c distance between this object
    //! and the \a other. If this object is ahead
    //! of the \a other the distance will be negative.
    [[nodiscard]] constexpr auto operator-(RandomAccessIterator other) const -> difference_type
    {
        return static_cast<difference_type>(mIterable->distance(mIndex, other.mIndex));
    }

    //! Returns copy of itself searched backward
    //! by \a index steps.
    [[nodiscard]] constexpr auto operator-(size_type index) const -> RandomAccessIterator
    {
        auto it = *this;
        it -= index;
        return it;
    }

    //! Searches backward by \a index steps for the
    //! previous valid index in the underlying
    //! \c iterable and returns a \c reference to itself.
    constexpr auto operator-=(size_type index) -> RandomAccessIterator &
    {
        mIndex = mIterable->previousIndex(mIndex, index);
        return *this;
    }

    //! Searches backward for the position of the
    //! previous valid index in the underlying
    //! \c iterable and returns a \c reference to itself.
    constexpr auto operator--() -> RandomAccessIterator &
    {
        return (*this -= 1);
    }

    //! Same as operator--() but returns the copy
    //! of itself before the search.
    [[nodiscard]] constexpr auto operator--(int) & -> RandomAccessIterator
    {
        auto it = *this;
        --(*this);
        return it;
    }

    //! Returns #reference to the value pointed to
    //! by the current index.
    [[nodiscard]] constexpr auto operator*() const -> reference
    {
        return mIterable->referenceAt(mIndex);
    }

    //! Returns the #reference to the element
    //! pointed to by the current index plus the
    //! \a index.
    //!
    //! \note Provides array semantics.
    [[nodiscard]] constexpr auto operator[](size_type index) const -> reference
    {
        return mIterable->referenceAt(mIndex + index);
    }

    //! Returns \c true if the \a right points to
    //! an element after the element pointed to
    //! by this object.
    [[nodiscard]] constexpr auto operator<(RandomAccessIterator right) const noexcept -> bool
    {
        return mIndex < right.mIndex;
    }

    //! Returns \c true if the \a right points to
    //! an element after or equal to the element
    //! pointed to by this object.
    [[nodiscard]] constexpr auto operator<=(RandomAccessIterator right) const noexcept -> bool
    {
        return mIndex <= right.mIndex;
    }

    //! Returns \c true if the \a right points to
    //! an element before the element pointed to
    //! by this object.
    [[nodiscard]] constexpr auto operator>(RandomAccessIterator right) const noexcept -> bool
    {
        return mIndex > right.mIndex;
    }

    //! Returns \c true if the \a right points to
    //! an element before or equal to the element
    //! pointed to by this object.
    [[nodiscard]] constexpr auto operator>=(RandomAccessIterator right) const noexcept -> bool
    {
        return mIndex >= right.mIndex;
    }

    //! Converts the iterator implicitly to another
    //! using the different but compatible template
    //! arguments. This operator is typically used
    //! to convert from the non-const to the \c const
    //! version of this iterator.
    template<typename ConstReferenceType>
    [[nodiscard]] constexpr operator RandomAccessIterator<const ValueType, ConstReferenceType, const Iterable>() const noexcept //NOLINT(google-explicit-constructor)
    {
        return RandomAccessIterator<const ValueType, ConstReferenceType, const Iterable>(mIndex, mIterable);
    }

private:
    size_type mIndex = INVALID_INDEX;
    Iterable *mIterable = nullptr;
};

//! \relates RandomAccessIterator
//! Returns \c true if the \a left points to the
//! same position in the same iterable as the \a right.
template<typename ValueType, typename ReferenceType, typename Iterable, typename OtherValueType, typename OtherReferenceType, typename OtherIterable>
[[nodiscard]] constexpr auto operator==(RandomAccessIterator<ValueType, ReferenceType, Iterable> left, RandomAccessIterator<OtherValueType, OtherReferenceType, OtherIterable> right) noexcept -> bool
{
    return left.iterable() == right.iterable() && left.index() == right.index();
}

//! \relates RandomAccessIterator
//! Returns \c true if the \a left does not point
//! to the same position in the same iterable as the
//! \a right.
template<typename ValueType, typename ReferenceType, typename Iterable, typename OtherValueType, typename OtherReferenceType, typename OtherIterable>
[[nodiscard]] constexpr auto operator!=(RandomAccessIterator<ValueType, ReferenceType, Iterable> left, RandomAccessIterator<OtherValueType, OtherReferenceType, OtherIterable> right) noexcept -> bool
{
    return !(left == right);
}
}

#endif
