#include "pch.hpp"

#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <bitset>
#include <climits>

namespace bitmaskbenchmark
{

TEST_CASE("BitMask")
{
    int acoreBitMask = 0;
    int stdBitSet = 0;
    std::uint16_t value = 0b0001000100000010; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    BENCHMARK_ADVANCED("[acore::BitMask]")
    (Catch::Benchmark::Chronometer meter)
    {
        acoreBitMask = meter.runs();
        meter.measure([&] {
            acore::BitMask mask{value};
            acore::size_type positions = 0;

            for (acore::size_type pos : mask)
            {
                positions += pos;
            }

            return positions;
        });
    };

    BENCHMARK_ADVANCED("[std::bitset]")
    (Catch::Benchmark::Chronometer meter)
    {
        stdBitSet = meter.runs();
        meter.measure([&] {
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
        });
    };

    REQUIRE(acoreBitMask > stdBitSet);
}
}
