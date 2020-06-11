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

#include <string>
#include <type_traits>

namespace exceptiontest
{
TEST_CASE("[acore::Exception]")
{
#if defined(MSVC) && !defined(DEBUG)
    REQUIRE(std::is_standard_layout_v<acore::Exception>);
#endif
    REQUIRE(std::is_default_constructible_v<acore::Exception>);
    REQUIRE(std::is_copy_constructible_v<acore::Exception>);
    REQUIRE(std::is_nothrow_move_constructible_v<acore::Exception>);
    REQUIRE(std::is_copy_assignable_v<acore::Exception>);
    REQUIRE(std::is_nothrow_move_assignable_v<acore::Exception>);
    REQUIRE(std::is_nothrow_destructible_v<acore::Exception>);
}

TEST_CASE("what() const noexcept -> const char * [acore::Exception]")
{
    acore::Exception exception;
    exception << "Test " << 3;
    REQUIRE(noexcept(exception.what()));
    REQUIRE(exception.what() == std::string{"Test 3"});
}

TEST_CASE("operator<<(const T &text) -> Exception && [acore::Exception]")
{
    acore::Exception exception;
    exception << "Test" << ' ' << 4;
    REQUIRE(exception.what() == std::string{"Test 4"});
}

TEST_CASE("acore::Exception [examples]")
{
    SECTION("[usage]")
    {
        try
        {
            // clang-format off
//! [[Usage]]
try
{
    throw acore::Exception{} << "Error message #" << 1;
}
catch(acore::Exception &e)
{
    throw std::string{e.what()}; //"Error message #1"
}
//! [[Usage]]
            // clang-format on
        }
        catch (std::string &w)
        {
            REQUIRE(std::string{w} == std::string("Error message #1"));
        }
    }
}
}
