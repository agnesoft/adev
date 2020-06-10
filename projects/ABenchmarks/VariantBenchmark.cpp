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

#include <any>
#include <unordered_map>
#include <variant>

namespace variantbenchmark
{
TEST_CASE("Variant")
{
    int acoreVariant = 0;
    int stdAny = 0;
    int stdVariant = 0;
    std::string value{"This string is not exactly short and will be used for benchmarking!"};
    using ValueType = decltype(value);

    BENCHMARK_ADVANCED("[acore::Variant]")
    (Catch::Benchmark::Chronometer meter)
    {
        acoreVariant = meter.runs();
        meter.measure([&] {
            acore::Variant var{value};
            return var.value<ValueType>();
        });
    };

    BENCHMARK_ADVANCED("[std::any]")
    (Catch::Benchmark::Chronometer meter)
    {
        stdAny = meter.runs();
        meter.measure([&] {
            std::any variant{value};
            return std::any_cast<ValueType>(variant);
        });
    };

    BENCHMARK_ADVANCED("[std::variant]")
    (Catch::Benchmark::Chronometer meter)
    {
        stdVariant = meter.runs();
        meter.measure([&] {
            std::variant<ValueType> variant{value};
            return std::get<ValueType>(variant);
        });
    };

    constexpr double benchmarkFactor = 3;
    REQUIRE(acoreVariant > (stdAny / benchmarkFactor));
    REQUIRE(acoreVariant > (stdVariant / benchmarkFactor));
}
}
