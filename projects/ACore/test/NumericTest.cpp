#include "pch.hpp"

#include <catch2/catch.hpp>

namespace numerictest
{
TEST_CASE("nextPowerOfTwo(size_type value) -> size_type [ACore]")
{
    REQUIRE(acore::nextPowerOfTwo(0) == std::int64_t{2});
    REQUIRE(acore::nextPowerOfTwo(1) == std::int64_t{2});
    REQUIRE(acore::nextPowerOfTwo(3) == std::int64_t{4});
    REQUIRE(acore::nextPowerOfTwo(5) == std::int64_t{8});
    REQUIRE(acore::nextPowerOfTwo(9) == std::int64_t{16});
    REQUIRE(acore::nextPowerOfTwo(4294967297) == std::int64_t{8589934592});
}
}
