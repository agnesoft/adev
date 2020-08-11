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

#include <utility>
#include <vector>

namespace insertquerytest
{
TEST_CASE("adb::insert().edge().from(acore::size_type id).to(acore::size_type id) [adb::Query]")
{
    const auto query = adb::insert()
                           .edge()
                           .from(0)
                           .to(1);

    const auto data = std::get<adb::InsertEdgesCount>(query.data());
    REQUIRE(data.from == 0);
    REQUIRE(data.to == 1);
    REQUIRE(data.count == 1);
}

TEST_CASE("adb::insert().edge().from(PlaceholderId placeholder).to(acore::size_type id) [adb::Query]")
{
    auto query = adb::insert()
                     .edge()
                     .from(adb::PlaceholderId{":from"})
                     .to(1);
    query.bind(":from", 4);

    const auto data = std::get<adb::InsertEdgesCount>(query.data());
    REQUIRE(data.from == 4);
    REQUIRE(data.to == 1);
    REQUIRE(data.count == 1);
}

TEST_CASE("adb::insert().edge().from(acore::size_type id).to(PlaceholderId placeholder) [adb::Query]")
{
    auto query = adb::insert()
                     .edge()
                     .from(0)
                     .to(adb::PlaceholderId{":to"});
    query.bind(":to", 3);

    const auto data = std::get<adb::InsertEdgesCount>(query.data());
    REQUIRE(data.from == 0);
    REQUIRE(data.to == 3);
    REQUIRE(data.count == 1);
}

TEST_CASE("adb::insert().edge().from(PlaceholderId placeholder).to(PlaceholderId placeholder) [adb::Query]")
{
    auto query = adb::insert()
                     .edge()
                     .from(adb::PlaceholderId{":from"})
                     .to(adb::PlaceholderId{":to"});
    query.bind(":from", 2);
    query.bind(":to", 0);

    const auto data = std::get<adb::InsertEdgesCount>(query.data());
    REQUIRE(data.from == 2);
    REQUIRE(data.to == 0);
    REQUIRE(data.count == 1);
}

TEST_CASE("adb::insert().edge().from(IdsQuery subQuery).to(acore::size_type id) [adb::Query]")
{
    const auto query = adb::insert()
                           .edge()
                           .from(adb::insert().node())
                           .to(2);

    REQUIRE(query.subQueries().size() == 1);

    const auto data = std::get<adb::InsertEdgesCount>(query.data());
    REQUIRE(data.from == acore::INVALID_INDEX);
    REQUIRE(data.to == 2);
    REQUIRE(data.count == 1);
}

TEST_CASE("adb::insert().edge().from(acore::size_type id).to(IdsQuery subQuery) [adb::Query]")
{
    const auto query = adb::insert()
                           .edge()
                           .from(1)
                           .to(adb::insert().node());

    REQUIRE(query.subQueries().size() == 1);

    const auto data = std::get<adb::InsertEdgesCount>(query.data());
    REQUIRE(data.from == 1);
    REQUIRE(data.to == acore::INVALID_INDEX);
    REQUIRE(data.count == 1);
}

TEST_CASE("adb::insert().edge().from(IdsQuery subQuery).to(IdsQuery subQuery) [adb::Query]")
{
    const auto query = adb::insert()
                           .edge()
                           .from(adb::insert().node())
                           .to(adb::insert().node());

    REQUIRE(query.subQueries().size() == 2);

    const auto data = std::get<adb::InsertEdgesCount>(query.data());
    REQUIRE(data.from == acore::INVALID_INDEX);
    REQUIRE(data.to == acore::INVALID_INDEX);
    REQUIRE(data.count == 1);
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

TEST_CASE("adb::insert().node(SelectValuesQuery query) -> Query [adb::Query]")
{
    const auto query = adb::insert()
                           .node(adb::select().values());

    REQUIRE(query.subQueries().size() == 1);
    REQUIRE(std::get<adb::InsertNodeValues>(query.data()).values == std::vector<adb::KeyValue>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert().node(PlaceholderValues placeholder) -> Query [adb::Query]")
{
    auto query = adb::insert()
                     .node(adb::PlaceholderValues{":values"});
    query.bind(":values", {{"Key1", "Value1"}, {"Key2", 4}});

    REQUIRE(std::get<adb::InsertNodeValues>(query.data()).values
            == std::vector<adb::KeyValue>{{"Key1", "Value1"}, {"Key2", 4}});
}

TEST_CASE("adb::insert().nodes(acore::size_type count) -> Query [adb::Query]")
{
    const auto query = adb::insert()
                           .nodes(3);

    REQUIRE(std::get<adb::InsertNodesCount>(query.data()).count == 3);
}

TEST_CASE("adb::insert().nodes(PlaceholderCount placeholder) -> Query [adb::Query]")
{
    auto query = adb::insert()
                     .nodes(adb::PlaceholderCount{":count"});
    query.bind(":count", 4);

    REQUIRE(std::get<adb::InsertNodesCount>(query.data()).count == 4);
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

TEST_CASE("adb::insert().nodes(PlaceholderValues values) -> Query [adb::Query]")
{
    auto query = adb::insert()
                     .nodes(adb::PlaceholderValues{":values"});
    query.bind(":values", {{{"Key1", "Value1"}, {"Key2", 4}}, {{"Key1", "Value2"}}, {{1, 2}, {-3, -4}}});

    REQUIRE(std::get<adb::InsertNodesValues>(query.data()).values
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
    REQUIRE(std::get<adb::InsertNodesCount>(query.data()).count == 0);
}

TEST_CASE("adb::insert().nodes(SelectValuesQuery query) -> Query [adb::Query]")
{
    const auto query = adb::insert()
                           .nodes(adb::select().values());

    REQUIRE(query.subQueries().size() == 1);
    REQUIRE(std::get<adb::InsertNodesValues>(query.data()).values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}
}
