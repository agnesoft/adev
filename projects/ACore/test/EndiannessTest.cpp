#include "pch.hpp"

#include <catch2/catch.hpp>

#include <cstring>

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
