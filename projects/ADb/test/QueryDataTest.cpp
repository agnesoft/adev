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

namespace querydatatest
{
TEST_CASE("Query(const Query &other) [adb::Query]")
{
    SECTION("[copy with subquery]")
    {
        const auto query = adb::insert().nodes(adb::select().count());
        const auto other{query}; //NOLINT(performance-unnecessary-copy-initialization)
        auto subQueries = other.subQueries();

        REQUIRE(subQueries.size() == 1);
        REQUIRE(std::get<adb::InsertNodesCount>(query.data()).count == 0);
    }
}
}
