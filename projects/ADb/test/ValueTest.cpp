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

namespace valuetest
{
TEST_CASE("[adb::Value]")
{
    REQUIRE(std::is_default_constructible_v<adb::Value>);
    REQUIRE(std::is_copy_constructible_v<adb::Value>);
    REQUIRE(std::is_nothrow_move_constructible_v<adb::Value>);
    REQUIRE(std::is_copy_assignable_v<adb::Value>);
    REQUIRE(std::is_nothrow_move_assignable_v<adb::Value>);
    REQUIRE(std::is_nothrow_destructible_v<adb::Value>);
}

TEST_CASE("Value() [adb::Value]")
{
    REQUIRE_NOTHROW(adb::Value{});
}

TEST_CASE("Value(T &value) [adb::Value]")
{
    SECTION("[trivial]")
    {
        REQUIRE_NOTHROW(adb::Value{1});
    }

    SECTION("[acore::size_type]")
    {
        REQUIRE_NOTHROW(adb::Value{acore::size_type{1}});
    }

    SECTION("[short c-string]")
    {
        REQUIRE_NOTHROW(adb::Value{"Hello"});
    }

    SECTION("[string view]")
    {
        REQUIRE_NOTHROW(adb::Value{std::string_view{"Hello"}});
    }

    SECTION("[short string]")
    {
        REQUIRE_NOTHROW(adb::Value{std::string{"Hello"}});
    }

    SECTION("[long string]")
    {
        REQUIRE_NOTHROW(adb::Value{std::string{"This is not exactly short string optimized string."}});
    }

    SECTION("[acore::Variant]")
    {
        REQUIRE_NOTHROW(adb::Value{acore::Variant{std::string{"Hello, World!"}}});
    }
}

TEST_CASE("get() const -> T [adb::Value]")
{
    SECTION("[default]")
    {
        REQUIRE_THROWS_AS(adb::Value{}.get<int>(), acore::Exception);
        REQUIRE_THROWS_AS(adb::Value{}.get<std::vector<int>>(), acore::Exception);
        REQUIRE_THROWS_AS(adb::Value{}.get<std::string_view>(), acore::Exception);
    }

    SECTION("[trivial]")
    {
        REQUIRE(adb::Value{1}.get<int>() == 1);
    }

    SECTION("[short string]")
    {
        REQUIRE(adb::Value{"Hello"}.get<std::string>() == "Hello");
    }

    SECTION("[string_view, small string]")
    {
        REQUIRE(adb::Value{"Hello"}.get<std::string_view>() == "Hello");
    }

    SECTION("[string_view, long string]")
    {
        REQUIRE(adb::Value{"Hello, World!"}.get<std::string_view>() == "Hello, World!");
    }

    SECTION("[string_view, full string]")
    {
        const char *str = "12345678";
        REQUIRE(adb::Value{str}.get<std::string_view>() == str);
    }

    SECTION("[string_view, empty string]")
    {
        const char *str = "";
        REQUIRE(adb::Value{str}.get<std::string_view>() == str);
    }

    SECTION("[small string with null, null terminated]")
    {
        const std::array<char, 4> str{'1', '\0', '3', '\0'};
        REQUIRE(adb::Value{std::string_view{str.data(), str.size()}}.get<std::string>()
                == std::string{str.data(), str.size() - 1});
    }

    SECTION("[small string with null]")
    {
        const std::array<char, 4> str{'1', '\0', '3', '4'};
        REQUIRE(adb::Value{std::string_view{str.data(), str.size()}}.get<std::string>()
                == std::string{str.data(), str.size()});
    }

    SECTION("[complex]")
    {
        const auto val = adb::Value{std::vector<int>{1, 2, 3, 4}}.get<std::vector<int>>();
        REQUIRE(val == std::vector<int>{1, 2, 3, 4});
    }

    SECTION("[trivial -> complex]")
    {
        REQUIRE_THROWS_AS(adb::Value{std::string{"Hello, World!"}}.get<int>(), acore::Exception);
    }

    SECTION("[complex -> trivial]")
    {
        REQUIRE_THROWS_AS(adb::Value{10}.get<std::vector<int>>(), acore::Exception);
    }
}

TEST_CASE("operator==(const Value &left, const Value &right) [adb::Value]")
{
    SECTION("[default, default]")
    {
        REQUIRE(adb::Value{} == adb::Value{});
    }

    SECTION("[trivial, default]")
    {
        REQUIRE_FALSE(adb::Value{1} == adb::Value{});
    }

    SECTION("[complex, default]")
    {
        REQUIRE_FALSE(adb::Value{std::vector<int>{1, 2, 3, 4}} == adb::Value{});
    }

    SECTION("[trivial, same]")
    {
        REQUIRE(adb::Value{"Hello"} == adb::Value{"Hello"});
    }

    SECTION("[trivial, different]")
    {
        REQUIRE_FALSE(adb::Value{"Hello"} == adb::Value{"World"});
    }

    SECTION("[complex, same]")
    {
        REQUIRE(adb::Value{std::vector<int>{1, 2, 3, 4}} == adb::Value{std::vector<int>{1, 2, 3, 4}});
    }

    SECTION("[complex, different]")
    {
        REQUIRE_FALSE(adb::Value{std::string{"Hello, World!"}} == adb::Value{"Some different string"});
    }
}

TEST_CASE("operator!=(const Value &left, const Value &right) [adb::Value]")
{
    SECTION("[default, default]")
    {
        REQUIRE_FALSE(adb::Value{} != adb::Value{});
    }

    SECTION("[trivial, default]")
    {
        REQUIRE(adb::Value{1} != adb::Value{});
    }

    SECTION("[complex, default]")
    {
        REQUIRE(adb::Value{std::vector<int>{1, 2, 3, 4}} != adb::Value{});
    }

    SECTION("[trivial, same]")
    {
        REQUIRE_FALSE(adb::Value{"Hello"} != adb::Value{"Hello"});
    }

    SECTION("[trivial, different]")
    {
        REQUIRE(adb::Value{"Hello"} != adb::Value{"World"});
    }

    SECTION("[complex, same]")
    {
        REQUIRE_FALSE(adb::Value{std::vector<int>{1, 2, 3, 4}} != adb::Value{std::vector<int>{1, 2, 3, 4}});
    }

    SECTION("[complex, different]")
    {
        REQUIRE(adb::Value{std::string{"Hello, World!"}} != adb::Value{"Some different string"});
    }
}
}
