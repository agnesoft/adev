#include "pch.hpp"

#include <Catch2/catch.hpp>

namespace exceptiontest
{
TEST_CASE("[acore::Exception]")
{
#ifndef _WIN32
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
