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

#include <type_traits>
#include <utility>
#include <vector>

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

TEST_CASE("Element(acore::size_type index, std::vector<KeyValue> data) noexcept [adb::Element]")
{
    std::vector<adb::KeyValue> data{adb::KeyValue{adb::Value{"Hello"}, adb::Value{"World"}}};
    REQUIRE(noexcept(adb::Element{acore::size_type{1}, std::move(data)})); //NOLINT(hicpp-invalid-access-moved, bugprone-use-after-move)
    REQUIRE_NOTHROW(adb::Element{acore::size_type{1}, std::move(data)}); //NOLINT(hicpp-invalid-access-moved, bugprone-use-after-move)
}

TEST_CASE("data() const noexcept -> const std::vector<KeyValue> & [adb::Element]")
{
    SECTION("[empty]")
    {
        const adb::Element e;
        REQUIRE(noexcept(e.data()));
        REQUIRE(e.data() == std::vector<adb::KeyValue>{}); //NOLINT(readability-container-size-empty)
    }

    SECTION("[index only]")
    {
        const adb::Element e{1};
        REQUIRE(e.data() == std::vector<adb::KeyValue>{}); //NOLINT(readability-container-size-empty)
    }

    SECTION("[data]")
    {
        const adb::Element e{1, {{"Key", "Value"}, {"ID", 3}, {-2, -4}}};
        REQUIRE(e.data() == std::vector<adb::KeyValue>{{"Key", "Value"}, {"ID", 3}, {-2, -4}});
    }
}

TEST_CASE("index() const noexcept -> acore::size_type [adb::Element]")
{
    SECTION("[empty]")
    {
        const adb::Element e;
        REQUIRE(noexcept(e.index()));
        REQUIRE(e.index() == acore::INVALID_INDEX);
    }

    SECTION("[index only]")
    {
        const adb::Element e{1};
        REQUIRE(e.index() == 1);
    }

    SECTION("[data]")
    {
        const adb::Element e{1, {{"Key", "Value"}, {"ID", 3}, {-2, -4}}};
        REQUIRE(e.data() == std::vector<adb::KeyValue>{{"Key", "Value"}, {"ID", 3}, {-2, -4}});
    }
}

TEST_CASE("operator==(const Element &left, const Element &right) -> bool [adb::Element]")
{
    REQUIRE(adb::Element{} == adb::Element{});
    REQUIRE(adb::Element{1} == adb::Element{1});
    REQUIRE(adb::Element{1, {{"Key", "Value"}, {"ID", 3}, {-2, -4}}} == adb::Element{1, {{"Key", "Value"}, {"ID", 3}, {-2, -4}}});
    REQUIRE_FALSE(adb::Element{} == adb::Element{1});
    REQUIRE_FALSE(adb::Element{1} == adb::Element{});
    REQUIRE_FALSE(adb::Element{1, {{"Key", "Value"}, {"ID", 3}, {-2, -4}}} == adb::Element{1, {{"Key", "Value"}, {-2, -4}}});
}

TEST_CASE("operator!=(const Element &left, const Element &right) -> bool [adb::Element]")
{
    REQUIRE_FALSE(adb::Element{} != adb::Element{});
    REQUIRE_FALSE(adb::Element{1} != adb::Element{1});
    REQUIRE_FALSE(adb::Element{1, {{"Key", "Value"}, {"ID", 3}, {-2, -4}}} != adb::Element{1, {{"Key", "Value"}, {"ID", 3}, {-2, -4}}});
    REQUIRE(adb::Element{} != adb::Element{1});
    REQUIRE(adb::Element{1} != adb::Element{});
    REQUIRE(adb::Element{1, {{"Key", "Value"}, {"ID", 3}, {-2, -4}}} != adb::Element{1, {{"Key", "Value"}, {-2, -4}}});
}
}
