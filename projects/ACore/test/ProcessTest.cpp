#include "pch.hpp"

#include <catch2/catch.hpp>
#include <filesystem>

namespace processtest
{
static constexpr const char *TEST_DIR = "acore.processtest.testdir";

class TestDir
{
public:
    TestDir() :
        mDir(std::filesystem::current_path().append(TEST_DIR).string())
    {
        removeDir();
        std::filesystem::create_directory(mDir);
    }

    TestDir(const TestDir &other) = delete;
    TestDir(TestDir &&other) = delete;

    ~TestDir() noexcept
    {
        try
        {
            try
            {
                removeDir();
            }
            catch(...)
            {
                INFO("Unable to remove dir processtest::TestDir::~TestDir()");
            }
        }
        catch(...)
        {
            std::terminate();
        }
    }

    [[nodiscard]] const std::string &testDir() const noexcept
    {
        return mDir;
    }

    TestDir &operator=(const TestDir &other) = delete;
    TestDir &operator=(TestDir &&other) = delete;

private:
    void removeDir()
    {
        if(std::filesystem::exists(TEST_DIR))
        {
            std::filesystem::remove(TEST_DIR);
        }
    }

    std::string mDir;
};

TEST_CASE("[acore::Process]")
{
    REQUIRE_FALSE(std::is_default_constructible_v<acore::Process>);
    REQUIRE(std::is_copy_constructible_v<acore::Process>);
    REQUIRE(std::is_nothrow_move_constructible_v<acore::Process>);
    REQUIRE(std::is_copy_assignable_v<acore::Process>);
    REQUIRE(std::is_nothrow_move_assignable_v<acore::Process>);
    REQUIRE(std::is_nothrow_destructible_v<acore::Process>);
}

TEST_CASE("Process(std::string process) [acore::Process]")
{
#ifdef _WIN32
    std::string command = "cmd /C echo Hello World";
#else
    std::string command = "echo Hello World";
#endif
    REQUIRE_NOTHROW(acore::Process{std::move(command)});
}

TEST_CASE("Process(std::string process, std::string workingDirectory) [acore::Process]")
{
    const TestDir testDir;
#ifdef _WIN32
    std::string command = "cmd /C echo Hello World";
#else
    std::string command = "echo Hello World";
#endif
    std::string workingDirectory = testDir.testDir();
    REQUIRE_NOTHROW(acore::Process{std::move(command), std::move(workingDirectory)});
}

TEST_CASE("command() const noexcept -> const std::string & [acore::Process]")
{
#ifdef _WIN32
    const acore::Process process{"cmd /C echo Hello World"};
    REQUIRE(noexcept(process.command()));
    REQUIRE(process.command() == "cmd /C echo Hello World");
#else
    const acore::Process process{"echo Hello World"};
    REQUIRE(process.command() == "echo Hello World");
#endif
}

TEST_CASE("exitCode() const noexcept -> int [acore::Process]")
{
    SECTION("[success]")
    {
#ifdef _WIN32
        const acore::Process process{"cmd /C echo Hello World"};
#else
        const acore::Process process{"echo Hello World"};
#endif
        REQUIRE(noexcept(process.exitCode()));
        REQUIRE(process.exitCode() == 0);
    }

    SECTION("[failure]")
    {
#ifdef _WIN32
        const acore::Process process{"cmd /C exit -1"};
#else
        const acore::Process process{"exit -1"};
#endif

        REQUIRE(process.exitCode() == -1);
    }
}

TEST_CASE("standardError() const noexcept -> const std::string & [acore::Process]")
{
#ifdef _WIN32
    const acore::Process process{"cmd /C >&2 echo error"};
    REQUIRE(noexcept(process.standardError()));
    REQUIRE(process.standardError() == "error\r\n");
#else
    const acore::Process process{">&2 echo error"};
    REQUIRE(noexcept(process.standardError()));
    REQUIRE(process.standardError() == "error\n");
#endif
}

TEST_CASE("standardOutput() const noexcept -> const std::string & [acore::Process]")
{
#ifdef _WIN32
    const acore::Process process{"cmd /C echo Hello World"};
    REQUIRE(noexcept(process.standardOutput()));
    REQUIRE(process.standardOutput() == "Hello World\r\n");
#else
    const acore::Process process{"echo Hello World"};
    REQUIRE(noexcept(process.standardOutput()));
    REQUIRE(process.standardOutput() == "Hello World\n");
#endif
}

TEST_CASE("workingDirectory() const noexcept -> const std::string & [acore::Process]")
{
    SECTION("[default]")
    {
#ifdef _WIN32
        const acore::Process process{"cmd /C echo Hello World"};
#else
        const acore::Process process{"echo Hello World"};
#endif
        REQUIRE(noexcept(process.workingDirectory()));
        REQUIRE(process.workingDirectory() == std::filesystem::current_path().string());
    }

    SECTION("[custom]")
    {
        const TestDir testDir;
#ifdef _WIN32
        const acore::Process process{"cmd /C echo Hello World", testDir.testDir()};
#else
        const acore::Process process{"echo Hello World", testDir.testDir()};
#endif
        REQUIRE(process.workingDirectory() == testDir.testDir());
    }
}
}
