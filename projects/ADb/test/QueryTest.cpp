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

TEST_CASE("Query(const Query &other) [adb::Query]")
{
    const auto query = adb::insert().node(adb::insert().node());
    const auto other{query}; //NOLINT(performance-unnecessary-copy-initialization)

    const auto &subQueryData = std::get<adb::InsertNodeQuery>(query.data());
    REQUIRE(std::get<adb::InsertNodesCount>(subQueryData.query->data()).count == 1);
}

TEST_CASE("adb::insert().node() -> Query [adb::Query]")
{
    const auto query = adb::insert()
                           .node();

    REQUIRE(std::get<adb::InsertNodesCount>(query.data()).count == 1);
}

TEST_CASE("adb::insert().node(std::vector<adb::KeyValue> values) -> Query [adb::Query]")
{
    const auto query = adb::insert()
                           .node({{"Key1", "Value1"}, {"Key2", 4}});

    REQUIRE(std::get<adb::InsertNodeValues>(query.data()).values
            == std::vector<adb::KeyValue>{{"Key1", "Value1"}, {"Key2", 4}});
}

TEST_CASE("adb::insert().node(Query query) -> Query [adb::Query]")
{
    const auto query = adb::insert()
                           .node(adb::insert().node());

    const auto &subQueryData = std::get<adb::InsertNodeQuery>(query.data());
    REQUIRE(std::get<adb::InsertNodesCount>(subQueryData.query->data()).count == 1);
}

TEST_CASE("adb::insert().node(const char *placeholderName) -> Query [adb::Query]")
{
    auto query = adb::insert()
                     .node(":values");
    query.bind(":values", std::vector<adb::KeyValue>{{"Key1", "Value1"}, {"Key2", 4}});

    REQUIRE(std::get<adb::InsertNodeValues>(query.data()).values
            == std::vector<adb::KeyValue>{{"Key1", "Value1"}, {"Key2", 4}});
}

TEST_CASE("adb::insert().nodes(acore::size_type count) -> Query [adb::Query]")
{
    const auto query = adb::insert()
                           .nodes(3);

    REQUIRE(std::get<adb::InsertNodesCount>(query.data()).count == 3);
}

TEST_CASE("adb::insert().nodes(std::vector<std::vector<adb::KeyValue>> values) -> Query [adb::Query]")
{
    const auto query = adb::insert()
                           .nodes({{{"Key1", "Value1"}, {"Key2", 4}},
                                   {{"Key1", "Value2"}},
                                   {{1, 2}, {-3, -4}}});

    REQUIRE(std::get<adb::InsertNodesValues>(query.data()).values
            == std::vector<std::vector<adb::KeyValue>>{
                {{"Key1", "Value1"}, {"Key2", 4}},
                {{"Key1", "Value2"}},
                {{1, 2}, {-3, -4}}});
}

TEST_CASE("adb::insert().nodes(Query query) -> Query [adb::Query]")
{
    const auto query = adb::insert()
                           .nodes(adb::insert().nodes(2));
    const auto &subQuery = std::get<adb::InsertNodesQuery>(query.data());

    REQUIRE(std::get<adb::InsertNodesCount>(subQuery.query->data()).count == 2);
}
}
