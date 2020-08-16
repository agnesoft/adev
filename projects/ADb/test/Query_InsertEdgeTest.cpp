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

namespace query_insertedgetest
{
TEST_CASE("adb::insert_into().edge().from(acore::size_type id).to(acore::size_type id) && -> adb::Query::Id [adb::Query]")
{
    const auto query = adb::insert_into()
                           .edge()
                           .from(0)
                           .to(1);

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 1);
    REQUIRE(data.from == std::vector<acore::size_type>{0});
    REQUIRE(data.to == std::vector<acore::size_type>{1});
    REQUIRE(data.type == adb::InsertEdgeData::Type::Symmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().edge().from(adb::Placeholder::Id placeholder).to(acore::size_type id) && -> adb::Query::Id [adb::Query]")
{
    auto query = adb::insert_into()
                     .edge()
                     .from(adb::Placeholder::Id{":from"})
                     .to(1);
    query.bind(":from", 4);

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 1);
    REQUIRE(data.from == std::vector<acore::size_type>{4});
    REQUIRE(data.to == std::vector<acore::size_type>{1});
    REQUIRE(data.type == adb::InsertEdgeData::Type::Symmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().edge().from(acore::size_type id).to(adb::Placeholder::Id placeholder) && -> adb::Query::Id [adb::Query]")
{
    auto query = adb::insert_into()
                     .edge()
                     .from(0)
                     .to(adb::Placeholder::Id{":to"});
    query.bind(":to", 3);

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 1);
    REQUIRE(data.from == std::vector<acore::size_type>{0});
    REQUIRE(data.to == std::vector<acore::size_type>{3});
    REQUIRE(data.type == adb::InsertEdgeData::Type::Symmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().edge().from(adb::Placeholder::Id placeholder).to(adb::Placeholder::Id placeholder) && -> adb::Query::Id [adb::Query]")
{
    auto query = adb::insert_into()
                     .edge()
                     .from(adb::Placeholder::Id{":from"})
                     .to(adb::Placeholder::Id{":to"});
    query.bind(":from", 2);
    query.bind(":to", 0);

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 1);
    REQUIRE(data.from == std::vector<acore::size_type>{2});
    REQUIRE(data.to == std::vector<acore::size_type>{0});
    REQUIRE(data.type == adb::InsertEdgeData::Type::Symmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().edge().from(Query::Id subQuery).to(acore::size_type id) && -> adb::Query::Id [adb::Query]")
{
    const auto query = adb::insert_into()
                           .edge()
                           .from(adb::insert_into().node())
                           .to(2);

    REQUIRE(query.subQueries().size() == 1);

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 1);
    REQUIRE(data.from == std::vector<acore::size_type>{}); //NOLINT(readability-container-size-empty)
    REQUIRE(data.to == std::vector<acore::size_type>{2});
    REQUIRE(data.type == adb::InsertEdgeData::Type::Symmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().edge().from(acore::size_type id).to(adb::Query::Ids subQuery) && -> adb::Query::Id [adb::Query]")
{
    const auto query = adb::insert_into()
                           .edge()
                           .from(1)
                           .to(adb::insert_into().node());

    REQUIRE(query.subQueries().size() == 1);

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 1);
    REQUIRE(data.from == std::vector<acore::size_type>{1});
    REQUIRE(data.to == std::vector<acore::size_type>{}); //NOLINT(readability-container-size-empty)
    REQUIRE(data.type == adb::InsertEdgeData::Type::Symmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().edge().from(adb::Query::Ids subQuery).to(adb::Query::Ids subQuery) && -> adb::Query::Id [adb::Query]")
{
    const auto query = adb::insert_into()
                           .edge()
                           .from(adb::insert_into().node())
                           .to(adb::insert_into().node());

    REQUIRE(query.subQueries().size() == 2);

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 1);
    REQUIRE(data.from == std::vector<acore::size_type>{}); //NOLINT(readability-container-size-empty)
    REQUIRE(data.to == std::vector<acore::size_type>{}); //NOLINT(readability-container-size-empty)
    REQUIRE(data.type == adb::InsertEdgeData::Type::Symmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().edge(std::vector<adb::KeyValue> values).from(acore::size_type id).to(acore::size_type id) && -> adb::Query::Id [adb::Query]")
{
    const auto query = adb::insert_into()
                           .edge()
                           .from(0)
                           .to(1)
                           .values({{"Key", "Value"},
                                    {1, 4},
                                    {"Id", -4}});

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 1);
    REQUIRE(data.from == std::vector<acore::size_type>{0});
    REQUIRE(data.to == std::vector<acore::size_type>{1});
    REQUIRE(data.type == adb::InsertEdgeData::Type::Symmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{{{"Key", "Value"}, {1, 4}, {"Id", -4}}});
}

TEST_CASE("adb::insert_into().edge().from(acore::size_type id).to(acore::size_type id).values(adb::Placeholder::Values placeholder) && -> adb::Query::Id [adb::Query]")
{
    auto query = adb::insert_into()
                     .edge()
                     .from(0)
                     .to(1)
                     .values(adb::Placeholder::Values{":values"});
    query.bind(":values", {{"Key", "Value"}, {1, 4}, {"Id", -4}});

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 1);
    REQUIRE(data.from == std::vector<acore::size_type>{0});
    REQUIRE(data.to == std::vector<acore::size_type>{1});
    REQUIRE(data.type == adb::InsertEdgeData::Type::Symmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{{{"Key", "Value"}, {1, 4}, {"Id", -4}}});
}

TEST_CASE("adb::insert_into().edge().from(acore::size_type id).to(acore::size_type id).values(adb::Query::Ids subQuery) && -> adb::Query::Id [adb::Query]")
{
    const auto query = adb::insert_into()
                           .edge()
                           .from(0)
                           .to(1)
                           .values(adb::select().values());

    REQUIRE(query.subQueries().size() == 1);

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 1);
    REQUIRE(data.from == std::vector<acore::size_type>{0});
    REQUIRE(data.to == std::vector<acore::size_type>{1});
    REQUIRE(data.type == adb::InsertEdgeData::Type::Symmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}
}
