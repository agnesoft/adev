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

namespace insertnodesquerytest
{
TEST_CASE("adb::insert().nodes(acore::size_type count) -> Query [adb::Query]")
{
    const auto query = adb::insert()
                           .nodes(3);

    REQUIRE(std::get<adb::InsertNodeData>(query.data()).values.size() == 3);
}

TEST_CASE("adb::insert().nodes(PlaceholderCount placeholder) -> Query [adb::Query]")
{
    SECTION("[single bind]")
    {
        auto query = adb::insert()
                         .nodes(adb::PlaceholderCount{":count"});
        query.bind(":count", 4);

        REQUIRE(std::get<adb::InsertNodeData>(query.data()).values.size() == 4);
    }

    SECTION("[multi bind]")
    {
        auto query = adb::insert()
                         .nodes(adb::PlaceholderCount{":count"});
        query.bind(":count", 4);
        query.bind(":count", 2);

        REQUIRE(std::get<adb::InsertNodeData>(query.data()).values.size() == 2);
    }
}

TEST_CASE("adb::insert().nodes(std::vector<std::vector<adb::KeyValue>> values) -> Query [adb::Query]")
{
    const auto query = adb::insert()
                           .nodes({{{"Key1", "Value1"}, {"Key2", 4}},
                                   {{"Key1", "Value2"}},
                                   {{1, 2}, {-3, -4}}});

    REQUIRE(std::get<adb::InsertNodeData>(query.data()).values
            == std::vector<std::vector<adb::KeyValue>>{
                {{"Key1", "Value1"}, {"Key2", 4}},
                {{"Key1", "Value2"}},
                {{1, 2}, {-3, -4}}});
}

TEST_CASE("adb::insert().nodes(PlaceholderValues values) -> Query [adb::Query]")
{
    auto query = adb::insert()
                     .nodes(adb::PlaceholderValues{":values"});
    query.bind(":values", {{{"Key1", "Value1"}, {"Key2", 4}}, {{"Key1", "Value2"}}, {{1, 2}, {-3, -4}}});

    REQUIRE(std::get<adb::InsertNodeData>(query.data()).values
            == std::vector<std::vector<adb::KeyValue>>{
                {{"Key1", "Value1"}, {"Key2", 4}},
                {{"Key1", "Value2"}},
                {{1, 2}, {-3, -4}}});
}

TEST_CASE("adb::insert().nodes(SelectCountQuery query) -> Query [adb::Query]")
{
    const auto query = adb::insert()
                           .nodes(adb::select().count());

    REQUIRE(query.subQueries().size() == 1);
    REQUIRE(std::get<adb::InsertNodeData>(query.data()).values.size() == 0);
}

TEST_CASE("adb::insert().nodes(SelectValuesQuery query) -> Query [adb::Query]")
{
    const auto query = adb::insert()
                           .nodes(adb::select().multiValues());

    REQUIRE(query.subQueries().size() == 1);
    REQUIRE(std::get<adb::InsertNodeData>(query.data()).values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}
}
