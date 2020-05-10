#include "TestFile.hpp"

#include <Catch2/catch.hpp>

#include <ABuild.hpp>

namespace abuildfiletest
{

TEST_CASE("File(const std::filesystem::path &path) [abuild::File]")
{
    SECTION("[existing]")
    {
        const abuildtest::TestFile testFile{"Abuild.FileTest.TestFile.cpp"};
        REQUIRE_NOTHROW(abuild::File{testFile.path()});
    }

    SECTION("[non existing]")
    {
        REQUIRE_NOTHROW(abuild::File{{}});
    }
}

TEST_CASE("content() const -> std::string [abuild::File")
{
    SECTION("[existing]")
    {
        std::string content = "int main() {}";
        const abuildtest::TestFile testFile{"Abuild.FileTest.TestFile.cpp", content};
        REQUIRE(abuild::File{testFile.path()}.content() == content);
    }

    SECTION("[empty]")
    {
        const abuildtest::TestFile testFile{"Abuild.FileTest.TestFile.cpp", ""};
        REQUIRE(abuild::File{testFile.path()}.content() == std::string{}); //NOLINT(readability-container-size-empty)
    }

    SECTION("[non existing]")
    {
        REQUIRE(abuild::File{""}.content() == std::string{}); //NOLINT(readability-container-size-empty)
    }
}

TEST_CASE("path() const noexcept -> const std::filesystem::path & [abuild::File]")
{
    const abuildtest::TestFile testFile{"Abuild.FileTest.TestFile.cpp"};
    const abuild::File file{testFile.path()};
    REQUIRE(noexcept(file.path()));
    REQUIRE(file.path() == testFile.path());
}

TEST_CASE("type() -> Type [abuild::File]")
{
    SECTION("[other]")
    {
        REQUIRE(abuild::File::type({}) == abuild::File::Type::Other);
        REQUIRE(abuild::File::type("file.txt") == abuild::File::Type::Other);
        REQUIRE(abuild::File::type("file.docx") == abuild::File::Type::Other);
        REQUIRE(abuild::File::type("cpp") == abuild::File::Type::Other);
        REQUIRE(abuild::File::type(".cpp/") == abuild::File::Type::Other);
        REQUIRE(abuild::File::type(".cpp") == abuild::File::Type::Other);
        REQUIRE(abuild::File::type("source.cpp/") == abuild::File::Type::Other);
    }

    SECTION("[translation unit]")
    {
        REQUIRE(abuild::File::type("source.cpp") == abuild::File::Type::TranslationUnit);
        REQUIRE(abuild::File::type("source.cxx") == abuild::File::Type::TranslationUnit);
        REQUIRE(abuild::File::type("source.cc") == abuild::File::Type::TranslationUnit);
        REQUIRE(abuild::File::type("source.c") == abuild::File::Type::TranslationUnit);
    }

    SECTION("[header]")
    {
        REQUIRE(abuild::File::type("source.hpp") == abuild::File::Type::Header);
        REQUIRE(abuild::File::type("source.hxx") == abuild::File::Type::Header);
        REQUIRE(abuild::File::type("source.h") == abuild::File::Type::Header);
    }
}
}
