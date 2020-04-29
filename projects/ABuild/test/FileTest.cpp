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
    const abuild::File file{{}};
    REQUIRE(noexcept(file.path()));
    REQUIRE(abuild::File{{}}.path() == std::filesystem::path{});
    REQUIRE(abuild::File{std::filesystem::current_path()}.path() == std::filesystem::current_path());
}

TEST_CASE("type() const noexcept -> Type [abuild::File]")
{
    const abuild::File file{{}};
    REQUIRE(noexcept(file.type()));

    SECTION("[other]")
    {
        REQUIRE(abuild::File{{}}.type() == abuild::File::Type::Other);
        REQUIRE(abuild::File{"file.txt"}.type() == abuild::File::Type::Other);
        REQUIRE(abuild::File{"file.docx"}.type() == abuild::File::Type::Other);
        REQUIRE(abuild::File{"cpp"}.type() == abuild::File::Type::Other);
        REQUIRE(abuild::File{".cpp/"}.type() == abuild::File::Type::Other);
        REQUIRE(abuild::File{".cpp"}.type() == abuild::File::Type::Other);
        REQUIRE(abuild::File{"source.cpp/"}.type() == abuild::File::Type::Other);
    }

    SECTION("[source]")
    {
        REQUIRE(abuild::File{"source.cpp"}.type() == abuild::File::Type::Source);
        REQUIRE(abuild::File{"source.cxx"}.type() == abuild::File::Type::Source);
        REQUIRE(abuild::File{"source.cc"}.type() == abuild::File::Type::Source);
        REQUIRE(abuild::File{"source.c"}.type() == abuild::File::Type::Source);
    }

    SECTION("[header]")
    {
        REQUIRE(abuild::File{"source.hpp"}.type() == abuild::File::Type::Header);
        REQUIRE(abuild::File{"source.hxx"}.type() == abuild::File::Type::Header);
        REQUIRE(abuild::File{"source.h"}.type() == abuild::File::Type::Header);
    }
}
}
