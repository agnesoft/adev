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

namespace query_insertnodestest
{
TEST_CASE("adb::insert_into().nodes(acore::size_type count) && -> adb::Query::Ids [adb::Query]")
{
    const auto query = adb::insert_into()
                           .nodes(3);

    const auto &data = std::get<adb::InsertNodeData>(query.data());
    REQUIRE(data.count == 3);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().nodes(adb::Placeholder::Count placeholder) && -> adb::Query::Ids [adb::Query]")
{
    auto query = adb::insert_into()
                     .nodes(adb::Placeholder::Count{":count"});
    query.bind(":count", 4);

    const auto &data = std::get<adb::InsertNodeData>(query.data());
    REQUIRE(data.count == 4);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().nodes(adb::Query::Count subQuery) && -> adb::Query::Ids [adb::Query]")
{
    const auto query = adb::insert_into()
                           .nodes(adb::select().count());

    REQUIRE(query.subQueries().size() == 1);

    const auto &data = std::get<adb::InsertNodeData>(query.data());
    REQUIRE(data.count == 0);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().nodes(acore::size_type count).values(std::vector<adb::KeyValue>> values) && -> adb::Query::Ids [adb::Query]")
{
    const auto query = adb::insert_into()
                           .nodes(4)
                           .values({{"Key", "Value"}, {"Id", 1}});

    const auto &data = std::get<adb::InsertNodeData>(query.data());
    REQUIRE(data.count == 4);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{{{"Key", "Value"}, {"Id", 1}}});
}

TEST_CASE("adb::insert_into().nodes(acore::size_type count).values(adb::Placeholder::Values placeholder) && -> adb::Query::Ids [adb::Query]")
{
    auto query = adb::insert_into()
                     .nodes(2)
                     .values(adb::Placeholder::Values{":values"});

    query.bind(":values", {{"Key", "Value"}, {"Id", 3}});

    const auto &data = std::get<adb::InsertNodeData>(query.data());
    REQUIRE(data.count == 2);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{{{"Key", "Value"}, {"Id", 3}}});
}

TEST_CASE("adb::insert_into().nodes(acore::size_type count).values(adb::Query::Values subQuery) && -> adb::Query::Ids [adb::Query]")
{
    const auto query = adb::insert_into()
                           .nodes(4)
                           .values(adb::select().values());

    REQUIRE(query.subQueries().size() == 1);

    const auto &data = std::get<adb::InsertNodeData>(query.data());
    REQUIRE(data.count == 4);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().nodes(acore::size_type count).values(std::vector<std::vector<adb::KeyValue>>> values) && -> adb::Query::Ids [adb::Query]")
{
    const auto query = adb::insert_into()
                           .nodes(4)
                           .values({{{"Key", "Value"}, {"Id", 1}},
                                    {{1, 2}, {3, 4}}});

    const auto &data = std::get<adb::InsertNodeData>(query.data());
    REQUIRE(data.count == 4);
    REQUIRE(data.values
            == std::vector<std::vector<adb::KeyValue>>{
                {{"Key", "Value"}, {"Id", 1}},
                {{1, 2}, {3, 4}}});
}

TEST_CASE("adb::insert_into().nodes(acore::size_type count).values(adb::Placeholder::MultiValues placeholder) && -> adb::Query::Ids [adb::Query]")
{
    auto query = adb::insert_into()
                     .nodes(2)
                     .values(adb::Placeholder::MultiValues{":values"});

    query.bind(":values", {{{"Key", "Value"}, {"Id", 3}}, {{"Key", "Value"}}});

    const auto &data = std::get<adb::InsertNodeData>(query.data());
    REQUIRE(data.count == 2);
    REQUIRE(data.values
            == std::vector<std::vector<adb::KeyValue>>{
                {{"Key", "Value"}, {"Id", 3}},
                {{"Key", "Value"}}});
}

TEST_CASE("adb::insert_into().nodes(acore::size_type count).values(adb::Query::MultiValues subQuery) && -> adb::Query::Ids [adb::Query]")
{
    const auto query = adb::insert_into()
                           .nodes(4)
                           .values(adb::select().multiValues());

    REQUIRE(query.subQueries().size() == 1);

    const auto &data = std::get<adb::InsertNodeData>(query.data());
    REQUIRE(data.count == 4);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().nodes().values(std::vector<std::vector<adb::KeyValue>> values) && -> adb::Query::Ids [adb::Query]")
{
    const auto query = adb::insert_into()
                           .nodes()
                           .values({{{"Key1", "Value1"}, {"Key2", 4}},
                                    {{"Key1", "Value2"}},
                                    {{1, 2}, {-3, -4}}});

    const auto &data = std::get<adb::InsertNodeData>(query.data());
    REQUIRE(data.count == 3);
    REQUIRE(data.values
            == std::vector<std::vector<adb::KeyValue>>{
                {{"Key1", "Value1"}, {"Key2", 4}},
                {{"Key1", "Value2"}},
                {{1, 2}, {-3, -4}}});
}

TEST_CASE("adb::insert_into().nodes().values(adb::Placeholder::MultiValues values) && -> adb::Query::Ids [adb::Query]")
{
    auto query = adb::insert_into()
                     .nodes()
                     .values(adb::Placeholder::MultiValues{":multiValues"});
    query.bind(":multiValues", {{{"Key1", "Value1"}, {"Key2", 4}}, {{"Key1", "Value2"}}, {{1, 2}, {-3, -4}}});

    const auto &data = std::get<adb::InsertNodeData>(query.data());
    REQUIRE(data.count == 3);
    REQUIRE(data.values
            == std::vector<std::vector<adb::KeyValue>>{
                {{"Key1", "Value1"}, {"Key2", 4}},
                {{"Key1", "Value2"}},
                {{1, 2}, {-3, -4}}});
}

TEST_CASE("adb::insert_into().nodes().values(adb::Query::MultiValues query) && -> adb::Query::Ids [adb::Query]")
{
    const auto query = adb::insert_into()
                           .nodes()
                           .values(adb::select().multiValues());

    REQUIRE(query.subQueries().size() == 1);

    const auto &data = std::get<adb::InsertNodeData>(query.data());
    REQUIRE(data.count == 0);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().nodes().values(std::vector<adb::KeyValue>> values) && -> adb::Query::Ids [adb::Query]")
{
    const auto query = adb::insert_into()
                           .nodes()
                           .values({{"Key", "Value"}, {"Id", 1}});

    const auto &data = std::get<adb::InsertNodeData>(query.data());
    REQUIRE(data.count == 0);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{{{"Key", "Value"}, {"Id", 1}}});
}

TEST_CASE("adb::insert_into().nodes().values(adb::Placeholder::Values placeholder) && -> adb::Query::Ids [adb::Query]")
{
    auto query = adb::insert_into()
                     .nodes()
                     .values(adb::Placeholder::Values{":values"});

    query.bind(":values", {{"Key", "Value"}, {"Id", 3}});

    const auto &data = std::get<adb::InsertNodeData>(query.data());
    REQUIRE(data.count == 0);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{{{"Key", "Value"}, {"Id", 3}}});
}

TEST_CASE("adb::insert_into().nodes().values(adb::Query::Values subQuery) && -> adb::Query::Ids [adb::Query]")
{
    const auto query = adb::insert_into()
                           .nodes()
                           .values(adb::select().values());

    REQUIRE(query.subQueries().size() == 1);

    const auto &data = std::get<adb::InsertNodeData>(query.data());
    REQUIRE(data.count == 0);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}
}
