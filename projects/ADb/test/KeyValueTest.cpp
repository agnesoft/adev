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

namespace keyvaluetest
{
TEST_CASE("[adb::KeyValue]")
{
    REQUIRE(std::is_default_constructible_v<adb::KeyValue>);
    REQUIRE(std::is_copy_constructible_v<adb::KeyValue>);
    REQUIRE(std::is_nothrow_move_constructible_v<adb::KeyValue>);
    REQUIRE(std::is_copy_assignable_v<adb::KeyValue>);
    REQUIRE(std::is_nothrow_move_assignable_v<adb::KeyValue>);
    REQUIRE(std::is_nothrow_destructible_v<adb::KeyValue>);
}

TEST_CASE("operator==(const KeyValue &left, const KeyValue &right) -> bool [adb::KeyValue]")
{
    SECTION("[same]")
    {
        REQUIRE(adb::KeyValue{} == adb::KeyValue{});
        REQUIRE(adb::KeyValue{adb::Value{"Identifier"}, adb::Value{1}} == adb::KeyValue{adb::Value{"Identifier"}, adb::Value{1}});
        REQUIRE(adb::KeyValue{adb::Value{"Message"}, adb::Value{"Hello, World!"}} == adb::KeyValue{adb::Value{"Message"}, adb::Value{"Hello, World!"}});
    }

    SECTION("[different]")
    {
        REQUIRE_FALSE(adb::KeyValue{} == adb::KeyValue{adb::Value{"Hi"}, adb::Value{}});
        REQUIRE_FALSE(adb::KeyValue{adb::Value{"Identifier"}, adb::Value{1}} == adb::KeyValue{adb::Value{"Identifier"}, adb::Value{2}});
        REQUIRE_FALSE(adb::KeyValue{adb::Value{"Message"}, adb::Value{"Hello, World!"}} == adb::KeyValue{adb::Value{"Text"}, adb::Value{"Hello, World!"}});
    }
}

TEST_CASE("operator!=(const KeyValue &left, const KeyValue &right) -> bool [adb::KeyValue]")
{
    SECTION("[same]")
    {
        REQUIRE_FALSE(adb::KeyValue{} != adb::KeyValue{});
        REQUIRE_FALSE(adb::KeyValue{adb::Value{"Identifier"}, adb::Value{1}} != adb::KeyValue{adb::Value{"Identifier"}, adb::Value{1}});
        REQUIRE_FALSE(adb::KeyValue{adb::Value{"Message"}, adb::Value{"Hello, World!"}} != adb::KeyValue{adb::Value{"Message"}, adb::Value{"Hello, World!"}});
    }

    SECTION("[different]")
    {
        REQUIRE(adb::KeyValue{} != adb::KeyValue{adb::Value{"Hi"}, adb::Value{}});
        REQUIRE(adb::KeyValue{adb::Value{"Identifier"}, adb::Value{1}} != adb::KeyValue{adb::Value{"Identifier"}, adb::Value{2}});
        REQUIRE(adb::KeyValue{adb::Value{"Message"}, adb::Value{"Hello, World!"}} != adb::KeyValue{adb::Value{"Text"}, adb::Value{"Hello, World!"}});
    }
}
}
