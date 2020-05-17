#ifndef ACORE_NUMERIC_HPP
#define ACORE_NUMERIC_HPP

#include "ACoreModule.hpp"

namespace acore
{
//! Returns the smallest ^2 of the \a value.
//! If the \a value is already ^2 this function
//! simply returns it.
[[nodiscard]] auto nextPowerOfTwo(size_type value) -> size_type;
}

#endif
