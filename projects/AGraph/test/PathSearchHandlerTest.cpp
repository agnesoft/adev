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

namespace pathsearchhandlertest
{
TEST_CASE("[agraph::PathSearchHandler]")
{
    REQUIRE(std::is_trivial_v<agraph::PathSearchHandler>);
}

TEST_CASE("operator()(acore::size_type index, acore::size_type distance) const -> acore::size_type [agraph::PathSearchHandler]")
{
    REQUIRE(agraph::PathSearchHandler{}(0, 0) == 1);
    REQUIRE(agraph::PathSearchHandler{}(0, 1) == 1);
    REQUIRE(agraph::PathSearchHandler{}(1, 2) == 1);
}
}
