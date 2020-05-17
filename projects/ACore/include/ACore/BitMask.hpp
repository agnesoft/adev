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
    [[nodiscard]] constexpr const_iterator begin() const noexcept
    {
        return ++end();
    }

    //! Returns an invalid #const_iterator.
    [[nodiscard]] constexpr const_iterator end() const noexcept
    {
        return const_iterator(INVALID_INDEX, this);
    }

    //! Returns \c true if no bit is set in the
    //! underlying value.
    [[nodiscard]] constexpr bool none() const noexcept
    {
        return mData.none();
    }

private:
    friend const_iterator;

    [[nodiscard]] constexpr size_type nextIndex(size_type index) const noexcept
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

    [[nodiscard]] constexpr size_type referenceAt(size_type index) const noexcept
    {
        return index;
    }

    std::bitset<sizeof(T) * CHAR_BIT> mData;
};
}

#endif
