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
#include <vector>

namespace depthfirstsearchtest
{
class SearchHandler
{
public:
    enum class Type
    {
        Continue,
        SkipEdge,
        Stop,
        Finish
    };

    SearchHandler() = default;

    explicit SearchHandler(Type type) :
        mType{type}
    {
    }

    auto distances() const -> std::vector<acore::size_type>
    {
        return mDistances;
    }

    auto operator()(acore::size_type index, acore::size_type distance) -> agraph::SearchControl
    {
        mDistances.push_back(distance);

        switch (mType)
        {
        case Type::Continue:
            return agraph::SearchControl::Continue;
        case Type::SkipEdge:
            return index < acore::INVALID_INDEX ? agraph::SearchControl::Skip : agraph::SearchControl::Continue;
        case Type::Stop:
            return index == 2 ? agraph::SearchControl::Stop : agraph::SearchControl::Continue;
        case Type::Finish:
            return index == 2 ? agraph::SearchControl::Finish : agraph::SearchControl::Continue;
        }

        return agraph::SearchControl::Continue;
    }

private:
    std::vector<acore::size_type> mDistances;
    Type mType = Type::Continue;
};

TEST_CASE("[agraph::DepthFirstSearch]")
{
}

TEST_CASE("from(const typename GraphType::Node &node) -> std::vector<acore::size_type> [agraph::DepthFirstSearch]")
{
    agraph::Graph graph;
    const auto root = graph.insertNode();
    const auto node1 = graph.insertNode();
    const auto node2 = graph.insertNode();
    const auto node3 = graph.insertNode();
    graph.insertEdge(root, node1);
    graph.insertEdge(root, node2);
    graph.insertEdge(root, node3);

    const auto node4 = graph.insertNode();
    graph.insertEdge(node4, node1);
    graph.insertEdge(node4, node2);
    graph.insertEdge(node4, node3);

    const auto node5 = graph.insertNode();
    graph.insertEdge(node2, node5);
    graph.insertEdge(node2, node5);

    SECTION("[acyclic]")
    {
        SECTION("[all]")
        {
            agraph::DepthFirstSearch<agraph::Graph, SearchHandler> search{&graph, SearchHandler{}};
            REQUIRE(search.from(root) == std::vector<acore::size_type>{0, -2, 1, -3, 2, -8, 5, -9, -4, 3});
            REQUIRE(search.handler().distances() == std::vector<acore::size_type>{0, 1, 2, 1, 2, 3, 4, 3, 1, 2});
        }

        SECTION("[skip edges]")
        {
            agraph::DepthFirstSearch<agraph::Graph, SearchHandler> search{&graph, SearchHandler{SearchHandler::Type::SkipEdge}};
            REQUIRE(search.from(root) == std::vector<acore::size_type>{0, 1, 2, 5, 3});
            REQUIRE(search.handler().distances() == std::vector<acore::size_type>{0, 1, 2, 1, 2, 3, 4, 3, 1, 2});
        }

        SECTION("[odd nodes]")
        {
            agraph::DepthFirstSearch<agraph::Graph, SearchHandler> search{&graph, SearchHandler{SearchHandler::Type::Stop}};
            REQUIRE(search.from(root) == std::vector<acore::size_type>{0, -2, 1, -3, -4, 3});
            REQUIRE(search.handler().distances() == std::vector<acore::size_type>{0, 1, 2, 1, 2, 1, 2});
        }

        SECTION("[short circuit]")
        {
            agraph::DepthFirstSearch<agraph::Graph, SearchHandler> search{&graph, SearchHandler{SearchHandler::Type::Finish}};
            REQUIRE(search.from(root) == std::vector<acore::size_type>{0, -2, 1, -3});
            REQUIRE(search.handler().distances() == std::vector<acore::size_type>{0, 1, 2, 1, 2});
        }
    }

    SECTION("[cyclic]")
    {
        graph.insertEdge(node5, root);
        graph.insertEdge(node5, node4);

        SECTION("[all]")
        {
            agraph::DepthFirstSearch<agraph::Graph, SearchHandler> search{&graph, SearchHandler{}};
            REQUIRE(search.from(root) == std::vector<acore::size_type>{0, -2, 1, -3, 2, -8, 5, -10, -11, 4, -5, -6, -7, 3, -9, -4});
            REQUIRE(search.handler().distances() == std::vector<acore::size_type>{0, 1, 2, 1, 2, 3, 4, 5, 5, 6, 7, 7, 7, 8, 3, 1});
        }

        SECTION("[skip edges]")
        {
            agraph::DepthFirstSearch<agraph::Graph, SearchHandler> search{&graph, SearchHandler{SearchHandler::Type::SkipEdge}};
            REQUIRE(search.from(root) == std::vector<acore::size_type>{0, 1, 2, 5, 4, 3});
            REQUIRE(search.handler().distances() == std::vector<acore::size_type>{0, 1, 2, 1, 2, 3, 4, 5, 5, 6, 7, 7, 7, 8, 3, 1});
        }
    }

    SECTION("[invalid]")
    {
        agraph::DepthFirstSearch<agraph::Graph, SearchHandler> search{&graph, SearchHandler{}};
        REQUIRE_THROWS_AS(search.from(graph.node(100)), acore::Exception);
    }
}

TEST_CASE("to(const typename GraphType::Node &node) -> std::vector<acore::size_type> [agraph::DepthFirstSearch]")
{
    agraph::Graph graph;
    const auto root = graph.insertNode();
    const auto node1 = graph.insertNode();
    const auto node2 = graph.insertNode();
    const auto node3 = graph.insertNode();
    graph.insertEdge(root, node1);
    graph.insertEdge(root, node2);
    graph.insertEdge(root, node3);

    const auto node4 = graph.insertNode();
    graph.insertEdge(node4, node1);
    graph.insertEdge(node4, node2);
    graph.insertEdge(node4, node3);

    const auto node5 = graph.insertNode();
    graph.insertEdge(node2, node5);
    graph.insertEdge(node2, node5);

    SECTION("[acyclic]")
    {
        SECTION("[all]")
        {
            agraph::DepthFirstSearch<agraph::Graph, SearchHandler> search{&graph, SearchHandler{}};
            REQUIRE(search.to(node5) == std::vector<acore::size_type>{5, -8, 2, -3, 0, -6, 4, -9});
            REQUIRE(search.handler().distances() == std::vector<acore::size_type>{0, 1, 2, 3, 4, 3, 4, 1});
        }

        SECTION("[skip edges]")
        {
            agraph::DepthFirstSearch<agraph::Graph, SearchHandler> search{&graph, SearchHandler{SearchHandler::Type::SkipEdge}};
            REQUIRE(search.to(node5) == std::vector<acore::size_type>{5, 2, 0, 4});
            REQUIRE(search.handler().distances() == std::vector<acore::size_type>{0, 1, 2, 3, 4, 3, 4, 1});
        }

        SECTION("[odd nodes]")
        {
            agraph::DepthFirstSearch<agraph::Graph, SearchHandler> search{&graph, SearchHandler{SearchHandler::Type::Stop}};
            REQUIRE(search.to(node5) == std::vector<acore::size_type>{5, -8, -9});
            REQUIRE(search.handler().distances() == std::vector<acore::size_type>{0, 1, 2, 1});
        }

        SECTION("[short circuit]")
        {
            agraph::DepthFirstSearch<agraph::Graph, SearchHandler> search{&graph, SearchHandler{SearchHandler::Type::Finish}};
            REQUIRE(search.to(node5) == std::vector<acore::size_type>{5, -8});
            REQUIRE(search.handler().distances() == std::vector<acore::size_type>{0, 1, 2});
        }
    }

    SECTION("[cyclic]")
    {
        graph.insertEdge(node5, root);
        graph.insertEdge(node5, node4);

        SECTION("[all]")
        {
            agraph::DepthFirstSearch<agraph::Graph, SearchHandler> search{&graph, SearchHandler{}};
            REQUIRE(search.to(node5) == std::vector<acore::size_type>{5, -8, 2, -3, 0, -10, -6, 4, -11, -9});
            REQUIRE(search.handler().distances() == std::vector<acore::size_type>{0, 1, 2, 3, 4, 5, 3, 4, 5, 1});
        }

        SECTION("[skip edges]")
        {
            agraph::DepthFirstSearch<agraph::Graph, SearchHandler> search{&graph, SearchHandler{SearchHandler::Type::SkipEdge}};
            REQUIRE(search.to(node5) == std::vector<acore::size_type>{5, 2, 0, 4});
            REQUIRE(search.handler().distances() == std::vector<acore::size_type>{0, 1, 2, 3, 4, 5, 3, 4, 5, 1});
        }
    }

    SECTION("[invalid]")
    {
        agraph::DepthFirstSearch<agraph::Graph, SearchHandler> search{&graph, SearchHandler{}};
        REQUIRE_THROWS_AS(search.to(graph.node(100)), acore::Exception);
    }
}
}
