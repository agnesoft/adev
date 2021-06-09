#ifdef _MSC_VER
export module abuild : settings;
export import<astl.hpp>;
#endif

namespace abuild
{
export class Settings
{
public:
    [[nodiscard]] auto cppHeaderExtensions() const noexcept -> const std::unordered_set<std::string> &
    {
        return mHeaderExtensions;
    }

    [[nodiscard]] auto cppSourceExtensions() const noexcept -> const std::unordered_set<std::string> &
    {
        return mSourceExtensions;
    }

    [[nodiscard]] auto executableFilenames() const noexcept -> const std::unordered_set<std::string> &
    {
        return mExeFilenames;
    }

    [[nodiscard]] auto ignoreDirectories() const noexcept -> const std::unordered_set<std::string> &
    {
        return mIgnoreDirs;
    }

    [[nodiscard]] auto projectNameSeparator() const noexcept -> const std::string &
    {
        return mSeparator;
    }

    [[nodiscard]] auto skipDirectories() const noexcept -> const std::unordered_set<std::string> &
    {
        return mSkipDirs;
    }

    [[nodiscard]] auto squashDirectories() const noexcept -> const std::unordered_set<std::string> &
    {
        return mSquashDirs;
    }

    [[nodiscard]] auto testDirectories() const noexcept -> const std::unordered_set<std::string> &
    {
        return mTestDirs;
    }

private:
    std::string mSeparator = ".";
    std::unordered_set<std::string> mHeaderExtensions{".hpp", ".hxx", ".h"};
    std::unordered_set<std::string> mSourceExtensions{".cpp", ".cxx", ".cc", ".ixx"};
    std::unordered_set<std::string> mExeFilenames{"main", "Main", "WinMain"};
    std::unordered_set<std::string> mIgnoreDirs{"build"};
    std::unordered_set<std::string> mSkipDirs{"projects", "Projects"};
    std::unordered_set<std::string> mSquashDirs{"src", "srcs", "SRC", "Src", "source", "sources", "Source", "Sources", "include", "Include", "includes", "Includes"};
    std::unordered_set<std::string> mTestDirs{"test", "Test", "tests", "Tests"};
};
}
