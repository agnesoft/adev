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

namespace breadthfirstsearchtest
{
class SearchHandler
{
public:
    enum class Type
    {
        Continue,
        SkipEdge,
        Stop,
        Finish,
        FinishAlt
    };

    SearchHandler() = default;

    explicit SearchHandler(Type type) :
        mType{type}
    {
    }

    [[nodiscard]] auto distances() const -> std::vector<acore::size_type>
    {
        return mDistances;
    }

    [[nodiscard]] auto operator()(acore::size_type index, acore::size_type distance) const -> agraph::SearchControl
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
            return index == 1 ? agraph::SearchControl::Finish : agraph::SearchControl::Continue;
        case Type::FinishAlt:
            return index == -6 ? agraph::SearchControl::Finish : agraph::SearchControl::Continue; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        }

        return agraph::SearchControl::Continue;
    }

private:
    mutable std::vector<acore::size_type> mDistances;
    Type mType = Type::Continue;
};

TEST_CASE("from(const typename GraphType::Node &node, const Handler &handler) -> std::vector<acore::size_type> [agraph::BreadthFirstSearch]")
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
            SearchHandler handler;
            REQUIRE(agraph::BreadthFirstSearch<agraph::Graph>::from(root, handler) == std::vector<acore::size_type>{0, -4, -3, -2, 3, 2, 1, -9, -8, 5});
            REQUIRE(handler.distances() == std::vector<acore::size_type>{0, 1, 1, 1, 2, 2, 2, 3, 3, 4});
        }

        SECTION("[skip edges]")
        {
            SearchHandler handler{SearchHandler::Type::SkipEdge};
            REQUIRE(agraph::BreadthFirstSearch<agraph::Graph>::from(root, handler) == std::vector<acore::size_type>{0, 3, 2, 1, 5});
            REQUIRE(handler.distances() == std::vector<acore::size_type>{0, 1, 1, 1, 2, 2, 2, 3, 3, 4});
        }

        SECTION("[odd nodes]")
        {
            SearchHandler handler{SearchHandler::Type::Stop};
            REQUIRE(agraph::BreadthFirstSearch<agraph::Graph>::from(root, handler) == std::vector<acore::size_type>{0, -4, -3, -2, 3, 1});
            REQUIRE(handler.distances() == std::vector<acore::size_type>{0, 1, 1, 1, 2, 2, 2});
        }

        SECTION("[short circuit]")
        {
            SearchHandler handler{SearchHandler::Type::Finish};
            REQUIRE(agraph::BreadthFirstSearch<agraph::Graph>::from(root, handler) == std::vector<acore::size_type>{0, -4, -3, -2, 3, 2});
            REQUIRE(handler.distances() == std::vector<acore::size_type>{0, 1, 1, 1, 2, 2, 2});
        }
    }

    SECTION("[cyclic]")
    {
        graph.insertEdge(node5, root);
        graph.insertEdge(node5, node4);

        SECTION("[all]")
        {
            SearchHandler handler;
            REQUIRE(agraph::BreadthFirstSearch<agraph::Graph>::from(root, handler) == std::vector<acore::size_type>{0, -4, -3, -2, 3, 2, 1, -9, -8, 5, -11, -10, 4, -7, -6, -5});
            REQUIRE(handler.distances() == std::vector<acore::size_type>{0, 1, 1, 1, 2, 2, 2, 3, 3, 4, 5, 5, 6, 7, 7, 7});
        }

        SECTION("[skip edges]")
        {
            SearchHandler handler{SearchHandler::Type::SkipEdge};
            REQUIRE(agraph::BreadthFirstSearch<agraph::Graph>::from(root, handler) == std::vector<acore::size_type>{0, 3, 2, 1, 5, 4});
            REQUIRE(handler.distances() == std::vector<acore::size_type>{0, 1, 1, 1, 2, 2, 2, 3, 3, 4, 5, 5, 6, 7, 7, 7});
        }
    }

    SECTION("[invalid]")
    {
        REQUIRE_THROWS_AS(agraph::BreadthFirstSearch<agraph::Graph>::from(graph.node(100), SearchHandler{}), acore::Exception);
    }
}

