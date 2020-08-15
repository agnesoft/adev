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

namespace query_insertedgestest
{
TEST_CASE("adb::insert_into().edges().from(std::vector<acore::size_type> id).to(std::vector<acore::size_type> ids) && -> adb::Query::Ids [adb::Query]")
{
    const auto query = adb::insert_into()
                           .edges()
                           .from({1, 2, 3})
                           .to({3, 2, 1});

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 0);
    REQUIRE(data.from == std::vector<acore::size_type>{1, 2, 3});
    REQUIRE(data.to == std::vector<acore::size_type>{3, 2, 1});
    REQUIRE(data.type == adb::InsertEdgeData::Type::Symmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().edges().from(adb::Placeholder::Ids placeholder).to(std::vector<acore::size_type> ids) && -> adb::Query::Ids [adb::Query]")
{
    auto query = adb::insert_into()
                     .edges()
                     .from(adb::Placeholder::Ids{":ids"})
                     .to({3, 2, 1});
    query.bind(":ids", {1, 2, 3});

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 0);
    REQUIRE(data.from == std::vector<acore::size_type>{1, 2, 3});
    REQUIRE(data.to == std::vector<acore::size_type>{3, 2, 1});
    REQUIRE(data.type == adb::InsertEdgeData::Type::Symmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().edges().from(adb::Query::Ids subQuery).to(acore::size_type) && -> adb::Query::Ids [adb::Query]")
{
    const auto query = adb::insert_into()
                           .edges()
                           .from(adb::insert_into().nodes(3))
                           .to({3, 2, 1});

    REQUIRE(query.subQueries().size() == 1);

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 0);
    REQUIRE(data.from == std::vector<acore::size_type>{}); //NOLINT(readability-container-size-empty)
    REQUIRE(data.to == std::vector<acore::size_type>{3, 2, 1});
    REQUIRE(data.type == adb::InsertEdgeData::Type::Symmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().edges().from(std::vector<acore::size_type> ids).to(adb::Placeholder::Ids placeholder) && -> adb::Query::Ids [adb::Query]")
{
    auto query = adb::insert_into()
                     .edges()
                     .from({3, 2, 1})
                     .to(adb::Placeholder::Ids{":ids"});
    query.bind(":ids", {1, 2, 3});

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 0);
    REQUIRE(data.from == std::vector<acore::size_type>{3, 2, 1});
    REQUIRE(data.to == std::vector<acore::size_type>{1, 2, 3});
    REQUIRE(data.type == adb::InsertEdgeData::Type::Symmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().edges().from(std::vector<acore::size_type> ids).to(adb::Query::Ids subQuery) && -> adb::Query::Ids [adb::Query]")
{
    const auto query = adb::insert_into()
                           .edges()
                           .from({3, 2})
                           .to(adb::insert_into().nodes(2));

    REQUIRE(query.subQueries().size() == 1);

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 0);
    REQUIRE(data.from == std::vector<acore::size_type>{3, 2});
    REQUIRE(data.to == std::vector<acore::size_type>{}); //NOLINT(readability-container-size-empty)
    REQUIRE(data.type == adb::InsertEdgeData::Type::Symmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().edges(acore::size_type count).from(std::vector<acore::size_type> id).to(std::vector<acore::size_type> ids) && -> adb::Query::Ids [adb::Query]")
{
    const auto query = adb::insert_into()
                           .edges(2)
                           .from({1, 2, 3})
                           .to({3, 2, 1});

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 2);
    REQUIRE(data.from == std::vector<acore::size_type>{1, 2, 3});
    REQUIRE(data.to == std::vector<acore::size_type>{3, 2, 1});
    REQUIRE(data.type == adb::InsertEdgeData::Type::Symmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().edges(adb::Placeholder::Count placeholder).from(std::vector<acore::size_type> id).to(std::vector<acore::size_type> ids) && -> adb::Query::Ids [adb::Query]")
{
    auto query = adb::insert_into()
                     .edges(adb::Placeholder::Count{":count"})
                     .from({1, 2, 3})
                     .to({3, 2, 1});

    query.bind(":count", 1);

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 1);
    REQUIRE(data.from == std::vector<acore::size_type>{1, 2, 3});
    REQUIRE(data.to == std::vector<acore::size_type>{3, 2, 1});
    REQUIRE(data.type == adb::InsertEdgeData::Type::Symmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().edges(adb::Query::Count subQuery).from(std::vector<acore::size_type> id).to(std::vector<acore::size_type> ids) && -> adb::Query::Ids [adb::Query]")
{
    const auto query = adb::insert_into()
                           .edges(adb::select().count())
                           .from({1, 2, 3})
                           .to({3, 2, 1});

    REQUIRE(query.subQueries().size() == 1);

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 0);
    REQUIRE(data.from == std::vector<acore::size_type>{1, 2, 3});
    REQUIRE(data.to == std::vector<acore::size_type>{3, 2, 1});
    REQUIRE(data.type == adb::InsertEdgeData::Type::Symmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().edges().from(std::vector<acore::size_type> ids).to(std::vector<acore::size_type> ids).values(std::vector<adb::KeyValue> values) && -> adb::Query::Ids [adb::Query]")
{
    const auto query = adb::insert_into()
                           .edges()
                           .from({1, 2})
                           .to({3, 4})
                           .values({{"Key", "Value"}, {1, 2}});

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 0);
    REQUIRE(data.from == std::vector<acore::size_type>{1, 2});
    REQUIRE(data.to == std::vector<acore::size_type>{3, 4});
    REQUIRE(data.type == adb::InsertEdgeData::Type::Symmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{{{"Key", "Value"}, {1, 2}}});
}

TEST_CASE("adb::insert_into().edges().from(std::vector<acore::size_type> ids).to(std::vector<acore::size_type> ids).values(adb::Placeholder::Values placeholder) && -> adb::Query::Ids [adb::Query]")
{
    auto query = adb::insert_into()
                     .edges()
                     .from({1, 2})
                     .to({3, 4})
                     .values(adb::Placeholder::Values{":values"});
    query.bind(":values", {{"Key", "Value"}, {1, 2}});

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 0);
    REQUIRE(data.from == std::vector<acore::size_type>{1, 2});
    REQUIRE(data.to == std::vector<acore::size_type>{3, 4});
    REQUIRE(data.type == adb::InsertEdgeData::Type::Symmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{{{"Key", "Value"}, {1, 2}}});
}

TEST_CASE("adb::insert_into().edges().from(std::vector<acore::size_type> ids).to(std::vector<acore::size_type> ids).values(adb::Query::Values subQuery) && -> adb::Query::Ids [adb::Query]")
{
    const auto query = adb::insert_into()
                           .edges()
                           .from({1, 2})
                           .to({3, 4})
                           .values(adb::select().values());

    REQUIRE(query.subQueries().size() == 1);

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 0);
    REQUIRE(data.from == std::vector<acore::size_type>{1, 2});
    REQUIRE(data.to == std::vector<acore::size_type>{3, 4});
    REQUIRE(data.type == adb::InsertEdgeData::Type::Symmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().edges().from(std::vector<acore::size_type> ids).to(std::vector<acore::size_type> ids).values(std::vector<std::vector<adb::KeyValue>> values) && -> adb::Query::Ids [adb::Query]")
{
    const auto query = adb::insert_into()
                           .edges()
                           .from({1, 2})
                           .to({3, 4})
                           .values({{{"Key", "Value"}, {1, 2}},
                                    {{1, 2}, {3, 4}, {4, -1}}});

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 0);
    REQUIRE(data.from == std::vector<acore::size_type>{1, 2});
    REQUIRE(data.to == std::vector<acore::size_type>{3, 4});
    REQUIRE(data.type == adb::InsertEdgeData::Type::Symmetric);
    REQUIRE(data.values
            == std::vector<std::vector<adb::KeyValue>>{
                {{"Key", "Value"}, {1, 2}},
                {{1, 2}, {3, 4}, {4, -1}}});
}

TEST_CASE("adb::insert_into().edges().from(std::vector<acore::size_type> ids).to(std::vector<acore::size_type> ids).values(adb::Placeholder::MultiValues placeholder) && -> adb::Query::Ids [adb::Query]")
{
    auto query = adb::insert_into()
                     .edges()
                     .from({1, 2})
                     .to({3, 4})
                     .values(adb::Placeholder::MultiValues{":values"});
    query.bind(":values", {{{"Key", "Value"}, {1, 2}}, {{1, 2}, {3, 4}, {4, -1}}});

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 0);
    REQUIRE(data.from == std::vector<acore::size_type>{1, 2});
    REQUIRE(data.to == std::vector<acore::size_type>{3, 4});
    REQUIRE(data.type == adb::InsertEdgeData::Type::Symmetric);
    REQUIRE(data.values
            == std::vector<std::vector<adb::KeyValue>>{
                {{"Key", "Value"}, {1, 2}},
                {{1, 2}, {3, 4}, {4, -1}}});
}

TEST_CASE("adb::insert_into().edges().from(std::vector<acore::size_type> ids).to(std::vector<acore::size_type> ids).values(adb::Query::MultiValues subQuery) && -> adb::Query::Ids [adb::Query]")
{
    const auto query = adb::insert_into()
                           .edges()
                           .from({1, 2})
                           .to({3, 4})
                           .values(adb::select().multiValues());

    REQUIRE(query.subQueries().size() == 1);

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 0);
    REQUIRE(data.from == std::vector<acore::size_type>{1, 2});
    REQUIRE(data.to == std::vector<acore::size_type>{3, 4});
    REQUIRE(data.type == adb::InsertEdgeData::Type::Symmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}
}
