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
    SECTION("[copy with subquery]")
    {
        const auto query = adb::insert_into()
                               .nodes(adb::select()
                                          .count());
        const auto other{query}; //NOLINT(performance-unnecessary-copy-initialization)

        REQUIRE(other.subQueries().size() == 1);
        REQUIRE(std::get<adb::InsertNodeData>(query.data()).count == 0); //NOLINT(readability-container-size-empty)
        REQUIRE(std::get<adb::InsertNodeData>(query.data()).values.size() == 0); //NOLINT(readability-container-size-empty)
    }
}

TEST_CASE("bind(std::string_view name, PlaceholderValue value) -> void [adb::Query]")
{
    SECTION("[missing]")
    {
        auto query = adb::insert_into()
                         .node();
        REQUIRE_THROWS_AS(query.bind(":placeholder", 1), acore::Exception);
    }

    SECTION("[existing]")
    {
        auto query = adb::insert_into()
                         .node()
                         .values(adb::Placeholder::Values{":placeholder"});

        query.bind(":placeholder", std::vector<adb::KeyValue>{{"Key", "Value"}});
        REQUIRE(std::get<adb::InsertNodeData>(query.data()).values == std::vector<std::vector<adb::KeyValue>>{{{"Key", "Value"}}});
    }

    SECTION("[duplicate]")
    {
        REQUIRE_THROWS_AS(adb::insert_into()
                              .edge()
                              .from(adb::Placeholder::Id{":id"})
                              .to(adb::Placeholder::Id{":id"}),
                          acore::Exception);
    }

    SECTION("[multi bind]")
    {
        auto query = adb::insert_into()
                         .nodes(adb::Placeholder::Count{":count"});
        query.bind(":count", 4);
        query.bind(":count", 2);
        REQUIRE(std::get<adb::InsertNodeData>(query.data()).count == 2);
    }

    SECTION("[bind subquery]")
    {
        auto query = adb::insert_into()
                         .edges()
                         .from(adb::insert_into()
                                   .nodes()
                                   .values(adb::Placeholder::MultiValues{":node_values"}))
                         .to(adb::insert_into()
                                 .nodes()
                                 .values(adb::Placeholder::MultiValues{":node_values2"}))
                         .values(adb::Placeholder::MultiValues{":edge_values"});
        query.bind(":node_values", {{{"Key", "Value"}, {1, 2}}, {{"Key2", "Value2"}, {3, 4}}});
        query.bind(":node_values2", {{{"Key3", "Value3"}, {-1, -2}}, {{"Key4", "Value4"}, {-3, -4}}});
        query.bind(":edge_values", {{{"Key5", "Value6"}, {0, 2}}, {{"Key6", "Value6"}, {-3, 1}}});

        REQUIRE(query.subQueries().size() == 2);
        REQUIRE(std::get<adb::InsertNodeData>(query.subQueries()[0].query()->data()).values
                == std::vector<std::vector<adb::KeyValue>>{
                    {{"Key", "Value"}, {1, 2}},
                    {{"Key2", "Value2"}, {3, 4}}});
        REQUIRE(std::get<adb::InsertNodeData>(query.subQueries()[1].query()->data()).values
                == std::vector<std::vector<adb::KeyValue>>{
                    {{"Key3", "Value3"}, {-1, -2}},
                    {{"Key4", "Value4"}, {-3, -4}}});
        REQUIRE(std::get<adb::InsertEdgeData>(query.data()).values
                == std::vector<std::vector<adb::KeyValue>>{
                    {{"Key5", "Value6"}, {0, 2}},
                    {{"Key6", "Value6"}, {-3, 1}}});
    }

    SECTION("[bind placeholder duplicated in subquery]")
    {
        REQUIRE_THROWS_AS(adb::insert_into()
                              .edges()
                              .from(adb::insert_into()
                                        .nodes()
                                        .values(adb::Placeholder::MultiValues{":values"}))
                              .to(adb::insert_into()
                                      .nodes()
                                      .values(adb::Placeholder::MultiValues{":values2"}))
                              .values(adb::Placeholder::MultiValues{":values"}),
                          acore::Exception);
    }

    SECTION("[bind subquery with duplicate placeholders]")
    {
        REQUIRE_THROWS_AS(adb::insert_into()
                              .edges()
                              .from(adb::insert_into()
                                        .nodes()
                                        .values(adb::Placeholder::MultiValues{":node_values"}))
                              .to(adb::insert_into()
                                      .nodes()
                                      .values(adb::Placeholder::MultiValues{":node_values"}))
                              .values(adb::Placeholder::MultiValues{":edge_values"}),
                          acore::Exception);
    }
}

TEST_CASE("data() const noexcept -> const QueryData & [adb::Query]")
{
    const auto query = adb::insert_into().node();
    REQUIRE(noexcept(query.data()));
}

TEST_CASE("subQueries() const noexcept -> const std::vector<SubQuery> & [adb::Query]")
{
    const auto query = adb::insert_into().nodes(adb::select().count());
    REQUIRE(query.subQueries().size() == 1);
    REQUIRE_NOTHROW(std::get<adb::SelectData>(query.subQueries()[0].query()->data()));
    REQUIRE(query.subQueries()[0].bind() == adb::bindInsertNodesCount);
}
}
