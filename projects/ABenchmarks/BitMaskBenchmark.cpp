#include "pch.hpp"

#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <Catch2/catch.hpp>

namespace abenchmarks
{
template<typename T>
class AbseilBitMask
{
public:
    explicit AbseilBitMask(T value) noexcept :
        mValue{value}
    {
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return mValue != 0;
    }

    [[nodiscard]] auto operator++() noexcept -> AbseilBitMask &
    {
        mValue &= (mValue - 1);
        return *this;
    }

    [[nodiscard]] auto operator*() const -> int
    {
#if defined(_MSC_VER) && !defined(__clang__)
        unsigned long result = 0;
        _BitScanForward(&result, mValue);
        return result;
#else
        return __builtin_ctz(mValue);
#endif
    }

private:
    T mValue = 0;
};

TEST_CASE("acore::BitMask")
{
    BENCHMARK("[acore::BitMask]")
    {
        acore::size_type positions = 0;
        for (acore::size_type i : acore::BitMask<std::uint16_t>{0b0010001011000101})
        {
            positions += i;
        }
        return positions;
    };

    BENCHMARK("[abseil::BitMask]")
    {
        auto positions = 0;
        AbseilBitMask<std::uint16_t> mask{0b0010001011000101};
        while (++mask)
        {
            positions += *mask;
        }
        return positions;
    };
}
}
