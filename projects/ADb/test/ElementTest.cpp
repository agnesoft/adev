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

namespace elementtest
{
TEST_CASE("[adb::Element]")
{
#if defined(_MSC_VER) && !defined(_DEBUG)
    REQUIRE(std::is_standard_layout_v<adb::Element>);
#endif
    REQUIRE(std::is_default_constructible_v<adb::Element>);
    REQUIRE(std::is_copy_constructible_v<adb::Element>);
    REQUIRE(std::is_nothrow_move_constructible_v<adb::Element>);
    REQUIRE(std::is_copy_assignable_v<adb::Element>);
    REQUIRE(std::is_nothrow_move_assignable_v<adb::Element>);
    REQUIRE(std::is_nothrow_destructible_v<adb::Element>);
}

TEST_CASE("Element() [adb::Element]")
{
    REQUIRE_NOTHROW(adb::Element{});
}

TEST_CASE("Element(acore::size_type index) [adb::Element]")
{
    REQUIRE_NOTHROW(adb::Element{acore::size_type{1}});
}

TEST_CASE("Element(acore::size_type index, std::vector<KeyValue> data) [adb::Element]")
{
    REQUIRE_NOTHROW(adb::Element{acore::size_type{1}, {adb::KeyValue{adb::Value{"Hello"}, adb::Value{"World"}}}});
}

TEST_CASE("data() const noexcept -> const std::vector<KeyValue> & [adb::Element]")
{
    SECTION("[empty]")
    {
        const adb::Element e;
        REQUIRE(noexcept(e.data()));
        REQUIRE(e.data() == std::vector<adb::KeyValue>{{1, 2}});
    }

    SECTION("[id only]")
    {
        const adb::Element e{1};
    }

    SECTION("[data]")
    {
    }
}

TEST_CASE("index() const noexcept -> acore::size_type [adb::Element]")
{
}

TEST_CASE("operator==(const Element &left, const Element &right) -> bool [adb::Element]")
{
}

TEST_CASE("operator!=(const Element &left, const Element &right) -> bool [adb::Element]")
{
}
}
