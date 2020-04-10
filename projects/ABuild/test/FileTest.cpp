#include <Catch2/catch.hpp>

#include <ABuild.hpp>

namespace abuildfiletest
{
TEST_CASE("File(std::filesystem::path path) [abuild::File]")
{
    REQUIRE_NOTHROW(abuild::File{{}});
}

TEST_CASE("path() const noexcept -> const std::filesystem::path & [abuild::File]")
{
    REQUIRE(abuild::File{{}}.path() == std::filesystem::path{});
    REQUIRE(abuild::File{std::filesystem::current_path()}.path() == std::filesystem::current_path());
}
}
