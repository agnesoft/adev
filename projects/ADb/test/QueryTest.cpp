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

#include <iostream>
#include <type_traits>
#include <utility>
#include <vector>

namespace querytest
{
TEST_CASE("[adb::Query]")
{
    REQUIRE_FALSE(std::is_default_constructible_v<adb::Query>);
    REQUIRE(std::is_copy_constructible_v<adb::Query>);
    REQUIRE(std::is_nothrow_move_constructible_v<adb::Query>);
    REQUIRE(std::is_copy_assignable_v<adb::Query>);
    REQUIRE(std::is_nothrow_move_assignable_v<adb::Query>);
    REQUIRE(std::is_nothrow_destructible_v<adb::Query>);
}

TEST_CASE("bind(std::string_view name, PlaceholderValue value) -> void [adb::Query]")
{
    SECTION("[missing]")
    {
        auto query = adb::insert().node();
        REQUIRE_THROWS_AS(query.bind(":placeholder", 1), acore::Exception);
    }

    SECTION("[existing]")
    {
        auto query = adb::insert().node(adb::PlaceholderValues{":placeholder"});

        REQUIRE_NOTHROW(query.bind(":placeholder", std::vector<adb::KeyValue>{}));
    }
}

TEST_CASE("data() const noexcept -> const QueryData & [adb::Query]")
{
    const auto query = adb::insert().node();
    REQUIRE(noexcept(query.data()));
}
}
