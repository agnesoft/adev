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

namespace dataindexmapelementtest
{
TEST_CASE("[acore::DataIndexMapElement]")
{
    REQUIRE(std::is_standard_layout_v<acore::DataIndexMapElement>); //C++20: is_trivial
    REQUIRE(std::is_default_constructible_v<acore::DataIndexMapElement>); //C++20: is_trivially_default_constructible_v
    REQUIRE(std::is_trivially_copy_constructible_v<acore::DataIndexMapElement>);
    REQUIRE(std::is_trivially_move_constructible_v<acore::DataIndexMapElement>);
    REQUIRE(std::is_trivially_copy_assignable_v<acore::DataIndexMapElement>);
    REQUIRE(std::is_trivially_move_assignable_v<acore::DataIndexMapElement>);
    REQUIRE(std::is_trivially_destructible_v<acore::DataIndexMapElement>);
}

TEST_CASE("DataIndexMapElement [acore::DataIndexMapElement]")
{
    SECTION("[default]")
    {
        constexpr acore::DataIndexMapElement element;
        REQUIRE(element.key == acore::INVALID_INDEX);
        REQUIRE(element.value == acore::INVALID_INDEX);
    }

    SECTION("[data]")
    {
        constexpr acore::DataIndexMapElement element{10, 200}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(element.key == 10);
        REQUIRE(element.value == 200);
    }
}

TEST_CASE("operator<(DataIndexMapElement left, DataIndexMapElement right) -> bool [acore::DataIndexMapElement]")
{
    SECTION("[default]")
    {
        REQUIRE(noexcept(acore::DataIndexMapElement{} < acore::DataIndexMapElement{}));
        constexpr acore::DataIndexMapElement left;
        constexpr acore::DataIndexMapElement right;
        REQUIRE_FALSE(left < right);
    }

    SECTION("[less]")
    {
        constexpr acore::DataIndexMapElement left{1, 1};
        constexpr acore::DataIndexMapElement right{1, 2};
        REQUIRE(left < right);
    }

    SECTION("[equal]")
    {
        constexpr acore::DataIndexMapElement left{1, 1};
        constexpr acore::DataIndexMapElement right{1, 1};
        REQUIRE_FALSE(left < right);
    }

    SECTION("[greater]")
    {
        constexpr acore::DataIndexMapElement left{2, 1};
        constexpr acore::DataIndexMapElement right{1, 1};
        REQUIRE_FALSE(left < right);
    }
}

TEST_CASE("operator==(DataIndexMapElement left, DataIndexMapElement right) -> bool [acore::DataIndexMapElement]")
{
    SECTION("[default]")
    {
        REQUIRE(noexcept(acore::DataIndexMapElement{} == acore::DataIndexMapElement{}));
        constexpr const acore::DataIndexMapElement left;
        constexpr const acore::DataIndexMapElement right;
        REQUIRE(left == right);
    }

    SECTION("[equal]")
    {
        constexpr acore::DataIndexMapElement left{10, 100}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        constexpr acore::DataIndexMapElement right{10, 100}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(left == right);
    }

    SECTION("[different]")
    {
        constexpr acore::DataIndexMapElement left{12, -5}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        constexpr acore::DataIndexMapElement right{10, -7}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE_FALSE(left == right);
    }
}

TEST_CASE("operator!=(DataIndexMapElement left, DataIndexMapElement right) -> bool [acore::DataIndexMapElement]")
{
    SECTION("[default]")
    {
        REQUIRE(noexcept(acore::DataIndexMapElement{} == acore::DataIndexMapElement{}));
        constexpr acore::DataIndexMapElement left;
        constexpr acore::DataIndexMapElement right;
        REQUIRE_FALSE(left != right);
    }

    SECTION("[equal]")
    {
        constexpr acore::DataIndexMapElement left{5, 222}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        constexpr acore::DataIndexMapElement right{5, 222}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE_FALSE(left != right);
    }

    SECTION("[different]")
    {
        constexpr acore::DataIndexMapElement left{-67, 50}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        constexpr acore::DataIndexMapElement right{-67, 51}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(left != right);
    }
}

TEST_CASE("operator<<(DataStream &stream, const DataIndexMapElement &element) -> DataStream &, operator>>(DataStream &stream, DataIndexMapElement &hash) -> DataStream & [acore::DataIndexMapElement]")
{
    acore::DataStream stream;

    SECTION("[default]")
    {
        constexpr acore::DataIndexMapElement element;
        stream << element;
        REQUIRE(stream.buffer().data().size() == sizeof(acore::DataIndexMapElement));
        acore::DataIndexMapElement other;
        stream.reset();
        stream >> other;
        REQUIRE(element == other);
    }

    SECTION("[data]")
    {
        constexpr acore::DataIndexMapElement element{23, 26}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        stream << element;
        REQUIRE(stream.buffer().data().size() == sizeof(acore::DataIndexMapElement));
        acore::DataIndexMapElement other;
        stream.reset();
        stream >> other;
        REQUIRE(element == other);
    }
}
}
