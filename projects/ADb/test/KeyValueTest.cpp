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

TEST_CASE("KeyValue() [adb::KeyValue]")
{
    REQUIRE_NOTHROW(adb::KeyValue{});
}

TEST_CASE("KeyValue(const KeyT &key, const ValueT &value) [adb::KeyValue]")
{
    REQUIRE_NOTHROW(adb::KeyValue{"id", 3});
    REQUIRE_NOTHROW(adb::KeyValue{1, 2});
    REQUIRE_NOTHROW(adb::KeyValue{"key", "value"});
    REQUIRE_NOTHROW(adb::KeyValue{"a very long key", "even longer value with lots of characters"});
}

TEST_CASE("key() const noexcept -> const Value & [adb::KeyValue]")
{
    REQUIRE_NOTHROW(adb::KeyValue{"id", 3}.key() == adb::Value{"id"});
    REQUIRE_NOTHROW(adb::KeyValue{1, 2}.key() == adb::Value{1});
    REQUIRE_NOTHROW(adb::KeyValue{"key", "value"}.key() == adb::Value{"key"});
    REQUIRE_NOTHROW(adb::KeyValue{"a very long key", "even longer value with lots of characters"}.key() == adb::Value{"a very long key"});
}

TEST_CASE("value() const noexcept -> const Value & [adb::KeyValue]")
{
    REQUIRE_NOTHROW(adb::KeyValue{"id", 3}.value() == adb::Value{3});
    REQUIRE_NOTHROW(adb::KeyValue{1, 2}.value() == adb::Value{2});
    REQUIRE_NOTHROW(adb::KeyValue{"key", "value"}.value() == adb::Value{"value"});
    REQUIRE_NOTHROW(adb::KeyValue{"a very long key", "even longer value with lots of characters"}.value() == adb::Value{"even longer value with lots of characters"});
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

TEST_CASE("adb::KeyValue [examples]")
{
    SECTION("[verbose]")
    {
        // clang-format off
        //! [[Verbose]]
adb::KeyValue keyValue{adb::Value{"Key"}, adb::Value{"Value"}};
        //! [[Verbose]]
        // clang-format on
    }

    SECTION("[brief]")
    {
        // clang-format off
        //! [[Brief]]
adb::KeyValue keyValue{"Key", "Value"};
        //! [[Brief]]
        // clang-format on
    }

    SECTION("[vector]")
    {
        // clang-format off
        //! [[Vector]]
std::vector<adb::KeyValue> keyValues = {{"Key", "Value"},
                                        {1, 2},
                                        {"Id", 4}};
        //! [[Vector]]
        // clang-format on
    }
}
}
