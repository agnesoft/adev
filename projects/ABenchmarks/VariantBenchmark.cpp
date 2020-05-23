#include "pch.hpp"

#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <Catch2/catch.hpp>

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

    REQUIRE(acoreVariant > (stdAny / 2));
    REQUIRE(acoreVariant > (stdVariant / 2));
}
}
