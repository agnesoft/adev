#ifndef ACORE_BITMASK_HPP
#define ACORE_BITMASK_HPP

#include "ForwardIterator.hpp"

#include <bitset>
#include <climits>

namespace acore
{
//! The \c BitMask<T> is a template class that provides
//! const iteration over the set bits in a value.
//!
//! The class lets you iterate over 0-indexed positions
//! of the set bits in a value of type \c T. The set
//! bits are iterated from the least significant bit
//! (right-most) to the left.
//!
//! Example:
//!
//! \snippet BitMaskTest.cpp [Usage]
//!
//! The primary use case of this class is to iterate
//! over the result of the acore::match() function.
template<typename T>
class BitMask
{
public:
    //! Immutable iterator type.
    using const_iterator = ForwardIterator<size_type, size_type, const BitMask>;

    //! Constructs a BitMask object with \a data.
    explicit constexpr BitMask(T data) noexcept :
        mData(data)
    {
    }

    //! Returns a #const_iterator pointing to the
    //! position of the first set bit or end() if
    //! no bit is set.
    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator
    {
        return ++end();
    }

    //! Returns an invalid #const_iterator.
    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator
    {
        return const_iterator{INVALID_INDEX, this};
    }

    //! Returns \c true if no bit is set in the
    //! underlying value.
    [[nodiscard]] constexpr auto none() const noexcept -> bool
    {
        return mData.none();
    }

private:
    friend const_iterator;

    [[nodiscard]] constexpr auto nextIndex(size_type index) const noexcept -> size_type
    {
        const auto size = static_cast<size_type>(mData.size());

        while (++index < size)
        {
            if (mData[index])
            {
                return index;
            }
        }

        return INVALID_INDEX;
    }

    [[nodiscard]] constexpr auto referenceAt(size_type index) const noexcept -> size_type
    {
        return index;
    }

    std::bitset<sizeof(T) * CHAR_BIT> mData;
};
}

#endif
