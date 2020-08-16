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

namespace nodetest
{
struct ExpectedNodeData
{
    acore::size_type index = acore::INVALID_INDEX;
    std::vector<acore::size_type> from;
    std::vector<acore::size_type> to;
};
}

namespace agraph
{
[[nodiscard]] auto operator==(const agraph::Graph::Node &node, const nodetest::ExpectedNodeData &data) -> bool
{
    nodetest::ExpectedNodeData actual{node.index(), {}, {}};

    for (const auto edge : node)
    {
        actual.from.push_back(edge.index());
    }

    for (auto it = node.rbegin(); it != node.rend(); ++it)
    {
        actual.to.push_back((*it).index());
    }

    return actual.index == data.index && actual.from == data.from && actual.to == data.to;
}
}

namespace Catch
{
template<>
struct StringMaker<nodetest::ExpectedNodeData>
{
    [[nodiscard]] static auto convert(const nodetest::ExpectedNodeData &data) -> std::string
    {
        std::stringstream os;
        os << '{' << data.index << ", from: ";

        for (acore::size_type index : data.from)
        {
            os << index << ", ";
        }

        os << "to: ";

        for (acore::size_type index : data.to)
        {
            os << index << ",";
        }

        os << "}";
        return os.str();
    }
};

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

namespace nodetest
{
TEST_CASE("[agraph::Node]")
{
    REQUIRE(std::is_copy_constructible_v<agraph::Graph::Node>);
    REQUIRE(std::is_copy_assignable_v<agraph::Graph::Node>);
    REQUIRE(std::is_nothrow_move_constructible_v<agraph::Graph::Node>);
    REQUIRE(std::is_nothrow_move_assignable_v<agraph::Graph::Node>);
    REQUIRE(std::is_nothrow_destructible_v<agraph::Graph::Node>);
}

TEST_CASE("Node() [agraph::Node]")
{
    SECTION("[default]")
    {
        REQUIRE(noexcept(agraph::Graph::Node{}));
    }

    SECTION("[data]")
    {
        agraph::Graph graph;
        const auto node = graph.insertNode();
        const auto other = graph.insertNode();
        graph.insertEdge(node, other);
        graph.insertEdge(other, node);
        graph.insertEdge(node, other);
        graph.insertEdge(node, other);
        graph.insertEdge(other, node);

        REQUIRE(node == ExpectedNodeData{0, {-5, -4, -2}, {-6, -3}});
    }
}

TEST_CASE("begin() const -> const_iterator [agraph::Node]")
{
    SECTION("[empty]")
    {
        agraph::Graph graph;
        const auto node = graph.insertNode();
        REQUIRE(node.begin() == node.end());
    }

    SECTION("[outgoing]")
    {
        agraph::Graph graph;
        const auto from = graph.insertNode();
        const auto to = graph.insertNode();
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);
        const auto edge = graph.insertEdge(from, to);

        REQUIRE((*from.begin()) == edge);
    }

    SECTION("[incoming]")
    {
        agraph::Graph graph;
        const auto from = graph.insertNode();
        const auto to = graph.insertNode();
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);

        REQUIRE(to.begin() == to.end());
    }

    SECTION("[mixed]")
    {
        agraph::Graph graph;
        const auto from = graph.insertNode();
        const auto to = graph.insertNode();
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);
        const auto edge = graph.insertEdge(from, to);
        graph.insertEdge(to, from);
        graph.insertEdge(to, from);

        REQUIRE((*from.begin()) == edge);
    }
}

TEST_CASE("edgeCount() const -> acore::size_type [agraph::Node]")
{
    SECTION("[outgoing]")
    {
        agraph::Graph graph;
        const auto from = graph.insertNode();
        const auto to = graph.insertNode();
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);

        REQUIRE(from.edgeCount() == 3);
    }

    SECTION("[incoming]")
    {
        agraph::Graph graph;
        const auto from = graph.insertNode();
        const auto to = graph.insertNode();
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);

        REQUIRE(to.edgeCount() == 3);
    }

    SECTION("[mixed]")
    {
        agraph::Graph graph;
        const auto from = graph.insertNode();
        const auto to = graph.insertNode();
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);
        graph.insertEdge(to, from);
        graph.insertEdge(to, from);

        REQUIRE(from.edgeCount() == 5);
    }
}

