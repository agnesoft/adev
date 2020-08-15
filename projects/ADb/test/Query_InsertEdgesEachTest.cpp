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
TEST_CASE("adb::insert_into().edges().fromEach(std::vector<acore::size_type> id).toEach(std::vector<acore::size_type> ids) && -> adb::Query::Ids [adb::Query]")
{
    const auto query = adb::insert_into()
                           .edges()
                           .fromEach({1, 2})
                           .toEach({3, 2, 1});

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 0);
    REQUIRE(data.from == std::vector<acore::size_type>{1, 2});
    REQUIRE(data.to == std::vector<acore::size_type>{3, 2, 1});
    REQUIRE(data.type == adb::InsertEdgeData::Type::Assymmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().edges().fromEach(adb::Placeholder::Ids placeholder).toEach(std::vector<acore::size_type> ids) && -> adb::Query::Ids [adb::Query]")
{
    auto query = adb::insert_into()
                     .edges()
                     .fromEach(adb::Placeholder::Ids{":ids"})
                     .toEach({3});
    query.bind(":ids", {1, 2, 3, 4});

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 0);
    REQUIRE(data.from == std::vector<acore::size_type>{1, 2, 3, 4});
    REQUIRE(data.to == std::vector<acore::size_type>{3});
    REQUIRE(data.type == adb::InsertEdgeData::Type::Assymmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().edges().fromEach(adb::Query::Ids subQuery).toEach(acore::size_type) && -> adb::Query::Ids [adb::Query]")
{
    const auto query = adb::insert_into()
                           .edges()
                           .fromEach(adb::insert_into().nodes(3))
                           .toEach({3, 2, 1});

    REQUIRE(query.subQueries().size() == 1);

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 0);
    REQUIRE(data.from == std::vector<acore::size_type>{}); //NOLINT(readability-container-size-empty)
    REQUIRE(data.to == std::vector<acore::size_type>{3, 2, 1});
    REQUIRE(data.type == adb::InsertEdgeData::Type::Assymmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().edges().fromEach(std::vector<acore::size_type> ids).toEach(adb::Placeholder::Ids placeholder) && -> adb::Query::Ids [adb::Query]")
{
    auto query = adb::insert_into()
                     .edges()
                     .fromEach({3, 2})
                     .toEach(adb::Placeholder::Ids{":ids"});
    query.bind(":ids", {1, 2, 3});

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 0);
    REQUIRE(data.from == std::vector<acore::size_type>{3, 2});
    REQUIRE(data.to == std::vector<acore::size_type>{1, 2, 3});
    REQUIRE(data.type == adb::InsertEdgeData::Type::Assymmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().edges().fromEach(std::vector<acore::size_type> ids).toEach(adb::Query::Ids subQuery) && -> adb::Query::Ids [adb::Query]")
{
    const auto query = adb::insert_into()
                           .edges()
                           .fromEach({3, 2})
                           .toEach(adb::insert_into().nodes(2));

    REQUIRE(query.subQueries().size() == 1);

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 0);
    REQUIRE(data.from == std::vector<acore::size_type>{3, 2});
    REQUIRE(data.to == std::vector<acore::size_type>{}); //NOLINT(readability-container-size-empty)
    REQUIRE(data.type == adb::InsertEdgeData::Type::Assymmetric);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().edges().fromEach(std::vector<acore::size_type> id).toEach(std::vector<acore::size_type> ids).values(std::vector<std::vector<adb::KeyValue>> values) && -> adb::Query::Ids [adb::Query]")
{
    const auto query = adb::insert_into()
                           .edges()
                           .fromEach({1, 2})
                           .toEach({3, 2, 1})
                           .values({{{"Key", "Value"}, {1, 2}},
                                    {{-1, 0}, {"ID", 4}, {"Message", "Some text"}}});

    const auto &data = std::get<adb::InsertEdgeData>(query.data());
    REQUIRE(data.count == 0);
    REQUIRE(data.from == std::vector<acore::size_type>{1, 2});
    REQUIRE(data.to == std::vector<acore::size_type>{3, 2, 1});
    REQUIRE(data.type == adb::InsertEdgeData::Type::Assymmetric);
    REQUIRE(data.values
            == std::vector<std::vector<adb::KeyValue>>{
                {{"Key", "Value"}, {1, 2}},
                {{-1, 0}, {"ID", 4}, {"Message", "Some text"}}});
}
}
