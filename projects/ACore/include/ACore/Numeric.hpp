#ifndef ACORE_NUMERIC_HPP
#define ACORE_NUMERIC_HPP

#include <cmath>
#include <cstdint>

namespace acore
{
//! Returns the smallest ^2 of the \a value.
//! If the \a value is already ^2 this function
//! simply returns it.
[[nodiscard]] inline auto nextPowerOfTwo(std::int64_t value) -> std::int64_t;
}

#endif
