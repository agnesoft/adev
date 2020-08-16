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

namespace agraph
{
[[nodiscard]] auto operator==(const Graph &graph, const std::vector<std::pair<acore::size_type, std::vector<acore::size_type>>> &elements) -> bool
{
    std::vector<std::pair<acore::size_type, std::vector<acore::size_type>>> graphElements;

    for (const auto node : graph)
    {
        auto &element = graphElements.emplace_back(std::pair<acore::size_type, std::vector<acore::size_type>>{node.index(), {}});
        element.second.reserve(node.fromCount());

        for (const auto edge : node)
        {
            element.second.push_back(edge.index());
        }
    }

    return graphElements == elements;
}
}

namespace Catch
{
template<>
struct StringMaker<std::vector<std::pair<acore::size_type, std::vector<acore::size_type>>>>
{
    [[nodiscard]] static auto convert(const std::vector<std::pair<acore::size_type, std::vector<acore::size_type>>> &elements) -> std::string
    {
        std::stringstream os;
        os << '{';
        for (const auto &element : elements)
        {
            os << '{' << element.first << ", {";

            for (const auto edge : element.second)
            {
                os << edge << ", ";
            }

            os << "}}, ";
        }
        os << '}';
        return os.str();
    }
};

template<>
struct StringMaker<agraph::Graph>
{
    [[nodiscard]] static auto convert(const agraph::Graph &graph) -> std::string
    {
        std::stringstream os;
        os << '{';
        for (const auto node : graph)
        {
            os << '{' << node.index() << ", {";

            for (const auto edge : node)
            {
                os << edge.index() << ", ";
            }

            os << "}}, ";
        }
        os << '}';
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

namespace graphtest
{
TEST_CASE("[agraph::Graph]")
{
#if defined(_MSC_VER) && !defined(_DEBUG)
    REQUIRE(std::is_standard_layout_v<agraph::Graph>);
#endif
    REQUIRE(std::is_default_constructible_v<agraph::Graph>);
    REQUIRE(std::is_copy_constructible_v<agraph::Graph>);
    REQUIRE(std::is_copy_assignable_v<agraph::Graph>);
    REQUIRE(std::is_nothrow_move_constructible_v<agraph::Graph>);
    REQUIRE(std::is_nothrow_move_assignable_v<agraph::Graph>);
    REQUIRE(std::is_nothrow_destructible_v<agraph::Graph>);
}

TEST_CASE("begin() noexcept -> const_iterator [agraph::Graph]")
{
    SECTION("[empty]")
    {
        const agraph::Graph graph;

        REQUIRE(noexcept(graph.begin()));
        REQUIRE(graph.begin() == graph.end());
    }

    SECTION("[data]")
    {
        agraph::Graph graph;
        graph.insertNode();
        graph.insertNode();
        graph.insertNode();

        const auto it = graph.begin();
        REQUIRE(it.index() == 0);
        REQUIRE(it.iterable() == &graph);
    }

    SECTION("[removed]")
    {
        agraph::Graph graph;
        const auto node = graph.insertNode();
        graph.insertNode();
        graph.insertNode();
        graph.removeNode(node);

        const auto it = graph.begin();
        REQUIRE(it.index() == 1);
        REQUIRE(it.iterable() == &graph);
    }
}

TEST_CASE("contains(const Edge &edge) const -> bool [agraph::Graph]")
{
    SECTION("[missing]")
    {
        const agraph::Graph graph;
        const auto edge = graph.edge(-2);

        REQUIRE_FALSE(graph.contains(edge));
    }

    SECTION("[data]")
    {
        agraph::Graph graph;
        auto node1 = graph.insertNode();
        auto node2 = graph.insertNode();
        auto edge = graph.insertEdge(node1, node2);
        graph.insertNode();

        REQUIRE(graph.contains(edge));
    }

    SECTION("[removed]")
    {
        agraph::Graph graph;
        auto node1 = graph.insertNode();
        auto node2 = graph.insertNode();
        auto edge = graph.insertEdge(node1, node2);
        graph.removeEdge(edge);

        REQUIRE_FALSE(graph.contains(edge));
    }
}

TEST_CASE("contains(const Node &node) const -> bool [agraph::Graph]")
{
    SECTION("[empty]")
    {
        const agraph::Graph graph;
        const auto node = graph.node(0);

        REQUIRE_FALSE(graph.contains(node));
    }

    SECTION("[data]")
    {
        agraph::Graph graph;
        const auto node = graph.insertNode();
        graph.insertNode();
        graph.insertNode();

        REQUIRE(graph.contains(node));
    }

    SECTION("[removed]")
    {
        agraph::Graph graph;
        const auto node = graph.insertNode();
        graph.insertNode();
        graph.insertNode();
        graph.removeNode(node);

        REQUIRE_FALSE(graph.contains(node));
    }
}

TEST_CASE("insertEdge(const Node &from, const Node &to) -> Edge [agraph::Graph]")
{
    SECTION("[data, no edges]")
    {
        agraph::Graph graph;
        const auto from = graph.insertNode();
        const auto to = graph.insertNode();
        const auto edge = graph.insertEdge(from, to);

        REQUIRE(edge.index() == -2);
        REQUIRE(edge.from() == from);
        REQUIRE(edge.to() == to);
        REQUIRE(graph == std::vector<std::pair<acore::size_type, std::vector<acore::size_type>>>{{0, {-2}}, {1, {}}});
    }

    SECTION("[data]")
    {
        agraph::Graph graph;
        const auto from = graph.insertNode();
        const auto to = graph.insertNode();
        graph.insertEdge(from, to);
        graph.insertEdge(from, to);
        graph.insertEdge(to, from);
        const auto edge = graph.insertEdge(from, to);

        REQUIRE(edge.index() == -5);
        REQUIRE(edge.from() == from);
        REQUIRE(edge.to() == to);
        REQUIRE(graph == std::vector<std::pair<acore::size_type, std::vector<acore::size_type>>>{{0, {-5, -3, -2}}, {1, {-4}}});
    }

    SECTION("[data, removed edges]")
    {
        agraph::Graph graph;
        const auto from = graph.insertNode();
        const auto to = graph.insertNode();
        graph.insertEdge(from, to);
        auto edge = graph.insertEdge(from, to);
        graph.insertEdge(to, from);
        graph.removeEdge(edge);
        edge = graph.insertEdge(from, to);

        REQUIRE(edge.index() == -3);
        REQUIRE(edge.from() == from);
        REQUIRE(edge.to() == to);
        REQUIRE(graph == std::vector<std::pair<acore::size_type, std::vector<acore::size_type>>>{{0, {-3, -2}}, {1, {-4}}});
    }

    SECTION("[invalid from]")
    {
        agraph::Graph graph;
        const auto from = graph.insertNode();
        const auto to = graph.insertNode();
        graph.removeNode(from);

        REQUIRE_THROWS_AS(graph.insertEdge(from, to), acore::Exception);
    }

    SECTION("[invalid to]")
    {
        agraph::Graph graph;
        const auto from = graph.insertNode();
        const auto to = graph.insertNode();
        graph.removeNode(to);

        REQUIRE_THROWS_AS(graph.insertEdge(from, to), acore::Exception);
    }
}

TEST_CASE("insertNode() -> Node [agraph::Graph]")
{
    SECTION("[empty]")
    {
        agraph::Graph graph;
        const auto node = graph.insertNode();

        REQUIRE(node.index() == 0);
        REQUIRE(node.graph() == &graph);
        REQUIRE(graph == std::vector<std::pair<acore::size_type, std::vector<acore::size_type>>>{{0, {}}});
    }

    SECTION("[data]")
    {
        agraph::Graph graph;
        graph.insertNode();
        graph.insertNode();
        const auto node = graph.insertNode();

        REQUIRE(node.index() == 2);
        REQUIRE(node.graph() == &graph);
        REQUIRE(graph == std::vector<std::pair<acore::size_type, std::vector<acore::size_type>>>{{0, {}}, {1, {}}, {2, {}}});
    }

    SECTION("[removed]")
    {
        agraph::Graph graph;
        graph.insertNode();
        auto node = graph.insertNode();
        graph.insertNode();
        graph.insertNode();
        graph.removeNode(node);
        node = graph.insertNode();

        REQUIRE(node.index() == 1);
        REQUIRE(node.graph() == &graph);
        REQUIRE(graph == std::vector<std::pair<acore::size_type, std::vector<acore::size_type>>>{{0, {}}, {1, {}}, {2, {}}, {3, {}}});
    }
}

TEST_CASE("edge(acore::size_type index) const -> Edge [agraph::Graph]")
{
    SECTION("[missing]")
    {
        const agraph::Graph graph;
        const auto edge = graph.edge(-2);

        REQUIRE(edge.index() == acore::INVALID_INDEX);
    }

    SECTION("[existing]")
    {
        agraph::Graph graph;
        const auto from = graph.insertNode();
        graph.insertNode();
        const auto to = graph.insertNode();
        graph.insertEdge(from, to);
        graph.insertEdge(to, from);
        const auto edge = graph.insertEdge(from, to);

        REQUIRE(graph.edge(-4) == edge);
    }

    SECTION("[removed]")
    {
        agraph::Graph graph;
        const auto node1 = graph.insertNode();
        const auto node2 = graph.insertNode();
        const auto node3 = graph.insertNode();
        const auto node4 = graph.insertNode();
        const auto node5 = graph.insertNode();
        graph.insertEdge(node1, node3);
        const auto edge2 = graph.insertEdge(node2, node4);
        graph.insertEdge(node5, node4);
        const auto edge4 = graph.insertEdge(node4, node4);
        graph.removeEdge(edge2);
        graph.removeEdge(edge4);

        const auto edge = graph.edge(-3);
        REQUIRE(edge.index() == acore::INVALID_INDEX);
    }
}

TEST_CASE("end() const noexcept -> const_iterator [agraph::Graph]")
{
    SECTION("[empty]")
    {
        const agraph::Graph graph;

        REQUIRE(noexcept(graph.end()));

        const auto it = graph.end();

        REQUIRE(it.index() == acore::INVALID_INDEX);
        REQUIRE(it.iterable() == &graph);
    }

    SECTION("[data]")
    {
        agraph::Graph graph;
        graph.insertNode();
        const auto node1 = graph.insertNode();
        const auto node2 = graph.insertNode();
        graph.insertEdge(node1, node2);
        graph.insertEdge(node2, node1);

        const auto it = graph.end();
        REQUIRE(it.index() == acore::INVALID_INDEX);
        REQUIRE(it.iterable() == &graph);
    }

    SECTION("[removed]")
    {
        agraph::Graph graph;
        const auto node = graph.insertNode();
        const auto node1 = graph.insertNode();
        const auto node2 = graph.insertNode();
        const auto edge = graph.insertEdge(node1, node2);
        graph.insertEdge(node2, node1);
        graph.removeEdge(edge);
        graph.removeNode(node);

        const auto it = graph.end();
        REQUIRE(it.index() == acore::INVALID_INDEX);
        REQUIRE(it.iterable() == &graph);
    }
}

TEST_CASE("edgeCount() const -> acore::size_type [agraph::Graph]")
{
    SECTION("[empty]")
    {
        const agraph::Graph graph;

        REQUIRE(graph.edgeCount() == 0);
    }

    SECTION("[data, no edges]")
    {
        agraph::Graph graph;
        graph.insertNode();
        graph.insertNode();
        graph.insertNode();

        REQUIRE(graph.edgeCount() == 0);
    }

    SECTION("[data]")
    {
        agraph::Graph graph;
        const auto node1 = graph.insertNode();
        const auto node2 = graph.insertNode();
        const auto node3 = graph.insertNode();
        const auto node4 = graph.insertNode();
        const auto node5 = graph.insertNode();
        graph.insertEdge(node1, node3);
        graph.insertEdge(node2, node4);
        graph.insertEdge(node5, node4);
        graph.insertEdge(node4, node4);

        REQUIRE(graph.edgeCount() == 4);
    }

    SECTION("[data, removed edges]")
    {
        agraph::Graph graph;
        const auto node1 = graph.insertNode();
        const auto node2 = graph.insertNode();
        const auto node3 = graph.insertNode();
        const auto node4 = graph.insertNode();
        const auto node5 = graph.insertNode();
        graph.insertEdge(node1, node3);
        const auto edge2 = graph.insertEdge(node2, node4);
        graph.insertEdge(node5, node4);
        const auto edge4 = graph.insertEdge(node4, node4);
        graph.removeEdge(edge2);
        graph.removeEdge(edge4);

        REQUIRE(graph.edgeCount() == 2);
    }

    SECTION("[data, removed node]")
    {
        agraph::Graph graph;
        const auto node1 = graph.insertNode();
        const auto node2 = graph.insertNode();
        const auto node3 = graph.insertNode();
        const auto node4 = graph.insertNode();
        const auto node5 = graph.insertNode();
        graph.insertEdge(node1, node3);
        graph.insertEdge(node2, node4);
        graph.insertEdge(node5, node4);
        graph.insertEdge(node4, node4);
        graph.removeNode(node4);

        REQUIRE(graph.edgeCount() == 1);
    }
}

TEST_CASE("nextEdgeIndex() const noexcept -> acore::size_type [agraph::Graph]")
{
    SECTION("[empty]")
    {
        const agraph::Graph graph;

        REQUIRE(noexcept(graph.nextEdgeIndex()));
        REQUIRE(graph.nextEdgeIndex() == -2);
    }

    SECTION("[data]")
    {
        agraph::Graph graph;
        const auto node1 = graph.insertNode();
        const auto node2 = graph.insertNode();
        const auto node3 = graph.insertNode();
        const auto node4 = graph.insertNode();
        const auto node5 = graph.insertNode();
        graph.insertEdge(node1, node3);
        graph.insertEdge(node2, node4);
        graph.insertEdge(node5, node4);
        graph.insertEdge(node4, node4);

        REQUIRE(graph.nextEdgeIndex() == -6);
    }

    SECTION("[data, removed edge]")
    {
        agraph::Graph graph;
        const auto node1 = graph.insertNode();
        const auto node2 = graph.insertNode();
        const auto node3 = graph.insertNode();
        const auto node4 = graph.insertNode();
        const auto node5 = graph.insertNode();
        graph.insertEdge(node1, node3);
        graph.insertEdge(node2, node4);
        const auto edge = graph.insertEdge(node5, node4);
        graph.insertEdge(node4, node4);
        graph.removeEdge(edge);

        REQUIRE(graph.nextEdgeIndex() == -4);
    }

    SECTION("[data, removed node]")
    {
        agraph::Graph graph;
        const auto node1 = graph.insertNode();
        const auto node2 = graph.insertNode();
        const auto node3 = graph.insertNode();
        const auto node4 = graph.insertNode();
        const auto node5 = graph.insertNode();
        graph.insertEdge(node1, node3);
        graph.insertEdge(node2, node4);
        graph.insertEdge(node5, node4);
        graph.insertEdge(node4, node4);
        graph.removeNode(node4);

        REQUIRE(graph.nextEdgeIndex() == -3);
    }
}

TEST_CASE("nextNodeIndex() const noexcept -> acore::size_type [agraph::Graph]")
{
    SECTION("[empty]")
    {
        const agraph::Graph graph;

        REQUIRE(noexcept(graph.nextNodeIndex()));
        REQUIRE(graph.nextNodeIndex() == 0);
    }

    SECTION("[data]")
    {
        agraph::Graph graph;
        graph.insertNode();
        graph.insertNode();
        graph.insertNode();

        REQUIRE(graph.nextNodeIndex() == 3);
    }

    SECTION("[data]")
    {
        agraph::Graph graph;
        graph.insertNode();
        const auto node = graph.insertNode();
        graph.insertNode();
        graph.removeNode(node);

        REQUIRE(graph.nextNodeIndex() == 1);
    }
}

TEST_CASE("node(acore::size_type index) const -> Node [agraph::Graph]")
{
    SECTION("[missing]")
    {
        const agraph::Graph graph;
        const auto node = graph.node(0);

        REQUIRE(node.index() == acore::INVALID_INDEX);
    }

    SECTION("[existing]")
    {
        agraph::Graph graph;
        const auto from = graph.insertNode();
        const auto node = graph.insertNode();
        const auto to = graph.insertNode();
        graph.insertEdge(from, to);
        graph.insertEdge(to, from);
        graph.insertEdge(from, to);

        REQUIRE(graph.node(1) == node);
    }

    SECTION("[removed]")
    {
        agraph::Graph graph;
        const auto node1 = graph.insertNode();
        const auto node2 = graph.insertNode();
        const auto node3 = graph.insertNode();
        const auto node4 = graph.insertNode();
        const auto node5 = graph.insertNode();
        graph.insertEdge(node1, node3);
        graph.insertEdge(node2, node4);
        graph.insertEdge(node5, node4);
        graph.insertEdge(node4, node4);
        graph.removeNode(node4);

        const auto node = graph.node(3);
        REQUIRE(node.index() == acore::INVALID_INDEX);
    }
}

TEST_CASE("nodeCount() const -> acore::size_type [agraph::Graph]")
{
    SECTION("[empty]")
    {
        const agraph::Graph graph;

        REQUIRE(graph.nodeCount() == 0);
    }

    SECTION("[data, no edges]")
    {
        agraph::Graph graph;
        graph.insertNode();
        graph.insertNode();
        graph.insertNode();

        REQUIRE(graph.nodeCount() == 3);
    }

    SECTION("[data]")
    {
        agraph::Graph graph;
        const auto node1 = graph.insertNode();
        const auto node2 = graph.insertNode();
        const auto node3 = graph.insertNode();
        const auto node4 = graph.insertNode();
        graph.insertEdge(node1, node3);
        graph.insertEdge(node2, node4);
        graph.insertEdge(node4, node4);

        REQUIRE(graph.nodeCount() == 4);
    }

    SECTION("[data, removed node]")
    {
        agraph::Graph graph;
        const auto node1 = graph.insertNode();
        const auto node2 = graph.insertNode();
        const auto node3 = graph.insertNode();
        const auto node4 = graph.insertNode();
        const auto node5 = graph.insertNode();
        graph.insertEdge(node1, node3);
        graph.insertEdge(node2, node4);
        graph.insertEdge(node5, node4);
        graph.insertEdge(node4, node4);
        graph.removeNode(node4);

        REQUIRE(graph.nodeCount() == 4);
    }
}

TEST_CASE("removeEdge(const Edge &edge) -> void [agraph::Graph]")
{
    SECTION("[invalid]")
    {
        agraph::Graph graph;
        const auto edge = graph.edge(-2);
        graph.removeEdge(edge);
        REQUIRE(graph == std::vector<std::pair<acore::size_type, std::vector<acore::size_type>>>{});
    }

    SECTION("[existing]")
    {
        agraph::Graph graph;
        const auto node1 = graph.insertNode();
        const auto node2 = graph.insertNode();
        const auto edge = graph.insertEdge(node1, node2);
        graph.insertEdge(node2, node1);

        graph.removeEdge(edge);

        REQUIRE(graph == std::vector<std::pair<acore::size_type, std::vector<acore::size_type>>>{{0, {}}, {1, {-3}}});
    }

    SECTION("[existing, multiple]")
    {
        agraph::Graph graph;
        const auto node1 = graph.insertNode();
        const auto node2 = graph.insertNode();
        graph.insertEdge(node1, node2);
        const auto edge = graph.insertEdge(node1, node2);
        graph.insertEdge(node1, node2);
        graph.insertEdge(node2, node1);

        graph.removeEdge(edge);

        REQUIRE(graph == std::vector<std::pair<acore::size_type, std::vector<acore::size_type>>>{{0, {-4, -2}}, {1, {-5}}});
    }

    SECTION("[removed]")
    {
        agraph::Graph graph;
        const auto node1 = graph.insertNode();
        const auto node2 = graph.insertNode();
        const auto edge = graph.insertEdge(node1, node2);
        graph.insertEdge(node2, node1);

        graph.removeEdge(edge);
        graph.removeEdge(edge);

        REQUIRE(graph == std::vector<std::pair<acore::size_type, std::vector<acore::size_type>>>{{0, {}}, {1, {-3}}});
    }
}

TEST_CASE("removeNode(const Node &node) -> void [agraph::Graph]")
{
    SECTION("[missing]")
    {
        agraph::Graph graph;
        const auto node = graph.node(0);
        graph.removeNode(node);
        REQUIRE(graph == std::vector<std::pair<acore::size_type, std::vector<acore::size_type>>>{});
    }

    SECTION("[existing, no edges]")
    {
        agraph::Graph graph;
        graph.insertNode();
        graph.insertNode();
        const auto node = graph.insertNode();

        graph.removeNode(node);

        REQUIRE(graph == std::vector<std::pair<acore::size_type, std::vector<acore::size_type>>>{{0, {}}, {1, {}}});
    }

    SECTION("[existing, incoming edges]")
    {
        agraph::Graph graph;
        const auto node1 = graph.insertNode();
        const auto node2 = graph.insertNode();
        const auto node3 = graph.insertNode();
        const auto node4 = graph.insertNode();
        const auto node5 = graph.insertNode();
        graph.insertEdge(node1, node4);
        graph.insertEdge(node2, node4);
        graph.insertEdge(node3, node4);
        graph.insertEdge(node5, node4);

        graph.removeNode(node4);

        REQUIRE(graph == std::vector<std::pair<acore::size_type, std::vector<acore::size_type>>>{{0, {}}, {1, {}}, {2, {}}, {4, {}}});
    }

    SECTION("[existing, outgoing edges]")
    {
        agraph::Graph graph;
        const auto node1 = graph.insertNode();
        const auto node2 = graph.insertNode();
        const auto node3 = graph.insertNode();
        const auto node4 = graph.insertNode();
        const auto node5 = graph.insertNode();
        graph.insertEdge(node2, node1);
        graph.insertEdge(node2, node2);
        graph.insertEdge(node2, node3);
        graph.insertEdge(node2, node4);
        graph.insertEdge(node2, node5);

        graph.removeNode(node2);

        REQUIRE(graph == std::vector<std::pair<acore::size_type, std::vector<acore::size_type>>>{{0, {}}, {2, {}}, {3, {}}, {4, {}}});
    }

    SECTION("[existing]")
    {
        agraph::Graph graph;
        const auto node1 = graph.insertNode();
        const auto node2 = graph.insertNode();
        const auto node3 = graph.insertNode();
        const auto node4 = graph.insertNode();
        const auto node5 = graph.insertNode();
        graph.insertEdge(node1, node3);
        graph.insertEdge(node2, node4);
        graph.insertEdge(node2, node5);
        graph.insertEdge(node4, node4);
        graph.insertEdge(node5, node1);
        graph.insertEdge(node5, node1);

        graph.removeNode(node4);

        REQUIRE(graph == std::vector<std::pair<acore::size_type, std::vector<acore::size_type>>>{{0, {-2}}, {1, {-4}}, {2, {}}, {4, {-7, -6}}});
    }

    SECTION("[removed]")
    {
        agraph::Graph graph;
        const auto node1 = graph.insertNode();
        const auto node2 = graph.insertNode();
        const auto node3 = graph.insertNode();
        const auto node4 = graph.insertNode();
        const auto node5 = graph.insertNode();
        graph.insertEdge(node1, node3);
        graph.insertEdge(node2, node4);
        graph.insertEdge(node2, node5);
        graph.insertEdge(node4, node4);
        graph.insertEdge(node5, node1);
        graph.insertEdge(node5, node1);

        graph.removeNode(node4);
        graph.removeNode(node4);

        REQUIRE(graph == std::vector<std::pair<acore::size_type, std::vector<acore::size_type>>>{{0, {-2}}, {1, {-4}}, {2, {}}, {4, {-7, -6}}});
    }
}

TEST_CASE("storage() noexcept -> Data * [agraph::Graph]")
{
    agraph::Graph graph;
    REQUIRE(noexcept(graph.storage()));
    REQUIRE(graph.storage());
}

TEST_CASE("storage() const noexcept -> const Data * [agraph::Graph]")
{
    const agraph::Graph graph;
    REQUIRE(noexcept(graph.storage()));
    REQUIRE(std::as_const(graph).storage());
}

TEST_CASE("agraph::Graph [examples]")
{
    SECTION("[usage]")
    {
        // clang-format off
        //! [[Usage]]
agraph::Graph graph;

//creating nodes & edges
const auto node1 = graph.insertNode(); //0
const auto node2 = graph.insertNode(); //1
const auto node3 = graph.insertNode(); //2
graph.insertEdge(node1, node2); //-2
graph.insertEdge(node2, node3); //-3

//exploring the graph depth first (simplified)
std::vector<acore::size_type> elementIds;
auto node = *graph.begin();

while (true)
{
    elementIds.push_back(node.index());
    auto it = node.begin();

    if (it != node.end())
    {
        const auto edge = (*it);
        elementIds.push_back(edge.index());
        node = edge.to();
    }
    else
    {
        break;
    }
}

//elementIds == {0, -2, 1, -3, 2};
        //! [[Usage]]
        // clang-format on;

        REQUIRE(elementIds == std::vector<acore::size_type>{0, -2, 1, -3, 2});
    }

    SECTION("[navigation]")
    {

    }
}
}
