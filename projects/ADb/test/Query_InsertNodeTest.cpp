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

namespace query_insertnodetest
{
TEST_CASE("adb::insert_into().node() -> adb::Query::Id [adb::Query]")
{
    const auto query = adb::insert_into()
                           .node();

    const auto &data = std::get<adb::InsertNodeData>(query.data());
    REQUIRE(data.count == 1);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert_into().node().values(std::vector<adb::KeyValue> values) -> adb::Query::Id [adb::Query]")
{
    const auto query = adb::insert_into()
                           .node()
                           .values({{"Key1", "Value1"}, {"Key2", 4}});

    const auto &data = std::get<adb::InsertNodeData>(query.data());
    REQUIRE(data.count == 1);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{{{"Key1", "Value1"}, {"Key2", 4}}});
}

TEST_CASE("adb::insert_into().node().values(adb::Placeholder::Values placeholder) -> adb::Query::Id [adb::Query]")
{
    SECTION("[single bind]")
    {
        auto query = adb::insert_into()
                         .node()
                         .values(adb::Placeholder::Values{":values"});
        query.bind(":values", {{"Key1", "Value1"}, {"Key2", 4}});

        const auto &data = std::get<adb::InsertNodeData>(query.data());
        REQUIRE(data.count == 1);
        REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{{{"Key1", "Value1"}, {"Key2", 4}}});
    }

    SECTION("[multi bind]")
    {
        auto query = adb::insert_into()
                         .node()
                         .values(adb::Placeholder::Values{":values"});
        query.bind(":values", {{"Key1", "Value1"}, {"Key2", 4}});
        query.bind(":values", {{"K", "V"}, {"K2", 3}});

        const auto &data = std::get<adb::InsertNodeData>(query.data());
        REQUIRE(data.count == 1);
        REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{{{"K", "V"}, {"K2", 3}}});
    }
}

TEST_CASE("adb::insert_into().node().values(adb::Query::Values query) -> adb::Query::Id [adb::Query]")
{
    const auto query = adb::insert_into()
                           .node()
                           .values(adb::select().values());

    REQUIRE(query.subQueries().size() == 1);

    const auto &data = std::get<adb::InsertNodeData>(query.data());
    REQUIRE(data.count == 1);
    REQUIRE(data.values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}
}
