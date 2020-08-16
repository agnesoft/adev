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

namespace resulttest
{
TEST_CASE("[adb::Result]")
{
#if defined(_MSC_VER) && !defined(_DEBUG)
    REQUIRE(std::is_standard_layout_v<adb::Result>);
#endif
    REQUIRE(std::is_default_constructible_v<adb::Result>);
    REQUIRE(std::is_copy_constructible_v<adb::Result>);
    REQUIRE(std::is_nothrow_move_constructible_v<adb::Result>);
    REQUIRE(std::is_copy_assignable_v<adb::Result>);
    REQUIRE(std::is_nothrow_move_assignable_v<adb::Result>);
    REQUIRE(std::is_nothrow_destructible_v<adb::Result>);
}

TEST_CASE("Result() [adb::Result]")
{
    REQUIRE_NOTHROW(adb::Result{});
}

TEST_CASE("Result(std::vector<Element) noexcept [adb::Result]")
{
    std::vector<adb::Element> data{{0, {{"Hello", "World"}, {"Second", "KeyValue"}}},
                                   {-2, {{1, 2}, {2, 3}, {3, 4}}},
                                   {1, {}}};
    REQUIRE(noexcept(adb::Result{std::move(data)})); //NOLINT(hicpp-invalid-access-moved, bugprone-use-after-move)
    REQUIRE_NOTHROW(adb::Result{std::move(data)}); //NOLINT(hicpp-invalid-access-moved, bugprone-use-after-move)
}
TEST_CASE("data() const noexcept -> const std::vector<adb::Element> & [adb::Result]")
{
    SECTION("[empty]")
    {
        REQUIRE(noexcept(adb::Result{}.data()));
        REQUIRE(adb::Result{}.data() == std::vector<adb::Element>{}); //NOLINT(readability-container-size-empty)
    }

    SECTION("[data]")
    {
        REQUIRE(adb::Result{{{0, {{"Hello", "World"}, {"Second", "KeyValue"}}},
                             {-2, {{1, 2}, {2, 3}, {3, 4}}},
                             {1, {}}}}
                    .data()
                == std::vector<adb::Element>{{0, {{"Hello", "World"}, {"Second", "KeyValue"}}},
                                             {-2, {{1, 2}, {2, 3}, {3, 4}}},
                                             {1, {}}});
    }
}
}
