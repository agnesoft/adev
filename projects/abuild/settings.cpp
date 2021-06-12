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

    auto setCppHeaderExtensions(std::unordered_set<std::string> extensions) noexcept -> void
    {
        mHeaderExtensions = std::move(extensions);
    }

    auto setCppSourceExtensions(std::unordered_set<std::string> extensions) noexcept -> void
    {
        mSourceExtensions = std::move(extensions);
    }

    auto setExecutableFilenames(std::unordered_set<std::string> filenames) noexcept -> void
    {
        mExeFilenames = std::move(filenames);
    }

    auto setIgnoreDirectories(std::unordered_set<std::string> directories) noexcept -> void
    {
        mIgnoreDirs = std::move(directories);
    }

    auto setProjectNameSeparator(std::string separator) noexcept -> void
    {
        mSeparator = std::move(separator);
    }

    auto setSkipDirectories(std::unordered_set<std::string> directories) noexcept -> void
    {
        mSkipDirs = std::move(directories);
    }

    auto setSquashDirectories(std::unordered_set<std::string> directories) noexcept -> void
    {
        mSquashDirs = std::move(directories);
    }

    auto setTestDirectories(std::unordered_set<std::string> directories) noexcept -> void
    {
        mTestDirs = std::move(directories);
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
