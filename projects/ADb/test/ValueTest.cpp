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
#include <vector>

namespace valutest
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

    SECTION("[short string]")
    {
        REQUIRE_NOTHROW(adb::Value{std::string{"Hello"}});
    }

    SECTION("[complex]")
    {
        REQUIRE_NOTHROW(adb::Value{std::string{"This is not exactly short string optimized string."}});
    }

    SECTION("[acore::Variant]")
    {
        REQUIRE_NOTHROW(adb::Value{acore::Variant{std::string{"Hello, World!"}}});
    }
}

TEST_CASE("operator T() const [adb::Value]")
{
    SECTION("[default]")
    {
        const auto val = static_cast<int>(adb::Value{});
        REQUIRE(val == 0);
    }

    SECTION("[trivial]")
    {
        const auto val = static_cast<int>(adb::Value{1});
        REQUIRE(val == 1);
    }

    SECTION("[short string]")
    {
        const auto val = static_cast<std::string>(adb::Value{"Hello"});
        REQUIRE(val == "Hello");
    }

    SECTION("[complex]")
    {
        const auto val = static_cast<std::vector<int>>(adb::Value{std::vector<int>{1, 2, 3, 4, 5}});
        REQUIRE(val == std::vector<int>{1, 2, 3, 4, 5});
    }
}
}
