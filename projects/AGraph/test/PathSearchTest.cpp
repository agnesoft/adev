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

#include <type_traits>
#include <unordered_map>

namespace pathsearchtest
{
class SearchHandler
{
public:
    auto setWeights(const std::unordered_map<acore::size_type, acore::size_type> &weights) -> void;
    [[nodiscard]] auto operator()(acore::size_type index, acore::size_type distance) const -> acore::size_type;

private:
    std::unordered_map<acore::size_type, acore::size_type> mWeights;
};

TEST_CASE("[agraph::PathSearch]")
{
    REQUIRE_FALSE(std::is_default_constructible_v<agraph::PathSearch<agraph::Graph>>);
    REQUIRE(std::is_copy_constructible_v<agraph::PathSearch<agraph::Graph>>);
    REQUIRE(std::is_nothrow_move_constructible_v<agraph::PathSearch<agraph::Graph>>);
    REQUIRE(std::is_copy_assignable_v<agraph::PathSearch<agraph::Graph>>);
    REQUIRE(std::is_move_assignable_v<agraph::PathSearch<agraph::Graph>>);
}

TEST_CASE("PathSearch(const GraphtType *graph [agraph::PathSearch]")
{
}

TEST_CASE("path(const typename GraphType::Node &from, const typename GraphType::Node &to) -> std::vector<acore::size_type> [agraph::PathSearch]")
{
}
}
