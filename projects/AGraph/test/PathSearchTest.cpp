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
#include <vector>

namespace pathsearchtest
{
class SearchHandler
{
public:
    auto setWeights(const std::unordered_map<acore::size_type, acore::size_type> &weights) -> void
    {
        mWeights = weights;
    }

    [[nodiscard]] auto operator()(acore::size_type index, acore::size_type distance) const -> acore::size_type
    {
        const auto it = mWeights.find(index);

        if (it != mWeights.end())
        {
            return it->second;
        }

        return agraph::PathSearchHandler{}(index, distance);
    }

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
    const agraph::Graph graph;
    REQUIRE((agraph::PathSearch{&graph}).graph() == &graph);
}

TEST_CASE("path(const typename GraphType::Node &from, const typename GraphType::Node &to) -> std::vector<acore::size_type> [agraph::PathSearch]")
{
    agraph::Graph graph;
    agraph::PathSearch path{&graph};

    SECTION("[triangle graph]")
    {
        const auto node1 = graph.insertNode();
        const auto node2 = graph.insertNode();
        const auto node3 = graph.insertNode();

        graph.insertEdge(node2, node3);
        graph.insertEdge(node1, node2);
        graph.insertEdge(node1, node3);

        SECTION("[unweighted]")
        {
            REQUIRE(agraph::PathSearch{&graph}.path(graph.node(0), graph.node(2)) == std::vector<acore::size_type>{0, -4, 2});
        }

        SECTION("[weighted]")
        {
            SearchHandler handler;
            handler.setWeights(std::unordered_map<acore::size_type, acore::size_type>{{-4, 10}});

            REQUIRE(agraph::PathSearch<agraph::Graph, SearchHandler>{&graph, handler}.path(graph.node(0), graph.node(2)) == std::vector<acore::size_type>{0, -3, 1, -2, 2});
        }
    }

    SECTION("[full graph]")
    {
        constexpr acore::size_type MAP_HEIGHT = 5;
        constexpr acore::size_type MAP_WIDTH = 5;
        std::array<std::array<acore::size_type, MAP_WIDTH>, MAP_HEIGHT> nodes;

        for (acore::size_type i = 0; i < MAP_HEIGHT; i++)
        {
            for (acore::size_type j = 0; j < MAP_WIDTH; j++)
            {
                nodes[i][j] = graph.insertNode().index();
            }
        }

        for (acore::size_type i = 0; i < MAP_HEIGHT; i++)
        {
            for (acore::size_type j = 0; j < MAP_WIDTH; j++)
            {
                const auto &node = graph.node(nodes[i][j]);

                if (i < (MAP_HEIGHT - 1))
                {
                    graph.insertEdge(node, graph.node(nodes[i + 1][j]));
                }

                if (0 < i)
                {
                    graph.insertEdge(node, graph.node(nodes[i - 1][j]));
                }

                if (j < (MAP_WIDTH - 1))
                {
                    graph.insertEdge(node, graph.node(nodes[i][j + 1]));
                }

                if (0 < j)
                {
                    graph.insertEdge(node, graph.node(nodes[i][j - 1]));
                }
            }
        }

        SECTION("[unweighted]")
        {
            REQUIRE(agraph::PathSearch{&graph}.path(graph.node(1), graph.node(8)) == std::vector<acore::size_type>{1, -5, 2, -8, 3, -10, 8});
        }

        SECTION("[weighted]")
        {
            constexpr acore::size_type FOREST_WEIGHT = 10;
            constexpr acore::size_type MOUNTAIN_WEIGHT = 0;
            constexpr acore::size_type ROAD_WEIGHT = 2;
            constexpr acore::size_type TOWN_WEIGHT = 1;
            std::unordered_map<acore::size_type, acore::size_type> w;

            // clang-format off
            w[0] = MOUNTAIN_WEIGHT; w[1] = TOWN_WEIGHT;      w[2] = MOUNTAIN_WEIGHT; w[3] = MOUNTAIN_WEIGHT; w[4] = MOUNTAIN_WEIGHT;
            w[5] = FOREST_WEIGHT;   w[6] = ROAD_WEIGHT;      w[7] = FOREST_WEIGHT;   w[8] = TOWN_WEIGHT;     w[9] = MOUNTAIN_WEIGHT;
            w[10] = FOREST_WEIGHT;  w[11] = TOWN_WEIGHT;     w[12] = ROAD_WEIGHT;    w[13] = ROAD_WEIGHT;    w[14] = MOUNTAIN_WEIGHT;
            w[15] = FOREST_WEIGHT;  w[16] = FOREST_WEIGHT;   w[17] = MOUNTAIN_WEIGHT;w[18] = MOUNTAIN_WEIGHT;w[19] = MOUNTAIN_WEIGHT;
            w[20] = MOUNTAIN_WEIGHT;w[21] = MOUNTAIN_WEIGHT; w[22] = TOWN_WEIGHT;    w[23] = ROAD_WEIGHT;    w[24] = FOREST_WEIGHT;
            // clang-format on

            SearchHandler handler;
            handler.setWeights(w);

            SECTION("[reachable]")
            {
                REQUIRE(agraph::PathSearch<agraph::Graph, SearchHandler>{&graph, handler}.path(graph.node(1), graph.node(8)) == std::vector<acore::size_type>{1, -4, 6, -18, 11, -38, 12, -42, 13, -45, 8});
            }

            SECTION("[unreachable]")
            {
                REQUIRE(agraph::PathSearch<agraph::Graph, SearchHandler>{&graph, handler}.path(graph.node(1), graph.node(22)) == std::vector<acore::size_type>{});
            }
        }
    }

    SECTION("[multi edge]")
    {
        const auto from = graph.insertNode();
        const auto to = graph.insertNode();

        graph.insertEdge(from, to);
        graph.insertEdge(from, to);

        SECTION("[unweighted]")
        {
            REQUIRE(agraph::PathSearch{&graph}.path(graph.node(0), graph.node(1)) == std::vector<acore::size_type>{0, -3, 1});
        }

        SECTION("[weighted]")
        {
            SearchHandler handler;
            handler.setWeights(std::unordered_map<acore::size_type, acore::size_type>{{-3, 10}});

            REQUIRE(agraph::PathSearch<agraph::Graph, SearchHandler>{&graph, handler}.path(graph.node(0), graph.node(1)) == std::vector<acore::size_type>{0, -2, 1});
        }
    }

    SECTION("[from invalid]")
    {
        graph.insertNode();

        REQUIRE_THROWS_AS(path.path(graph.node(1), graph.node(0)), acore::Exception);
    }

    SECTION("[to invalid]")
    {
        graph.insertNode();

        REQUIRE_THROWS_AS(path.path(graph.node(0), graph.node(1)), acore::Exception);
    }
}
}
