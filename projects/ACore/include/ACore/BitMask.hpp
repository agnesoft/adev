#ifndef ACORE_BITMASK_HPP
#define ACORE_BITMASK_HPP

#include "ACoreModule.hpp"

namespace acore
{
//! The \c BitMask is a class that provides iteration
//! over set bits in a 16 bit value (std::uint16_t).
//!
//! The value must not be 0. To test whether the value is
//! zero before dereferncing the mask use explicit conversion
//! to bool operator.
//!
//! The class lets you iterate over 0-indexed positions
//! of the set bits in a value. The set bits are iterated
//! from the least significant bit (right-most) to the left.
//!
//! Example:
//!
//! \snippet BitMaskTest.cpp [Usage]
//!
//! The primary usage of this class is to iterate
//! over the result of the acore::match() function.
class BitMask
{
public:
    //! Constructs a BitMask object with \a data.
    explicit constexpr BitMask(std::uint16_t data) noexcept :
        mData(data)
    {
    }

    //! Returns this mask. Used for iteration.
    [[nodiscard]] auto begin() const noexcept -> BitMask
    {
        return *this;
    }

    //! Returns an empty mask. Used for iteration.
    [[nodiscard]] auto end() const noexcept -> BitMask
    {
        return BitMask{0};
    }

    //! Increments the position of the mask to the next
    //! set bit.
    auto operator++() noexcept -> BitMask &
    {
        mData &= (mData - 1);
        return *this;
    }

    //! Returns the index of the current set bit.
    [[nodiscard]] auto operator*() const noexcept -> size_type
    {
#if defined(_MSC_VER) && !defined(__clang__)
        unsigned long index = 0;
        _BitScanForward(&index, mData);
        return static_cast<size_type>(index);
#elif defined(__GNUC__) || defined(__clang__)
        return __builtin_ctz(mData);
#else
        static_assert(false, "Unsupported compiler. Requires a built-in trailing zeroes counting function. (E.g. __builtin_ctz on GCC or Clang)");
#endif
    }

private:
    //! Returns true if the \a left mask is
    //! the same as the \a right.
    [[nodiscard]] friend auto operator==(const BitMask &left, const BitMask &right) -> bool
    {
        return left.mData == right.mData;
    }

    //! Returns true if the \a left mask is
    //! NOT the same as the \a right.
    [[nodiscard]] friend auto operator!=(const BitMask &left, const BitMask &right) -> bool
    {
        return !(left == right);
    }

    std::uint16_t mData = 0;
};

}

#endif
