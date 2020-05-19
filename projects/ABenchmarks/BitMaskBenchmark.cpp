#include "pch.hpp"

#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <Catch2/catch.hpp>

#include <bitset>

namespace abenchmarks
{
TEST_CASE("BitMask")
{
    constexpr std::uint16_t value = 0b1001000100010001;

    BENCHMARK("[acore::BitMask]")
    {
        acore::BitMask mask{value};
        acore::size_type positions = 0;

        for (acore::size_type pos : mask)
        {
            positions += pos;
        }

        return positions;
    };

    BENCHMARK("std::bitset")
    {
        std::bitset<sizeof(std::uint16_t) * CHAR_BIT> mask{value};
        acore::size_type positions = 0;

        for (size_t i = 0; i < mask.size(); ++i)
        {
            if (mask[i])
            {
                positions += static_cast<acore::size_type>(i);
            }
        }

        return positions;
    };
}
}
