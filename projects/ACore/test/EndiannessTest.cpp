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

namespace endiannesstest
{
class EndiannessGuard
{
public:
    EndiannessGuard() :
        mEndianness(acore::Endianness::littleEndianness)
    {
    }

    EndiannessGuard(const EndiannessGuard &other) = delete;
    EndiannessGuard(EndiannessGuard &&other) noexcept = delete;

    ~EndiannessGuard()
    {
        acore::Endianness::littleEndianness = mEndianness;
    }

    auto operator=(const EndiannessGuard &other) -> EndiannessGuard & = delete;
    auto operator=(EndiannessGuard &&other) noexcept -> EndiannessGuard & = delete;

private:
    bool mEndianness = true;
};

TEST_CASE("acore::bswap()")
{
    REQUIRE(acore::bswap<std::uint16_t>(0) == 0);
    REQUIRE(acore::bswap<std::uint16_t>(0b10000000'00001100) == 0b00001100'10000000);
    REQUIRE(acore::bswap<std::uint32_t>(0b10000000'01100000'00000000'00011001) == 0b00011001'00000000'01100000'10000000);
    REQUIRE(acore::bswap<std::uint64_t>(0b10000000'00000100'00100000'00011100'00000000'00001111'00010000'00110000) == 0b00110000'00010000'00001111'00000000'00011100'00100000'00000100'10000000);
}

TEST_CASE("acore::fromLittleEndian()")
{
    const EndiannessGuard guard;

    SECTION("[big endian]")
    {
        acore::Endianness::littleEndianness = false;

        REQUIRE(acore::fromLittleEndian<std::uint16_t>(0) == 0);
        REQUIRE(acore::fromLittleEndian<std::uint16_t>(0b10000000'00001100) == 0b00001100'10000000);
        REQUIRE(acore::fromLittleEndian<std::uint32_t>(0b10000000'01100000'00000000'00011001) == 0b00011001'00000000'01100000'10000000);
        REQUIRE(acore::fromLittleEndian<std::uint64_t>(0b10000000'00000100'00100000'00011100'00000000'00001111'00010000'00110000) == 0b00110000'00010000'00001111'00000000'00011100'00100000'00000100'10000000);
    }

    SECTION("[little endian]")
    {
        acore::Endianness::littleEndianness = true;

        REQUIRE(acore::fromLittleEndian<std::uint16_t>(0) == 0);
        REQUIRE(acore::fromLittleEndian<std::uint16_t>(0b10000000'00001100) == 0b10000000'00001100);
        REQUIRE(acore::fromLittleEndian<std::uint32_t>(0b10000000'01100000'00000000'00011001) == 0b10000000'01100000'00000000'00011001);
        REQUIRE(acore::fromLittleEndian<std::uint64_t>(0b10000000'00000100'00100000'00011100'00000000'00001111'00010000'00110000) == 0b10000000'00000100'00100000'00011100'00000000'00001111'00010000'00110000);
    }
}

TEST_CASE("acore::toLittleEndian()")
{
    const EndiannessGuard guard;

    SECTION("[big endian]")
    {
        acore::Endianness::littleEndianness = false;

        REQUIRE(acore::toLittleEndian<std::uint16_t>(0) == 0);
        REQUIRE(acore::toLittleEndian<std::uint16_t>(0b10000000'00001100) == 0b00001100'10000000);
        REQUIRE(acore::toLittleEndian<std::uint32_t>(0b10000000'01100000'00000000'00011001) == 0b00011001'00000000'01100000'10000000);
        REQUIRE(acore::toLittleEndian<std::uint64_t>(0b10000000'00000100'00100000'00011100'00000000'00001111'00010000'00110000) == 0b00110000'00010000'00001111'00000000'00011100'00100000'00000100'10000000);
    }

    SECTION("[little endian]")
    {
        acore::Endianness::littleEndianness = true;

        REQUIRE(acore::toLittleEndian<std::uint16_t>(0) == 0);
        REQUIRE(acore::toLittleEndian<std::uint16_t>(0b10000000'00001100) == 0b10000000'00001100);
        REQUIRE(acore::toLittleEndian<std::uint32_t>(0b10000000'01100000'00000000'00011001) == 0b10000000'01100000'00000000'00011001);
        REQUIRE(acore::toLittleEndian<std::uint64_t>(0b10000000'00000100'00100000'00011100'00000000'00001111'00010000'00110000) == 0b10000000'00000100'00100000'00011100'00000000'00001111'00010000'00110000);
    }
}

TEST_CASE("acore::littleEndian()")
{
    const std::uint16_t i = 1;
    char c = 0;
    std::memcpy(&c, &i, 1);
    const bool isLittle = c == char{1};

    REQUIRE(acore::littleEndian() == isLittle);
}
}
