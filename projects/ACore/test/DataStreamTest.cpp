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

#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace datastreamtest
{
TEST_CASE("[acore::DataStream]")
{
    REQUIRE(std::is_default_constructible_v<acore::DataStream>);
    REQUIRE(std::is_copy_constructible_v<acore::DataStream>);
    REQUIRE(std::is_nothrow_move_constructible_v<acore::DataStream>);
    REQUIRE(std::is_copy_assignable_v<acore::DataStream>);
    REQUIRE(std::is_nothrow_move_assignable_v<acore::DataStream>);
    REQUIRE(std::is_nothrow_destructible_v<acore::DataStream>);
}

TEST_CASE("DataStream() [acore::DataStream]")
{
    const acore::DataStream stream;
    REQUIRE(stream.buffer().data() == std::vector<char>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("DataStream(std::vector<char> data) [acore::DataStream]")
{
    acore::DataStream stream{{'a', 'b', 'c'}};
    REQUIRE(stream.buffer().data() == std::vector<char>{'a', 'b', 'c'});
}

TEST_CASE("buffer() noexcept -> Buffer & [acore::DataStream]")
{
    acore::DataStream stream;
    REQUIRE(noexcept(stream.buffer()));
    REQUIRE(stream.buffer().data() == std::vector<char>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("buffer() const noexcept -> Buffer & [acore::DataStream]")
{
    const acore::DataStream stream;
    REQUIRE(noexcept(stream.buffer()));
    REQUIRE(stream.buffer().data() == std::vector<char>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("pos() const noexcept -> size_type [acore::DataStream]")
{
    SECTION("[empty]")
    {
        const acore::DataStream stream;
        REQUIRE(noexcept(stream.pos()));
        REQUIRE(stream.pos() == 0);
    }

    SECTION("[data]")
    {
        acore::DataStream stream;
        stream.write("Hello world!", 12); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(std::as_const(stream).pos() == 12);
    }
}

TEST_CASE("read(char *data, size_type count) -> void [acore::DataStream]")
{
    SECTION("[empty]")
    {
        acore::DataStream stream;
        std::array<char, 10> buf{}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE_THROWS_AS(stream.read(buf.data(), 5), acore::Exception);
    }

    SECTION("[in bounds]")
    {
        acore::DataStream stream;
        stream.buffer().data().assign({'A', 'b', 'c', 'd', 'e', 'f', 'g', 'H'});
        std::vector<char> buf(3);
        stream.seek(2);
        stream.read(buf.data(), 3);
        REQUIRE(buf == std::vector<char>{'c', 'd', 'e'});
    }

    SECTION("[out of bounds]")
    {
        acore::DataStream stream;
        stream.buffer().data().assign({'1', '2', '3'});
        std::vector<char> buf(5); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE_THROWS_AS(stream.read(buf.data(), 5), acore::Exception);
    }
}

TEST_CASE("reset() -> void [acore::DataStream]")
{
    acore::DataStream stream;
    stream.write("hello world", 11); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    stream.reset();
    REQUIRE(stream.pos() == 0);
}

TEST_CASE("seek(size_type) -> void [acore::DataStream]")
{
    acore::DataStream stream;
    stream.seek(11); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    REQUIRE(stream.pos() == 11);
}

TEST_CASE("write(const char *data, size_type count) -> void [acore::DataStream]")
{
    SECTION("[empty]")
    {
        acore::DataStream stream;
        stream.write("Hello World!", 12); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(stream.buffer().data() == std::vector<char>{'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'});
    }

    SECTION("[data]")
    {
        acore::DataStream stream;
        stream.write("Hello World!", 12); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

        SECTION("[in bounds]")
        {
            stream.seek(6); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            stream.write("There", 5); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(stream.buffer().data() == std::vector<char>{'H', 'e', 'l', 'l', 'o', ' ', 'T', 'h', 'e', 'r', 'e', '!'});
        }

        SECTION("[out of bounds]")
        {
            stream.seek(11); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            stream.write(" Test!", 6); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(stream.buffer().data() == std::vector<char>{'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', ' ', 'T', 'e', 's', 't', '!'});
        }

        SECTION("[invalid pos]")
        {
            stream.seek(-1);
            REQUIRE_THROWS_AS(stream.write("Hello", 5), acore::Exception);
        }
    }
}

TEST_CASE("operator<<(DataStream &stream, const T &value) -> DataStream &, operator>>(DataStream &stream, T &value) -> DataStream & [acore::DataStream]")
{
    acore::DataStream stream;

    SECTION("bool")
    {
        stream << true << false;
        REQUIRE(stream.buffer().size() == static_cast<acore::size_type>(sizeof(bool) + sizeof(bool)));
        stream.reset();
        bool t = false;
        bool f = true;
        stream >> t >> f;
        REQUIRE(t);
        REQUIRE(f == false);
    }

    SECTION("char")
    {
        stream << 'A' << 'B';
        REQUIRE(stream.buffer().size() == static_cast<acore::size_type>(sizeof(char) + sizeof(char)));
        stream.reset();
        char c = 0;
        char d = 0;
        stream >> c >> d;
        REQUIRE(c == 'A');
        REQUIRE(d == 'B');
    }

    SECTION("std::uint8_t, std::int8_t")
    {
        stream << std::numeric_limits<std::uint8_t>::max() << std::numeric_limits<std::int8_t>::min();
        REQUIRE(stream.buffer().size() == static_cast<acore::size_type>(sizeof(std::uint8_t) + sizeof(std::uint8_t)));
        stream.reset();
        std::uint8_t i = 0;
        std::int8_t j = 0;
        stream >> i >> j;
        REQUIRE(i == std::numeric_limits<std::uint8_t>::max());
        REQUIRE(j == std::numeric_limits<std::int8_t>::min());
    }

    SECTION("std::uint16_t, std::int16_t")
    {
        stream << std::numeric_limits<std::uint16_t>::max() << std::numeric_limits<std::int16_t>::min();
        REQUIRE(stream.buffer().size() == static_cast<acore::size_type>(sizeof(std::uint16_t) + sizeof(std::uint16_t)));
        stream.reset();
        std::uint16_t i = 0;
        std::int16_t j = 0;
        stream >> i >> j;
        REQUIRE(i == std::numeric_limits<std::uint16_t>::max());
        REQUIRE(j == std::numeric_limits<std::int16_t>::min());
    }

    SECTION("std::uint32_t, std::int32_t")
    {
        stream << std::numeric_limits<std::uint32_t>::max() << std::numeric_limits<std::int32_t>::min();
        REQUIRE(stream.buffer().size() == static_cast<acore::size_type>(sizeof(std::uint32_t) + sizeof(std::uint32_t)));
        stream.reset();
        std::uint32_t i = 0;
        std::int32_t j = 0;
        stream >> i >> j;
        REQUIRE(i == std::numeric_limits<std::uint32_t>::max());
        REQUIRE(j == std::numeric_limits<std::int32_t>::min());
    }

    SECTION("std::uint64_t, std::int64_t")
    {
        stream << std::numeric_limits<std::uint64_t>::max() << std::numeric_limits<std::int64_t>::min();
        REQUIRE(stream.buffer().size() == static_cast<acore::size_type>(sizeof(std::uint64_t) + sizeof(std::uint64_t)));
        stream.reset();
        std::uint64_t i = 0;
        std::int64_t j = 0;
        stream >> i >> j;
        REQUIRE(i == std::numeric_limits<std::uint64_t>::max());
        REQUIRE(j == std::numeric_limits<std::int64_t>::min());
    }

    SECTION("const char*, std::string")
    {
        const std::string str1{"Hello World!"};
        const std::string str2;
        const std::string str3{"Hello, World!"};

        stream << str1.data() << str2.data() << str3;
        REQUIRE(stream.buffer().size() == static_cast<acore::size_type>(sizeof(acore::size_type) + str1.size() + sizeof(acore::size_type) + str2.size() + sizeof(acore::size_type) + str3.size()));

        std::string str5;
        std::string str6;
        std::string str7;

        stream.reset();
        stream >> str5 >> str6 >> str7;
        REQUIRE(str5 == str1);
        REQUIRE(str6 == str2);
        REQUIRE(str7 == str3);
    }

    SECTION("float")
    {
        const float i = -4.4F;
        const float j = 1.1023F;
        stream << i << j;
        REQUIRE(stream.buffer().size() == static_cast<acore::size_type>(sizeof(float) + sizeof(float)));
        stream.reset();
        float k = 0.0F;
        float l = 0.0F;
        stream >> k >> l;
        REQUIRE(std::fabs(k - i) < std::numeric_limits<double>::epsilon());
        REQUIRE(std::fabs(l - j) < std::numeric_limits<double>::epsilon());
    }

    SECTION("double")
    {
        const double i = -10.102;
        const double j = 10.00000410007;
        stream << i << j;
        REQUIRE(stream.buffer().size() == static_cast<acore::size_type>(sizeof(double) + sizeof(double)));
        stream.reset();
        double k = 0.0;
        double l = 0.0;
        stream >> k >> l;
        REQUIRE(std::fabs(k - i) < std::numeric_limits<double>::epsilon());
        REQUIRE(std::fabs(l - j) < std::numeric_limits<double>::epsilon());
    }

    SECTION("std::vector<int>")
    {
        std::vector<int> v1;
        std::vector<int> v2{1, 2, 3, 4};
        stream << v1 << v2;
        REQUIRE(stream.buffer().size() == static_cast<acore::size_type>(sizeof(acore::size_type) + sizeof(acore::size_type) + (sizeof(int) * 4)));
        stream.reset();
        std::vector<int> v3;
        std::vector<int> v4;
        stream >> v3 >> v4;
        REQUIRE(v3 == v1);
        REQUIRE(v4 == v2);
    }

    SECTION("std::vector<std::string>")
    {
        const std::string str1{"Hello"};
        const std::string str2{"World"};
        const std::string str3{"!"};
        std::vector<std::string> v1{str1, str2, str3};
        std::vector<std::string> v2;
        stream << v1 << v2;
        REQUIRE(stream.buffer().size() == static_cast<acore::size_type>(sizeof(acore::size_type) + sizeof(acore::size_type) + str1.size() + sizeof(acore::size_type) + str2.size() + sizeof(acore::size_type) + str3.size() + sizeof(acore::size_type)));
        stream.reset();
        std::vector<std::string> v3;
        std::vector<std::string> v4;
        stream >> v3 >> v4;
        REQUIRE(v3 == v1);
        REQUIRE(v4 == v2);
    }

    SECTION("std::initializer_list<int>")
    {
        stream << std::initializer_list<int>{1, 2, 3, 4};
        REQUIRE(stream.buffer().size() == static_cast<acore::size_type>(sizeof(acore::size_type) + sizeof(int) * 4));
        stream.reset();
        std::vector<int> v;
        stream >> v;
        REQUIRE(v == std::vector<int>{1, 2, 3, 4});
    }

    SECTION("std::pair<int, std::string>")
    {
        const std::string str1{"Hi"};
        std::pair pair1{4, str1};
        std::pair<int, std::string> pair2;
        stream << pair1 << pair2;
        REQUIRE(stream.buffer().size() == static_cast<acore::size_type>(sizeof(int) + sizeof(acore::size_type) + str1.size() + sizeof(int) + sizeof(acore::size_type)));
    }
}

TEST_CASE("operator<<(DataStream &&stream, const T &value) -> DataStream &&, operator>>(DataStream &&stream, T &value) -> DataStream &&")
{
    const std::vector<char> data = (acore::DataStream{} << std::vector<int>{1, 2, 3, 4, 5} << std::string{"Hello World"}).buffer().data();
    REQUIRE(data.size() == 47);
    std::string str;
    std::vector<int> vec;
    acore::DataStream{data} >> vec >> str;
    REQUIRE(str == std::string{"Hello World"});
    REQUIRE(vec == std::vector<int>{1, 2, 3, 4, 5});
}

TEST_CASE("acore::DataStream [examples]")
{
    SECTION("[usage]")
    {
        // clang-format off
        //! [[Usage]]
acore::DataStream stream;
stream << 4;
stream << 1.0F;
stream << "Serializing C-string";
stream << std::vector<std::string>{"Hello", "World"};

int i = 0;
float f = 0.0F;
std::string str;
std::vector<std::string> vec;

//Deserializing the stored values
stream.reset();
stream >> i >> f >> str >> vec;
        //! [[Usage]]
        // clang-format on
        REQUIRE(i == 4);
        REQUIRE(f == 1.0F);
        REQUIRE(str == "Serializing C-string");
        REQUIRE(vec == std::vector<std::string>{"Hello", "World"});
    }
}
}
