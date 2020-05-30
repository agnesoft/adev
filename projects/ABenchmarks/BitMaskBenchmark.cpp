// Copyright 2020 Michael Vlach
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
