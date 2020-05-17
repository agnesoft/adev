#ifndef ACORE_FORWARDITERATOR_HPP
#define ACORE_FORWARDITERATOR_HPP

#include "ACoreModule.hpp"

#include <iterator>

namespace acore
{
//! The ForwardIterator is a template class that
//! provides STL-style forward iterator.
//!
//! The ForwardIterator supports only forward iteration
//! in single steps. It requires the \c Iterable
//! template argument type to implement the following
//! methods:
//!
//! - <tt> std::int64_t %nextIndex(std:int64_t index) const </tt>
//! - <tt> ReferenceType %referenceAt(std::int64_t index) const </tt>
//!
//! The first method is used to increment the iterator.
//! The second one is used to dereference it and to
//! obtain the \c ReferenceType at the current index.
template<typename ValueType, typename ReferenceType, typename Iterable>
class ForwardIterator
{
public:
    //! Synonym for the \c ValueType template argument.
    using value_type = ValueType;

    //! Synonym for the #value_type *.
    using pointer = value_type *;

    //! Synonym for the \c ReferenceType template argument.
    using reference = ReferenceType;

    //! Synonym for the <a href="http://en.cppreference.com/w/cpp/types/ptrdiff_t">ptrdiff_t</a>.
    using difference_type = std::ptrdiff_t;

    //! Synonym for the <a href="http://en.cppreference.com/w/cpp/iterator/iterator_tags">std::forward_iterator_tag</a>.
    using iterator_category = std::forward_iterator_tag;

    //! Constructs an invalid iterator that points
    //! to index acore::INVALID_INDEX and no \c iterable.
    constexpr ForwardIterator() noexcept = default;

    //! Constructs an iterator with \a index
    //! and \a iterable.
    constexpr ForwardIterator(size_type index, Iterable *iterable) noexcept :
        mIndex(index),
        mIterable(iterable)
    {
    }

    //! Returns the \c index this iterator points to.
    [[nodiscard]] constexpr size_type index() const noexcept
    {
        return mIndex;
    }

    //! Returns the \c iterable this iterator operators on.
    [[nodiscard]] constexpr Iterable *iterable() const noexcept
    {
        return mIterable;
    }

    //! Searches forward for the position of the
    //! next valid index in the underlying \c iterable
    //! and returns a \c reference to itself.
    constexpr ForwardIterator &operator++()
    {
        mIndex = mIterable->nextIndex(mIndex);
        return *this;
    }

    //! Same as operator++() but returns the copy
    //! of itself before the search.
    [[nodiscard]] constexpr ForwardIterator operator++(int) &
    {
        ForwardIterator it = *this;
        ++(*this);
        return it;
    }

    //! Returns #reference to the value pointed to
    //! by the current index.
    [[nodiscard]] constexpr reference operator*() const
    {
        return mIterable->referenceAt(mIndex);
    }

    //! Converts the iterator implicitly to another
    //! using the different but compatible template
    //! arguments. This operator is typically used
    //! to convert from the non-const to the \c const
    //! version of this iterator.
    template<typename ConstReferenceType>
    [[nodiscard]] constexpr operator ForwardIterator<ValueType, ConstReferenceType, const Iterable>() const noexcept
    {
        return ForwardIterator<ValueType, ConstReferenceType, const Iterable>(mIndex, mIterable);
    }

private:
    size_type mIndex = INVALID_INDEX;
    Iterable *mIterable = nullptr;
};

//! \relates ForwardIterator
//! Returns \c true if the \a left points to the
//! same position in the same iterable as the \a right.
template<typename T, typename ReferenceT, typename Iterable>
[[nodiscard]] constexpr bool operator==(ForwardIterator<T, ReferenceT, Iterable> left, ForwardIterator<T, ReferenceT, Iterable> right) noexcept
{
    return left.index() == right.index() && left.iterable() == right.iterable();
}

//! \relates ForwardIterator
//! Returns \c true if the \a left does not point
//! to the same position in the same iterable as the
//! \a right.
template<typename T, typename ReferenceT, typename Iterable>
[[nodiscard]] constexpr bool operator!=(ForwardIterator<T, ReferenceT, Iterable> left, ForwardIterator<T, ReferenceT, Iterable> right) noexcept
{
    return !(left == right);
}
}

#endif
