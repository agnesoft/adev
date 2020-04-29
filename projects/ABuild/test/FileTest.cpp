#include <Catch2/catch.hpp>

#include <ABuild.hpp>
#include <fstream>

namespace abuildfiletest
{
class TestFile
{
public:
    explicit TestFile(std::string name) :
        mFile{name}
    {
        removeFile();
    }

    TestFile(std::string name, const std::string &content) :
        mFile{std::move(name)}
    {
        std::fstream stream{mFile.string(), std::ios::in | std::ios::out | std::ios::trunc};
        stream.write(content.data(), content.size());
    }

    ~TestFile()
    {
        removeFile();
    }

    [[nodiscard]] const std::filesystem::path &path() const noexcept
    {
        return mFile;
    }

private:
    void removeFile() const
    {
        if (std::filesystem::exists(mFile))
        {
            std::filesystem::remove(mFile);
        }
    }

    std::filesystem::path mFile;
};

TEST_CASE("File(std::filesystem::path path) [abuild::File]")
{
    SECTION("[existing]")
    {
        const TestFile testFile{"Abuild.FileTest.TestFile.cpp"};
        REQUIRE_NOTHROW(abuild::File{testFile.path()});
    }

    SECTION("[non-existing]")
    {
        REQUIRE_NOTHROW(abuild::File{{}});
    }
}

TEST_CASE("path() const noexcept -> const std::filesystem::path & [abuild::File]")
{
    const TestFile testFile{"Abuild.FileTest.TestFile.cpp"};
    const abuild::File file{testFile.path()};
    REQUIRE(noexcept(file.path()));
    REQUIRE(file.path() == testFile.path());
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

TEST_CASE("isChanged() const -> bool [abuild::File]")
{
    const std::string originalContent = "int main() {}";
    const TestFile testFile{"Abuild.FileTest.TestFile.cpp", originalContent};
    abuild::File file{testFile.path()};

    SECTION("[unchanged]")
    {
        REQUIRE_FALSE(file.isChanged());
    }

    SECTION("[changed]")
    {
        {
            std::fstream stream{testFile.path().string(), std::ios::in | std::ios::out | std::ios::trunc};
            const std::string newContent = "int main() { return 0; }";
            stream.write(newContent.c_str(), newContent.size());
        }

        REQUIRE(file.isChanged());
    }

    SECTION("[changed, same size]")
    {
        {
            std::fstream stream{testFile.path().string(), std::ios::in | std::ios::out | std::ios::trunc};
            const std::string newContent = "void foo() {}";
            stream.write(newContent.c_str(), newContent.size());
        }

        REQUIRE(file.isChanged());
    }

    SECTION("[reverted]")
    {
        {
            std::fstream stream{testFile.path().string(), std::ios::in | std::ios::out | std::ios::trunc};
            const std::string newContent = "void foo() {}";
            stream.write(newContent.c_str(), newContent.size());
        }

        {
            std::fstream stream{testFile.path().string(), std::ios::in | std::ios::out | std::ios::trunc};
            stream.write(originalContent.c_str(), originalContent.size());
        }

        REQUIRE_FALSE(file.isChanged());
    }

    SECTION("[timestamp updated]")
    {
        std::filesystem::path path{testFile.path()};
        auto fileTime = std::filesystem::last_write_time(path);
        fileTime += std::chrono::hours{1};
        std::filesystem::last_write_time(path, fileTime);

        REQUIRE_FALSE(file.isChanged());
    }
}
}
