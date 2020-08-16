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

namespace elementtest
{
class Graph;

class TestElement : public agraph::Element<TestElement, Graph>
{
public:
    constexpr TestElement() noexcept = default;
    constexpr TestElement(acore::size_type index, const Graph *graph) noexcept :
        agraph::Element<TestElement, Graph>{index, graph}
    {
    }
};

class Graph
{
public:
    [[nodiscard]] constexpr auto contains(const TestElement &element) const noexcept -> bool
    {
        return element.index() == mIndex;
    }

    constexpr auto setIndex(acore::size_type index) -> void
    {
        mIndex = index;
    }

private:
    acore::size_type mIndex = acore::INVALID_INDEX;
};

TEST_CASE("[agraph::Element]")
{
    REQUIRE(std::is_copy_constructible_v<TestElement>);
    REQUIRE(std::is_copy_assignable_v<TestElement>);
    REQUIRE(std::is_nothrow_move_constructible_v<TestElement>);
    REQUIRE(std::is_nothrow_move_assignable_v<TestElement>);
    REQUIRE(std::is_nothrow_destructible_v<TestElement>);
}

TEST_CASE("Element() noexcept [agraph::Element]")
{
    REQUIRE(noexcept(TestElement{}));
}

TEST_CASE("graph() const noexcept -> const GraphType * [agraph::Element]")
{
    const Graph graph;

    REQUIRE(noexcept(TestElement{}.graph()));
    REQUIRE(TestElement{}.graph() == nullptr);
    REQUIRE(TestElement{acore::INVALID_INDEX, nullptr}.graph() == nullptr);
    REQUIRE(TestElement{acore::INVALID_INDEX, &graph}.graph() == &graph);
}

TEST_CASE("index() const noexcept -> acore::size_type [agraph::Element]")
{
    REQUIRE(noexcept(TestElement{}.index()));
    REQUIRE(TestElement{}.index() == acore::INVALID_INDEX);
    REQUIRE(TestElement{acore::INVALID_INDEX, nullptr}.index() == acore::INVALID_INDEX);
    REQUIRE(TestElement{1, nullptr}.index() == 1);
    REQUIRE(TestElement{-2, nullptr}.index() == -2);
}

TEST_CASE("isValid() const noexcept -> bool [agraph::Element]")
{
    Graph graph;

    REQUIRE(noexcept(TestElement{}.isValid()));
    REQUIRE_FALSE(TestElement{}.isValid());
    REQUIRE_FALSE(TestElement{acore::INVALID_INDEX, nullptr}.isValid());
    REQUIRE_FALSE(TestElement{acore::INVALID_INDEX, &graph}.isValid());
    REQUIRE_FALSE(TestElement{1, &graph}.isValid());

    graph.setIndex(1);

    REQUIRE(TestElement{1, &graph}.isValid());
    REQUIRE_FALSE(TestElement{2, &graph}.isValid());
}

TEST_CASE("Element(acore::size_type index, const GraphType *graph) noexcept [agraph::Element]")
{
    const Graph graph;
    REQUIRE(noexcept(TestElement{1, &graph}));
}

TEST_CASE("operator==(Element left, Element right) noexcept -> bool [agraph::Element]")
{
    const Graph graph;

    REQUIRE(noexcept(TestElement{} == TestElement{}));
    REQUIRE(TestElement{} == TestElement{});
    REQUIRE(TestElement{1, nullptr} == TestElement{1, nullptr});
    REQUIRE(TestElement{1, &graph} == TestElement{1, &graph});
    REQUIRE_FALSE(TestElement{} == TestElement{1, &graph});
    REQUIRE_FALSE(TestElement{0, &graph} == TestElement{1, &graph});
    REQUIRE_FALSE(TestElement{1, &graph} == TestElement{1, nullptr});
}

TEST_CASE("operator!=(Element left, Element right) noexcept -> bool [agraph::Element]")
{
    const Graph graph;

    REQUIRE(noexcept(TestElement{} != TestElement{}));
    REQUIRE_FALSE(TestElement{} != TestElement{});
    REQUIRE_FALSE(TestElement{1, nullptr} != TestElement{1, nullptr});
    REQUIRE_FALSE(TestElement{1, &graph} != TestElement{1, &graph});
    REQUIRE(TestElement{} != TestElement{1, &graph});
    REQUIRE(TestElement{0, &graph} != TestElement{1, &graph});
    REQUIRE(TestElement{1, &graph} != TestElement{1, nullptr});
}
}
