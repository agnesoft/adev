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

namespace insertnodequerytest
{
TEST_CASE("adb::insert().node() -> Query [adb::Query]")
{
    const auto query = adb::insert()
                           .node();

    REQUIRE(std::get<adb::InsertNodeData>(query.data()).values.size() == 1);
}

TEST_CASE("adb::insert().node(std::vector<adb::KeyValue> values) -> Query [adb::Query]")
{
    const auto query = adb::insert()
                           .node({{"Key1", "Value1"}, {"Key2", 4}});

    REQUIRE(std::get<adb::InsertNodeData>(query.data()).values
            == std::vector<std::vector<adb::KeyValue>>{{{"Key1", "Value1"}, {"Key2", 4}}});
}

TEST_CASE("adb::insert().node(SelectValuesQuery query) -> Query [adb::Query]")
{
    const auto query = adb::insert()
                           .node(adb::select().values());

    REQUIRE(query.subQueries().size() == 1);
    REQUIRE(std::get<adb::InsertNodeData>(query.data()).values == std::vector<std::vector<adb::KeyValue>>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("adb::insert().node(PlaceholderValues placeholder) -> Query [adb::Query]")
{
    SECTION("[single bind]")
    {
        auto query = adb::insert()
                         .node(adb::PlaceholderValues{":values"});
        query.bind(":values", {{"Key1", "Value1"}, {"Key2", 4}});

        REQUIRE(std::get<adb::InsertNodeData>(query.data()).values
                == std::vector<std::vector<adb::KeyValue>>{{{"Key1", "Value1"}, {"Key2", 4}}});
    }

    SECTION("[multi bind]")
    {
        auto query = adb::insert()
                         .node(adb::PlaceholderValues{":values"});
        query.bind(":values", {{"Key1", "Value1"}, {"Key2", 4}});
        query.bind(":values", {{"K", "V"}, {"K2", 3}});

        REQUIRE(std::get<adb::InsertNodeData>(query.data()).values
                == std::vector<std::vector<adb::KeyValue>>{{{"K", "V"}, {"K2", 3}}});
    }
}
}
