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

namespace insertedgequerytest
{
TEST_CASE("adb::insert().edge().from(acore::size_type id).to(acore::size_type id) [adb::Query]")
{
    const auto query = adb::insert()
                           .edge()
                           .from(0)
                           .to(1);

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.from == std::vector<acore::size_type>{0});
    REQUIRE(data.to == std::vector<acore::size_type>{1});
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert().edge().from(PlaceholderId placeholder).to(acore::size_type id) [adb::Query]")
{
    auto query = adb::insert()
                     .edge()
                     .from(adb::PlaceholderId{":from"})
                     .to(1);
    query.bind(":from", 4);

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.from == std::vector<acore::size_type>{4});
    REQUIRE(data.to == std::vector<acore::size_type>{1});
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert().edge().from(acore::size_type id).to(PlaceholderId placeholder) [adb::Query]")
{
    auto query = adb::insert()
                     .edge()
                     .from(0)
                     .to(adb::PlaceholderId{":to"});
    query.bind(":to", 3);

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.from == std::vector<acore::size_type>{0});
    REQUIRE(data.to == std::vector<acore::size_type>{3});
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert().edge().from(PlaceholderId placeholder).to(PlaceholderId placeholder) [adb::Query]")
{
    SECTION("[single bind]")
    {
        auto query = adb::insert()
                         .edge()
                         .from(adb::PlaceholderId{":from"})
                         .to(adb::PlaceholderId{":to"});
        query.bind(":from", 2);
        query.bind(":to", 0);

        const auto &data = std::get<adb::InsertEdgeData>(query.data());
        REQUIRE(data.from == std::vector<acore::size_type>{2});
        REQUIRE(data.to == std::vector<acore::size_type>{0});
        REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
    }

    SECTION("[multi bind]")
    {
        auto query = adb::insert()
                         .edge()
                         .from(adb::PlaceholderId{":from"})
                         .to(adb::PlaceholderId{":to"});
        query.bind(":from", 2);
        query.bind(":from", 3);
        query.bind(":to", 0);
        query.bind(":to", 1);

        const auto &data = std::get<adb::InsertEdgeData>(query.data());
        REQUIRE(data.from == std::vector<acore::size_type>{3});
        REQUIRE(data.to == std::vector<acore::size_type>{1});
        REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
    }
}

TEST_CASE("adb::insert().edge().from(IdsQuery subQuery).to(acore::size_type id) [adb::Query]")
{
    const auto query = adb::insert()
                           .edge()
                           .from(adb::insert().node())
                           .to(2);

    REQUIRE(query.subQueries().size() == 1);

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.from == std::vector<acore::size_type>{}); //NOLINT(readability-container-size-empty)
    REQUIRE(data.to == std::vector<acore::size_type>{2});
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert().edge().from(acore::size_type id).to(IdsQuery subQuery) [adb::Query]")
{
    const auto query = adb::insert()
                           .edge()
                           .from(1)
                           .to(adb::insert().node());

    REQUIRE(query.subQueries().size() == 1);

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.from == std::vector<acore::size_type>{1});
    REQUIRE(data.to == std::vector<acore::size_type>{}); //NOLINT(readability-container-size-empty)
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert().edge().from(IdsQuery subQuery).to(IdsQuery subQuery) [adb::Query]")
{
    const auto query = adb::insert()
                           .edge()
                           .from(adb::insert().node())
                           .to(adb::insert().node());

    REQUIRE(query.subQueries().size() == 2);

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.from == std::vector<acore::size_type>{}); //NOLINT(readability-container-size-empty)
    REQUIRE(data.to == std::vector<acore::size_type>{}); //NOLINT(readability-container-size-empty)
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert().edge(std::vector<adb::KeyValue> values).from(acore::size_type id).to(acore::size_type id) [adb::Query]")
{
    const auto query = adb::insert()
                           .edge({{"Key", "Value"},
                                  {1, 4},
                                  {"Id", -4}})
                           .from(0)
                           .to(1);

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.from == std::vector<acore::size_type>{0});
    REQUIRE(data.to == std::vector<acore::size_type>{1});
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{{{"Key", "Value"}, {1, 4}, {"Id", -4}}});
}

TEST_CASE("adb::insert().edge(PlaceholderValues placeholder).from(acore::size_type id).to(acore::size_type id) [adb::Query]")
{
    SECTION("[single bind]")
    {
        auto query = adb::insert()
                         .edge(adb::PlaceholderValues{":values"})
                         .from(0)
                         .to(1);
        query.bind(":values", {{"Key", "Value"}, {1, 4}, {"Id", -4}});

        const auto &data = std::get<adb::InsertEdgeData>(query.data());
        REQUIRE(data.from == std::vector<acore::size_type>{0});
        REQUIRE(data.to == std::vector<acore::size_type>{1});
        REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{{{"Key", "Value"}, {1, 4}, {"Id", -4}}});
    }

    SECTION("[multi bind]")
    {
        auto query = adb::insert()
                         .edge(adb::PlaceholderValues{":values"})
                         .from(0)
                         .to(1);
        query.bind(":values", {{"Key", "Value"}, {1, 4}, {"Id", -4}});
        query.bind(":values", {{1, 0}});

        const auto &data = std::get<adb::InsertEdgeData>(query.data());
        REQUIRE(data.from == std::vector<acore::size_type>{0});
        REQUIRE(data.to == std::vector<acore::size_type>{1});
        REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{{{1, 0}}});
    }
}

TEST_CASE("adb::insert().edge(IdsQuery subQuery).from(acore::size_type id).to(acore::size_type id) [adb::Query]")
{
    const auto query = adb::insert()
                           .edge(adb::select().values())
                           .from(0)
                           .to(1);

    REQUIRE(query.subQueries().size() == 1);

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.from == std::vector<acore::size_type>{0});
    REQUIRE(data.to == std::vector<acore::size_type>{1});
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}
}
