#include "Endianness.hpp"

#include <cstring>

namespace acore
{
bool Endianness::littleEndianness = []() noexcept { //NOLINT(cert-err58-cpp)
    const std::uint16_t i = 1;
    char firstByte = 0;
    std::memcpy(&firstByte, &i, 1U);
    return firstByte == char(1);
}();
}