TEST_CASE("end() const -> const_iterator [agraph::Node]")
{
    SECTION("[invalid]")
    {
        const agraph::Graph::Node node;
        auto end = node.end();

        REQUIRE(end.index() == acore::INVALID_INDEX);
        REQUIRE(end.iterable() == &node);
        REQUIRE_THROWS_AS(*end, acore::Exception);
        REQUIRE_THROWS_AS(++end, acore::Exception);
    }

    SECTION("[valid]")
    {
        agraph::Graph graph;
        const auto node = graph.insertNode();
        auto end = node.end();

        REQUIRE(end.index() == acore::INVALID_INDEX);
        REQUIRE(end.iterable() == &node);
        REQUIRE_THROWS_AS(*end, acore::Exception);
        REQUIRE_THROWS_AS(++end, acore::Exception);
    }
}

TEST_CASE("fromCount() const -> acore::size_type [agraph::Node]")
{
    SECTION("[outgoing]")
    {
        agraph::Graph graph;
        const auto from = graph.insertNode();
        const auto to = graph.insertNode();
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);

        REQUIRE(from.fromCount() == 3);
    }

    SECTION("[incoming]")
    {
        agraph::Graph graph;
        const auto from = graph.insertNode();
        const auto to = graph.insertNode();
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);

        REQUIRE(to.fromCount() == 0);
    }

    SECTION("[mixed]")
    {
        agraph::Graph graph;
        const auto from = graph.insertNode();
        const auto to = graph.insertNode();
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);
        graph.insertEdge(to, from);
        graph.insertEdge(to, from);

        REQUIRE(from.fromCount() == 3);
    }
}

TEST_CASE("rbegin() const -> const_iterator [agraph::Node]")
{
    SECTION("[empty]")
    {
        agraph::Graph graph;
        const auto node = graph.insertNode();
        REQUIRE(node.rbegin() == node.rend());
    }

    SECTION("[outgoing]")
    {
        agraph::Graph graph;
        const auto from = graph.insertNode();
        const auto to = graph.insertNode();
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);
        const auto edge = graph.insertEdge(from, to);

        REQUIRE((*to.rbegin()) == edge);
    }

    SECTION("[incoming]")
    {
        agraph::Graph graph;
        const auto from = graph.insertNode();
        const auto to = graph.insertNode();
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);

        REQUIRE(from.rbegin() == from.rend());
    }

    SECTION("[mixed]")
    {
        agraph::Graph graph;
        const auto from = graph.insertNode();
        const auto to = graph.insertNode();
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);
        graph.insertEdge(to, from);
        const auto edge = graph.insertEdge(to, from);

        REQUIRE((*from.rbegin()) == edge);
    }
}

TEST_CASE("rend() const -> const_iterator [agraph::Node]")
{
    SECTION("[invalid]")
    {
        const agraph::Graph::Node node;
        auto end = node.rend();

        REQUIRE(end.index() == acore::INVALID_INDEX);
        REQUIRE(end.iterable() == &node);
        REQUIRE_THROWS_AS(*end, acore::Exception);
        REQUIRE_THROWS_AS(++end, acore::Exception);
    }

    SECTION("[valid]")
    {
        agraph::Graph graph;
        const auto node = graph.insertNode();
        auto end = node.rend();

        REQUIRE(end.index() == acore::INVALID_INDEX);
        REQUIRE(end.iterable() == &node);
        REQUIRE_THROWS_AS(*end, acore::Exception);
        REQUIRE_THROWS_AS(++end, acore::Exception);
    }
}

TEST_CASE("toCount() const -> acore::size_type [agraph::Node]")
{
    SECTION("[outgoing]")
    {
        agraph::Graph graph;
        const auto from = graph.insertNode();
        const auto to = graph.insertNode();
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);

        REQUIRE(from.toCount() == 0);
    }

    SECTION("[incoming]")
    {
        agraph::Graph graph;
        const auto from = graph.insertNode();
        const auto to = graph.insertNode();
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);

        REQUIRE(to.toCount() == 3);
    }

    SECTION("[mixed]")
    {
        agraph::Graph graph;
        const auto from = graph.insertNode();
        const auto to = graph.insertNode();
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);
        graph.insertEdge(to, from);
        graph.insertEdge(to, from);

        REQUIRE(from.toCount() == 2);
    }
}
}
