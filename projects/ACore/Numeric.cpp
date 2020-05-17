#include "Numeric.hpp"

#include <cmath>

namespace acore
{
auto nextPowerOfTwo(size_type value) -> size_type
{
    return static_cast<size_type>(std::uint64_t{1} << (static_cast<std::uint64_t>(std::log2(value - 1)) + 1));
}
}
