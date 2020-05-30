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

#include <catch2/catch.hpp>

namespace bitmasktest
{
TEST_CASE("BitMask(T data) noexcept [acore::BitMask]")
{
    std::vector<acore::size_type> positions;

    SECTION("[none]")
    {
        REQUIRE(noexcept(acore::BitMask{std::uint16_t{0}}));
        acore::BitMask bitMask{std::uint16_t{0}};

        for (acore::size_type pos : bitMask)
        {
            positions.push_back(pos);
        }

        REQUIRE(positions == std::vector<acore::size_type>{}); //NOLINT(readability-container-size-empty)
    }

    SECTION("[single]")
    {
        acore::BitMask bitMask{std::uint16_t{0b0000000000010000}}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

        for (acore::size_type pos : bitMask)
        {
            positions.push_back(pos);
        }

        REQUIRE(positions == std::vector<acore::size_type>{4});
    }

    SECTION("[multi]")
    {
        acore::BitMask bitMask{std::uint16_t{0b0100000101000100}}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

        for (acore::size_type pos : bitMask)
        {
            positions.push_back(pos);
        }
        REQUIRE(positions == std::vector<acore::size_type>{2, 6, 8, 14});
    }

    SECTION("[all]")
    {
        acore::BitMask bitMask{std::uint16_t{0b1111111111111111}}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

        for (acore::size_type pos : bitMask)
        {
            positions.push_back(pos);
        }

        REQUIRE(positions == std::vector<acore::size_type>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});
    }
}

TEST_CASE("begin() const noexcept -> BitMask [acore::BitMask]")
{
    acore::BitMask bitMask{0b0001000100010001}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    REQUIRE(noexcept(bitMask.begin()));
    REQUIRE(bitMask.begin() == bitMask);
}

TEST_CASE("end() const noexcept -> BitMask [acore::BitMask]]")
{
    acore::BitMask bitMask{0b0001000100010001}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    REQUIRE(noexcept(bitMask.end()));
    REQUIRE_FALSE(bitMask.end() == bitMask);
    REQUIRE(bitMask.end() == acore::BitMask{0});
}

TEST_CASE("operator*() const noexcept -> size_type [acore::BitMask]")
{
    SECTION("[none]")
    {
        acore::BitMask bitMask{std::uint16_t{0}};
        REQUIRE(noexcept(*bitMask));
    }

    SECTION("[single]")
    {
        acore::BitMask bitMask{std::uint16_t{0b1000000000000000}}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(*(bitMask) == 15);
    }

    SECTION("[multi]")
    {
        acore::BitMask bitMask{std::uint16_t{0b0000000001001010}}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(*(bitMask) == 1);
    }
}

TEST_CASE("operator++() noexcept -> BitMask & [acore::BitMask]")
{
    SECTION("[multiple]")
    {
        acore::BitMask bitMask{std::uint16_t{0b0000000000101000}}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(noexcept(++bitMask));
        REQUIRE(*(++bitMask) == 5);
    }

    SECTION("[all]")
    {
        acore::BitMask bitMask{std::uint16_t{0b1111111111111111}}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(noexcept(++bitMask));
        REQUIRE(*(++bitMask) == 1);
    }
}

TEST_CASE("operator==(const BitMask &left, const BitMask &right) noexcept -> bool [acore::BitMask]")
{
    SECTION("[equal]")
    {
        acore::BitMask bitMask{0b0000000010000000}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        acore::BitMask other{0b0000000010000000}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(bitMask == other);
    }

    SECTION("[different]")
    {
        acore::BitMask bitMask{0b0000000010000000}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        acore::BitMask other{0b0000100010000001}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE_FALSE(bitMask == other);
    }
}

TEST_CASE("operator!=(const BitMask &left, const BitMask &right) noexcept -> bool [acore::BitMask]")
{
    SECTION("[equal]")
    {
        acore::BitMask bitMask{0b0000000010000000}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        acore::BitMask other{0b0000000010000000}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE_FALSE(bitMask != other);
    }

    SECTION("[different]")
    {
        acore::BitMask bitMask{0b0000000010000000}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        acore::BitMask other{0b0000100010000001}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(bitMask != other);
    }
}

TEST_CASE("acore::BitMask<T> [examples]")
{
    SECTION("[usage]")
    {
        // clang-format off
        //! [[Usage]]
const std::uint16_t value{0b0001000110001010};
acore::BitMask mask{value};
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
