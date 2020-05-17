#include "pch.hpp"

#include <Catch2/catch.hpp>

namespace bitmasktest
{
TEST_CASE("BitMask(T data) [acore::BitMask]")
{
    SECTION("[none]")
    {
        REQUIRE(noexcept(acore::BitMask{acore::size_type{0}}));
        constexpr acore::BitMask bitMask{acore::size_type{0}};
        REQUIRE(std::vector<acore::size_type>(bitMask.begin(), bitMask.end()) == std::vector<acore::size_type>{}); //NOLINT(readability-container-size-empty)
    }

    SECTION("[single]")
    {
        constexpr acore::BitMask bitMask{std::uint8_t{0b00010000}};
        REQUIRE(std::vector<acore::size_type>(bitMask.begin(), bitMask.end()) == std::vector<acore::size_type>{4});
    }

    SECTION("[multi]")
    {
        constexpr acore::BitMask bitMask{std::uint16_t{0b0100000101000100}};
        REQUIRE(std::vector<acore::size_type>(bitMask.begin(), bitMask.end()) == std::vector<acore::size_type>{2, 6, 8, 14});
    }

    SECTION("[all]")
    {
        constexpr acore::BitMask bitMask{std::uint32_t{0b11111111111111111111111111111111}};
        REQUIRE(std::vector<acore::size_type>(bitMask.begin(), bitMask.end()) == std::vector<acore::size_type>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31});
    }
}

TEST_CASE("begin() const noexcept -> const_iterator [acore::BitMask]")
{
    SECTION("[none]")
    {
        constexpr acore::BitMask bitMask{acore::size_type{0}};
        REQUIRE(noexcept(bitMask.end()));
        REQUIRE(bitMask.begin() == bitMask.end());
    }

    SECTION("[single]")
    {
        constexpr acore::BitMask bitMask{std::uint16_t{0b1000000000000000}};
        REQUIRE_FALSE(bitMask.begin() == bitMask.end());
        REQUIRE(*bitMask.begin() == 15);
    }

    SECTION("[multi]")
    {
        constexpr acore::BitMask bitMask{std::uint8_t{0b01001010}};
        REQUIRE_FALSE(bitMask.begin() == bitMask.end());
        REQUIRE(*bitMask.begin() == 1);
    }
}

TEST_CASE("end() const noexcept -> const_iterator [acore::BitMask]")
{
    constexpr acore::BitMask bitMask{std::uint8_t{0b01010101}};
    REQUIRE(noexcept(bitMask.end()));
    REQUIRE(bitMask.begin() != bitMask.end());
}

TEST_CASE("none() const -> bool [acore::BitMask]")
{
    SECTION("[none]")
    {
        constexpr acore::BitMask bitMask{std::uint32_t{}};
        REQUIRE(noexcept(bitMask.none()));
        REQUIRE(bitMask.none());
    }

    SECTION("[multi]")
    {
        constexpr acore::BitMask bitMask{acore::size_type{123456789}};
        REQUIRE_FALSE(bitMask.none());
    }
}

TEST_CASE("acore::BitMask<T> [examples]")
{
    SECTION("[usage]")
    {
        // clang-format off
        //! [[Usage]]
constexpr acore::BitMask mask(std::uint16_t{0b0001000110001010});
std::vector<acore::size_type> positions;

for(acore::size_type pos : mask)
{
    positions.push_back(pos);
}

//positions == {1, 3, 7, 8, 12}
        //! [[Usage]]
        // clang-format on
        REQUIRE(positions == std::vector<acore::size_type>{1, 3, 7, 8, 12});
    }
}
}
