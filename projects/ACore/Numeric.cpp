#include "Numeric.hpp"

namespace acore
{
auto nextPowerOfTwo(std::int64_t value) -> std::int64_t
{
    return static_cast<std::int64_t>(std::uint64_t(1) << (static_cast<std::uint64_t>(std::log2(value - 1)) + 1));
}
}
