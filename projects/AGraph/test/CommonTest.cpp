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

namespace commontest
{
TEST_CASE("edgeToIndex(acore::size_type index) noexcept -> acore::size_type [agraph]")
{
    REQUIRE(noexcept(agraph::edgeToIndex(-2)));
    REQUIRE(agraph::edgeToIndex(-2) == 0);
    REQUIRE(agraph::edgeToIndex(-3) == 1);
    REQUIRE(agraph::edgeToIndex(-4) == 2);
}

TEST_CASE("indexToEdge(acore::size_type index) noexcept -> acore::size_type [agraph]")
{
    REQUIRE(noexcept(agraph::indexToEdge(0)));
    REQUIRE(agraph::indexToEdge(0) == -2);
    REQUIRE(agraph::indexToEdge(1) == -3);
    REQUIRE(agraph::indexToEdge(2) == -4);
}

TEST_CASE("isEdge(acore::size_type index) noexcept -> bool [agraph]")
{
    REQUIRE(noexcept(agraph::isEdge(0)));
    REQUIRE_FALSE(agraph::isEdge(1));
    REQUIRE_FALSE(agraph::isEdge(0));
    REQUIRE_FALSE(agraph::isEdge(acore::INVALID_INDEX));
    REQUIRE(agraph::isEdge(-2));
    REQUIRE(agraph::isEdge(-100));
}

TEST_CASE("isNode(acore::size_type index) noexcept -> bool [agraph]")
{
    REQUIRE(noexcept(agraph::isNode(0)));
    REQUIRE(agraph::isNode(1));
    REQUIRE(agraph::isNode(0));
    REQUIRE_FALSE(agraph::isNode(acore::INVALID_INDEX));
    REQUIRE_FALSE(agraph::isNode(-2));
    REQUIRE_FALSE(agraph::isNode(-100));
}

TEST_CASE("operator==(NodeData left, NodeData right) noexcept -> bool [agraph]")
{
    REQUIRE(noexcept(agraph::NodeData{} == agraph::NodeData{}));
    REQUIRE(agraph::NodeData{} == agraph::NodeData{});
    REQUIRE(agraph::NodeData{1, 2, 3, 4} == agraph::NodeData{1, 2, 3, 4});
    REQUIRE_FALSE(agraph::NodeData{1, 2, 3, 4} == agraph::NodeData{4, 3, 2, 1});
}

TEST_CASE("operator!=(NodeData left, NodeData right) noexcept -> bool [agraph]")
{
    REQUIRE(noexcept(agraph::NodeData{} != agraph::NodeData{}));
    REQUIRE_FALSE(agraph::NodeData{} != agraph::NodeData{});
    REQUIRE_FALSE(agraph::NodeData{1, 2, 3, 4} != agraph::NodeData{1, 2, 3, 4});
    REQUIRE(agraph::NodeData{1, 2, 3, 4} != agraph::NodeData{4, 3, 2, 1});
}

TEST_CASE("operator==(EdgeData left, EdgeData right) noexcept -> bool [agraph]")
{
    REQUIRE(noexcept(agraph::NodeData{} == agraph::NodeData{}));
    REQUIRE(agraph::EdgeData{} == agraph::EdgeData{});
    REQUIRE(agraph::EdgeData{1, 2, 3, 4} == agraph::EdgeData{1, 2, 3, 4});
    REQUIRE_FALSE(agraph::EdgeData{1, 2, 3, 4} == agraph::EdgeData{4, 3, 2, 1});
}

TEST_CASE("operator!=(EdgeData left, EdgeData right) noexcept -> bool [agraph]")
{
    REQUIRE(noexcept(agraph::EdgeData{} != agraph::EdgeData{}));
    REQUIRE_FALSE(agraph::EdgeData{} != agraph::EdgeData{});
    REQUIRE_FALSE(agraph::EdgeData{1, 2, 3, 4} != agraph::EdgeData{1, 2, 3, 4});
    REQUIRE(agraph::EdgeData{1, 2, 3, 4} != agraph::EdgeData{4, 3, 2, 1});
}

TEST_CASE("operator<<(acore::DataStreamBase<Buffer> &stream, NodeData node) -> acore::DataStreamBase<Buffer> &, operator>>(acore::DataStreamBase<Buffer> &stream, NodeData &node) -> acore::DataStreamBase<Buffer> & [agraph]")
{
    const agraph::NodeData nodeData{1, 2, 3, 4};
    acore::DataStream stream;
    stream << nodeData;
    stream.reset();
    agraph::NodeData other;
    stream >> other;
    REQUIRE(nodeData == other);
}

TEST_CASE("operator<<(acore::DataStreamBase<Buffer> &stream, EdgeData node) -> acore::DataStreamBase<Buffer> &, operator>>(acore::DataStreamBase<Buffer> &stream, EdgeData &node) -> acore::DataStreamBase<Buffer> & [agraph]")
{
    const agraph::NodeData edgeData{1, 2, 3, 4};
    acore::DataStream stream;
    stream << edgeData;
    stream.reset();
    agraph::NodeData other;
    stream >> other;
    REQUIRE(edgeData == other);
}
}
