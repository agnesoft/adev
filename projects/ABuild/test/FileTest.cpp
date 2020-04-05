#include <Catch2/catch.hpp>

#include <ABuild.hpp>

namespace abuildfiletest
{
TEST_CASE("File(std::filesystem::path path) [abuild::File]")
{
    REQUIRE_NOTHROW(abuild::File{{}});
}
}
