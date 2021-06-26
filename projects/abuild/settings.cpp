#ifdef _MSC_VER
export module abuild : settings;
export import<astl.hpp>;
#endif

namespace abuild
{
export class Settings
{
public:
    [[nodiscard]] auto clangInstallDirectory() const noexcept -> const std::string &
    {
        return mClangInstallDirectory;
    }

    [[nodiscard]] auto cppHeaderExtensions() const noexcept -> const std::unordered_set<std::string> &
    {
        return mCppHeaderExtensions;
    }

    [[nodiscard]] auto cppSourceExtensions() const noexcept -> const std::unordered_set<std::string> &
    {
        return mCppSourceExtensions;
    }

    [[nodiscard]] auto gccInstallDirectory() const noexcept -> const std::string &
    {
        return mGCCInstallDirectory;
    }

    [[nodiscard]] auto executableFilenames() const noexcept -> const std::unordered_set<std::string> &
    {
        return mExecutableFilenames;
    }

    [[nodiscard]] auto ignoreDirectories() const noexcept -> const std::unordered_set<std::string> &
    {
        return mIgnoreDirectories;
    }

    [[nodiscard]] auto msvcInstallDirectory() const noexcept -> const std::string &
    {
        return mMSVCInstallDirectory;
    }

    [[nodiscard]] auto projectNameSeparator() const noexcept -> const std::string &
    {
        return mProjectNameSeparator;
    }

    auto setClangInstallDirectory(std::string directory) noexcept -> void
    {
        mClangInstallDirectory = std::move(directory);
    }

    auto setCppHeaderExtensions(std::unordered_set<std::string> extensions) noexcept -> void
    {
        mCppHeaderExtensions = std::move(extensions);
    }

    auto setCppSourceExtensions(std::unordered_set<std::string> extensions) noexcept -> void
    {
        mCppSourceExtensions = std::move(extensions);
    }

    auto setGCCInstallDirectory(std::string directory) -> void
    {
        mGCCInstallDirectory = std::move(directory);
    }

    auto setExecutableFilenames(std::unordered_set<std::string> filenames) noexcept -> void
    {
        mExecutableFilenames = std::move(filenames);
    }

    auto setIgnoreDirectories(std::unordered_set<std::string> directories) noexcept -> void
    {
        mIgnoreDirectories = std::move(directories);
    }

    auto setMSVCInstallDirectory(std::string directory) noexcept -> void
    {
        mMSVCInstallDirectory = std::move(directory);
    }

    auto setProjectNameSeparator(std::string separator) noexcept -> void
    {
        mProjectNameSeparator = std::move(separator);
    }

    auto setSkipDirectories(std::unordered_set<std::string> directories) noexcept -> void
    {
        mSkipDirectories = std::move(directories);
    }

    auto setSquashDirectories(std::unordered_set<std::string> directories) noexcept -> void
    {
        mSquashDirectories = std::move(directories);
    }

    auto setTestDirectories(std::unordered_set<std::string> directories) noexcept -> void
    {
        mTestDirectories = std::move(directories);
    }

    [[nodiscard]] auto skipDirectories() const noexcept -> const std::unordered_set<std::string> &
    {
        return mSkipDirectories;
    }

    [[nodiscard]] auto squashDirectories() const noexcept -> const std::unordered_set<std::string> &
    {
        return mSquashDirectories;
    }

    [[nodiscard]] auto testDirectories() const noexcept -> const std::unordered_set<std::string> &
    {
        return mTestDirectories;
    }

private:
    std::string mProjectNameSeparator = ".";
    std::string mGCCInstallDirectory = "/usr";
#ifdef _WIN32
    std::string mClangInstallDirectory = "C:/Program Files/LLVM";
#else
    std::string mClangInstallDirectory = "/usr";
#endif
    std::string mMSVCInstallDirectory = "C:/Program Files (x86)/Microsoft Visual Studio/";
    std::unordered_set<std::string> mCppHeaderExtensions{".hpp", ".hxx", ".h"};
    std::unordered_set<std::string> mCppSourceExtensions{".cpp", ".cxx", ".cc", ".ixx"};
    std::unordered_set<std::string> mExecutableFilenames{"main", "Main", "WinMain"};
    std::unordered_set<std::string> mIgnoreDirectories{"build"};
    std::unordered_set<std::string> mSkipDirectories{"projects", "Projects"};
    std::unordered_set<std::string> mSquashDirectories{"src", "srcs", "SRC", "Src", "source", "sources", "Source", "Sources", "include", "Include", "includes", "Includes"};
    std::unordered_set<std::string> mTestDirectories{"test", "Test", "tests", "Tests"};
};
}
