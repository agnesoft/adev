#ifndef ACORE_NUMERIC_HPP
#define ACORE_NUMERIC_HPP

#include "ACoreModule.hpp"

#include <cmath>

namespace acore
{
//! Returns the smallest ^2 of the \a value.
//! If the \a value is already ^2 this function
//! simply returns it.
[[nodiscard]] inline auto nextPowerOfTwo(size_type value) -> size_type
{
    return static_cast<size_type>(std::uint64_t{1} << (static_cast<std::uint64_t>(std::log2(value - 1)) + 1));
}
}

#endif
