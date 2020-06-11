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

#include "TestFile.hpp"

#include <catch2/catch.hpp>

#include <ABuild.hpp>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace sourcetest
{
TEST_CASE("update() -> bool [abuild::File]")
{
    const std::string originalContent = "#include <string>\n"
                                        "#include \"my/included/header.h\""
                                        "\n"
                                        "int main() {}";
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

    SECTION("[includes changed]")
    {
        {
            std::fstream stream{testFile.path().string(), abuildtest::testFileOpenMode()};
            const std::string newContent = "#include \"my/included/header.h\""
                                           "\n"
                                           "int main() {}";
            stream.write(newContent.c_str(), newContent.size());
        }

        REQUIRE(source.update());
        REQUIRE_FALSE(source.update());
    }
}

TEST_CASE("includes() const noexcept -> std::vector<std::string> [abuild::Source]")
{
    SECTION("[no includes]")
    {
        const abuildtest::TestFile testFile{"Abuild.FileTest.TestFile.cpp", "int main() {}"};
        const abuild::Source source{testFile.path()};
        REQUIRE(noexcept(source.includes()));
        REQUIRE(source.includes() == std::vector<std::string>{}); //NOLINT(readability-container-size-empty)
    }

    SECTION("[single include]")
    {
        const abuildtest::TestFile testFile{"Abuild.FileTest.TestFile.cpp",
                                            "#include \"Lib/SomeFile.h\"\n"
                                            "\n"
                                            "int main() {}\n"};
        REQUIRE(abuild::Source{testFile.path()}.includes() == std::vector<std::string>{"Lib/SomeFile.h"});
    }

    SECTION("[multiple includes]")
    {
        const abuildtest::TestFile testFile{"Abuild.FileTest.TestFile.cpp",
                                            "#include \"Lib/SomeFile.h\"\n"
                                            "#include \"SomeInclude.h\"\n"
                                            "\n"
                                            "int main() {}"};
        REQUIRE(abuild::Source{testFile.path()}.includes() == std::vector<std::string>{"Lib/SomeFile.h", "SomeInclude.h"});
    }

    SECTION("[quotes brackets mix]")
    {
        const abuildtest::TestFile testFile{"Abuild.FileTest.TestFile.cpp",
                                            "#include \"Lib/SomeFile.h\"\n"
                                            "#include <string>\n"
                                            "#include \"SomeInclude.h\"\n"
                                            "#include <vector>\n"
                                            "\n"
                                            "int main() {}"};
        REQUIRE(abuild::Source{testFile.path()}.includes() == std::vector<std::string>{"Lib/SomeFile.h", "string", "SomeInclude.h", "vector"});
    }

    SECTION("[leading spaces]")
    {
        const abuildtest::TestFile testFile{"Abuild.FileTest.TestFile.cpp",
                                            "#ifdef X\n"
                                            "  #include \"Lib/SomeFile.h\"\n"
                                            "#endif\n"
                                            "#include \"SomeInclude.h\"\n"
                                            "  #include <cstdint>"
                                            "\n"
                                            "int main() {}"};
        REQUIRE(abuild::Source{testFile.path()}.includes() == std::vector<std::string>{"Lib/SomeFile.h", "SomeInclude.h", "cstdint"});
    }

    SECTION("[trailing spaces]")
    {
        const abuildtest::TestFile testFile{"Abuild.FileTest.TestFile.cpp",
                                            "#ifdef X\n"
                                            "  #include    \"Lib/SomeFile.h\"\n"
                                            "#endif\n"
                                            "#include    \"SomeInclude.h\"\n"
                                            "  #include <cstdint>"
                                            "\n"
                                            "int main() {}"};
        REQUIRE(abuild::Source{testFile.path()}.includes() == std::vector<std::string>{"Lib/SomeFile.h", "SomeInclude.h", "cstdint"});
    }

    SECTION("[tabs]")
    {
        const abuildtest::TestFile testFile{"Abuild.FileTest.TestFile.cpp",
                                            "#ifdef X\n"
                                            "\t#include    \"Lib/SomeFile.h\"\n"
                                            "#endif\n"
                                            "#include\t\t\"SomeInclude.h\"\n"
                                            "\t#include <cstdint>"
                                            "\n"
                                            "int main() {}"};
        REQUIRE(abuild::Source{testFile.path()}.includes() == std::vector<std::string>{"Lib/SomeFile.h", "SomeInclude.h", "cstdint"});
    }

    SECTION("[backslashes]")
    {
        const abuildtest::TestFile testFile{"Abuild.FileTest.TestFile.cpp",
                                            "#ifdef X\n"
                                            "\t#include    \"Lib\\\\SomeFile.h\"\n"
                                            "#endif\n"
                                            "#include\t\t\"SomeInclude.h\"\n"
                                            "\t#include <cstdint>"
                                            "\n"
                                            "int main() {}"};
        REQUIRE(abuild::Source{testFile.path()}.includes() == std::vector<std::string>{"Lib\\\\SomeFile.h", "SomeInclude.h", "cstdint"});
    }

    SECTION("[commented include]")
    {
        const abuildtest::TestFile testFile{"Abuild.FileTest.TestFile.cpp",
                                            "//Some comment at the beginning\n"
                                            "#include \"Lib/SomeFile.h\"\n"
                                            "//#include <string>\n"
                                            "#include <atomic>"
                                            "\n"
                                            "int main() {}"};
        REQUIRE(abuild::Source{testFile.path()}.includes() == std::vector<std::string>{"Lib/SomeFile.h", "atomic"});
    }

    SECTION("[string literal]")
    {
        const abuildtest::TestFile testFile{"Abuild.FileTest.TestFile.cpp",
                                            "//Some comment at the beginning\n"
                                            "#include \"Lib/SomeFile.h\"\n"
                                            "//#include <string>\n"
                                            "#include <atomic>"
                                            "\n"
                                            "int main() { const char *c = \"#include \\\"foo\\\"\" }"};
        REQUIRE(abuild::Source{testFile.path()}.includes() == std::vector<std::string>{"Lib/SomeFile.h", "atomic"});
    }

    SECTION("[division]")
    {
        const abuildtest::TestFile testFile{"Abuild.FileTest.TestFile.cpp",
                                            "//Some comment at the beginning\n"
                                            "#include \"Lib/SomeFile.h\"\n"
                                            "//#include <string>\n"
                                            "#include <atomic>"
                                            "\n"
                                            "int main() { return 4 / 2 }"};
        REQUIRE(abuild::Source{testFile.path()}.includes() == std::vector<std::string>{"Lib/SomeFile.h", "atomic"});
    }

    SECTION("[unrelated comment]")
    {
        const abuildtest::TestFile testFile{"Abuild.FileTest.TestFile.cpp",
                                            "/* Do not use "
                                            "#include \"foo\" "
                                            "in your code */\n"
                                            "\n"
                                            "int main() {}\n"};
        REQUIRE(abuild::Source{testFile.path()}.includes() == std::vector<std::string>{}); //NOLINT(readability-container-size-empty)
    }

    SECTION("[unfinished comment]")
    {
        const abuildtest::TestFile testFile{"Abuild.FileTest.TestFile.cpp",
                                            "/* Do not use "
                                            "#include \"foo\" "
                                            "in your code \n"
                                            "\n"
                                            "int main() {}\n"};
        REQUIRE(abuild::Source{testFile.path()}.includes() == std::vector<std::string>{}); //NOLINT(readability-container-size-empty)
    }

    SECTION("[unfinished include]")
    {
        const abuildtest::TestFile testFile{"Abuild.FileTest.TestFile.cpp",
                                            "#include \"Lib\n"
                                            "\n"
                                            "int main() {}\n"};
        REQUIRE(abuild::Source{testFile.path()}.includes() == std::vector<std::string>{}); //NOLINT(readability-container-size-empty)
    }

    SECTION("[added]")
    {
        const abuildtest::TestFile testFile{"Abuild.FileTest.TestFile.cpp",
                                            "#include \"Lib/SomeFile.h\"\n"
                                            "#include <string>\n"
                                            "#include \"SomeInclude.h\"\n"
                                            "#include <vector>\n"
                                            "\n"
                                            "int main() {}"};
        std::filesystem::last_write_time(testFile.path(), std::filesystem::last_write_time(testFile.path()) - std::chrono::hours{1});

        abuild::Source source{testFile.path()};
        REQUIRE(source.includes() == std::vector<std::string>{"Lib/SomeFile.h", "string", "SomeInclude.h", "vector"});

        {
            std::fstream stream{testFile.path().string(), abuildtest::testFileOpenMode()};
            const std::string newContent = "#include \"Lib/SomeFile.h\"\n"
                                           "#include <string>\n"
                                           "#include \"SomeInclude.h\"\n"
                                           "#include \"my/included/header.h\"\n"
                                           "#include <vector>\n"
                                           "\n"
                                           "int main() {}";
            stream.write(newContent.c_str(), newContent.size());
        }

        REQUIRE(source.update());
        REQUIRE(source.includes() == std::vector<std::string>{"Lib/SomeFile.h", "string", "SomeInclude.h", "my/included/header.h", "vector"});
    }

    SECTION("[removed]")
    {
        const abuildtest::TestFile testFile{"Abuild.FileTest.TestFile.cpp",
                                            "#include \"Lib/SomeFile.h\"\n"
                                            "#include <string>\n"
                                            "#include \"SomeInclude.h\"\n"
                                            "#include <vector>\n"
                                            "\n"
                                            "int main() {}"};
        std::filesystem::last_write_time(testFile.path(), std::filesystem::last_write_time(testFile.path()) - std::chrono::hours{1});

        abuild::Source source{testFile.path()};
        REQUIRE(source.includes() == std::vector<std::string>{"Lib/SomeFile.h", "string", "SomeInclude.h", "vector"});

        {
            std::fstream stream{testFile.path().string(), abuildtest::testFileOpenMode()};
            const std::string newContent = "#include \"Lib/SomeFile.h\"\n"
                                           "#include <string>\n"
                                           "//#include <vector>\n"
                                           "\n"
                                           "int main() {}";
            stream.write(newContent.c_str(), newContent.size());
        }

        REQUIRE(source.update());
        REQUIRE(source.includes() == std::vector<std::string>{"Lib/SomeFile.h", "string"});
    }
}
}