TEST_CASE("to(const typename GraphType::Node &node, const Handler &handler) -> std::vector<acore::size_type> [agraph::BreadthFirstSearch]")
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
            SearchHandler handler;
            REQUIRE(agraph::BreadthFirstSearch<agraph::Graph>::to(node5, handler) == std::vector<acore::size_type>{5, -9, -8, 2, -6, -3, 4, 0});
            REQUIRE(handler.distances() == std::vector<acore::size_type>{0, 1, 1, 2, 3, 3, 4, 4});
        }

        SECTION("[skip edges]")
        {
            SearchHandler handler{SearchHandler::Type::SkipEdge};
            REQUIRE(agraph::BreadthFirstSearch<agraph::Graph>::to(node5, handler) == std::vector<acore::size_type>{5, 2, 4, 0});
            REQUIRE(handler.distances() == std::vector<acore::size_type>{0, 1, 1, 2, 3, 3, 4, 4});
        }

        SECTION("[odd nodes]")
        {
            SearchHandler handler{SearchHandler::Type::Stop};
            REQUIRE(agraph::BreadthFirstSearch<agraph::Graph>::to(node5, handler) == std::vector<acore::size_type>{5, -9, -8});
            REQUIRE(handler.distances() == std::vector<acore::size_type>{0, 1, 1, 2});
        }

        SECTION("[short circuit]")
        {
            SearchHandler handler{SearchHandler::Type::FinishAlt};
            REQUIRE(agraph::BreadthFirstSearch<agraph::Graph>::to(node5, handler) == std::vector<acore::size_type>{5, -9, -8, 2, -3});
            REQUIRE(handler.distances() == std::vector<acore::size_type>{0, 1, 1, 2, 3, 3});
        }
    }

    SECTION("[cyclic]")
    {
        graph.insertEdge(node5, root);
        graph.insertEdge(node5, node4);

        SECTION("[all]")
        {
            SearchHandler handler;
            REQUIRE(agraph::BreadthFirstSearch<agraph::Graph>::to(node5, handler) == std::vector<acore::size_type>{5, -9, -8, 2, -6, -3, 4, 0, -11, -10});
            REQUIRE(handler.distances() == std::vector<acore::size_type>{0, 1, 1, 2, 3, 3, 4, 4, 5, 5});
        }

        SECTION("[skip edges]")
        {
            SearchHandler handler{SearchHandler::Type::SkipEdge};
            REQUIRE(agraph::BreadthFirstSearch<agraph::Graph>::to(node5, handler) == std::vector<acore::size_type>{5, 2, 4, 0});
            REQUIRE(handler.distances() == std::vector<acore::size_type>{0, 1, 1, 2, 3, 3, 4, 4, 5, 5});
        }
    }

    SECTION("[invalid]")
    {
        REQUIRE_THROWS_AS(agraph::BreadthFirstSearch<agraph::Graph>::to(graph.node(100), SearchHandler{}), acore::Exception);
    }
}

TEST_CASE("agraph::BreadthFirstSearch [examples]")
{
    SECTION("[usage]")
    {
        // clang-format off
        //! [[Graph]]
agraph::Graph graph;
auto node1 = graph.insertNode(); //0
auto node2 = graph.insertNode(); //1
auto node3 = graph.insertNode(); //2
auto node4 = graph.insertNode(); //3

graph.insertEdge(node1, node2); //-2
graph.insertEdge(node1, node3); //-3
graph.insertEdge(node2, node4); //-4
graph.insertEdge(node3, node4); //-5
        //! [[Graph]]

        //! [[Usage]]
auto ids = agraph::BreadthFirstSearch<agraph::Graph>::from(
            node1,
            []([[maybe_unused]] acore::size_type index, [[maybe_unused]] acore::size_type distance) {
                return agraph::SearchControl::Continue;
            });
// ids == {
//  0: node1 (root, level 0)
// -3: edge1 (level 1)
// -2: edge2 (level 1)
//  2: node3 (level 2)
//  1: node2 (level 2)
// -5: edge3 (level 3)
// -4: edge4 (level 3)
//  3: node4 (level 4)
        //! [[Usage]]
        // clang-format on

        REQUIRE(ids == std::vector<acore::size_type>{0, -3, -2, 2, 1, -5, -4, 3});
    }
}
}
