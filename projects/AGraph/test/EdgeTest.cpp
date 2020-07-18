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

namespace Catch
{
template<>
struct StringMaker<agraph::Graph::Edge>
{
    [[nodiscard]] static auto convert(const agraph::Graph::Edge &edge) -> std::string
    {
        std::stringstream os;
        os << '{' << edge.index() << '}';
        return os.str();
    }
};

template<>
struct StringMaker<agraph::Graph::Node>
{
    [[nodiscard]] static auto convert(const agraph::Graph::Node &node) -> std::string
    {
        std::stringstream os;
        os << '{' << node.index() << ", from: ";

        for (const auto edge : node)
        {
            os << edge.index() << ", ";
        }

        os << "to: ";

        for (auto it = node.rbegin(); it != node.rend(); ++it)
        {
            os << (*it).index() << ", ";
        }

        os << '}';
        return os.str();
    }
};
}

namespace edgetest
{
TEST_CASE("[agraph::Edge]")
{
    REQUIRE(std::is_copy_constructible_v<agraph::Graph::Edge>);
    REQUIRE(std::is_copy_assignable_v<agraph::Graph::Edge>);
    REQUIRE(std::is_nothrow_move_constructible_v<agraph::Graph::Edge>);
    REQUIRE(std::is_nothrow_move_assignable_v<agraph::Graph::Edge>);
    REQUIRE(std::is_nothrow_destructible_v<agraph::Graph::Edge>);
}

TEST_CASE("Edge() [agraph::Edge]")
{
    REQUIRE(noexcept(agraph::Graph::Edge{}));
}

TEST_CASE("from() const -> Node [agraph::Edge]")
{
    agraph::Graph graph;
    const auto from = graph.insertNode();
    const auto to = graph.insertNode();
    const auto edge = graph.insertEdge(from, to);

    REQUIRE(edge.from() == from);
}

TEST_CASE("to() const -> Node [agraph::Edge]")
{
    agraph::Graph graph;
    const auto from = graph.insertNode();
    const auto to = graph.insertNode();
    const auto edge = graph.insertEdge(from, to);

    REQUIRE(edge.to() == to);
}
}
