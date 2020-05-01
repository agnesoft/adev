#include "TestFile.hpp"

#include <Catch2/catch.hpp>

#include <ABuild.hpp>

namespace sourcetest
{
TEST_CASE("update() -> bool [abuild::File]")
{
    const std::string originalContent = "int main() {}";
    const abuildtest::TestFile testFile{"Abuild.FileTest.TestFile.cpp", originalContent};
    std::filesystem::last_write_time(testFile.path(), std::filesystem::last_write_time(testFile.path()) - std::chrono::hours{1});

    abuild::Source source{testFile.path()};

    SECTION("[unchanged]")
    {
        REQUIRE_FALSE(source.update());
        REQUIRE_FALSE(source.update());
    }

    SECTION("[changed]")
    {
        {
            std::fstream stream{testFile.path().string(), abuildtest::testFileOpenMode()};
            const std::string newContent = "int main() { return 0; }";
            stream.write(newContent.c_str(), newContent.size());
        }

        REQUIRE(source.update());
        REQUIRE_FALSE(source.update());
    }

    SECTION("[changed, same size]")
    {
        {
            std::fstream stream{testFile.path().string(), abuildtest::testFileOpenMode()};
            const std::string newContent = "void foo() {}";
            stream.write(newContent.c_str(), newContent.size());
        }

        REQUIRE(source.update());
        REQUIRE_FALSE(source.update());
    }

    SECTION("[reverted]")
    {
        {
            std::fstream stream{testFile.path().string(), abuildtest::testFileOpenMode()};
            const std::string newContent = "void foo() {}";
            stream.write(newContent.c_str(), newContent.size());
        }

        {
            std::fstream stream{testFile.path().string(), abuildtest::testFileOpenMode()};
            stream.write(originalContent.c_str(), originalContent.size());
        }

        REQUIRE_FALSE(source.update());
        REQUIRE_FALSE(source.update());
    }

    SECTION("[timestamp updated]")
    {
        std::filesystem::last_write_time(testFile.path(), std::filesystem::last_write_time(testFile.path()) + std::chrono::hours{1});
        REQUIRE_FALSE(source.update());
        REQUIRE_FALSE(source.update());
    }
}
}
