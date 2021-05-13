#ifdef _MSC_VER
export module abuild : settings;
import<astl.hpp>;
#endif

namespace abuild
{
export class Settings
{
public:
    [[nodiscard]] static auto cppHeaderExtensions() -> const std::unordered_set<std::string> &
    {
        static const std::unordered_set<std::string> extensions{
            ".hpp",
            ".hxx",
            ".h"};

        return extensions;
    }

    [[nodiscard]] static auto cppSourceExtensions() -> const std::unordered_set<std::string> &
    {
        static const std::unordered_set<std::string> extensions{
            ".cpp",
            ".cxx",
            ".cc",
            ".ixx"};

        return extensions;
    }

    [[nodiscard]] static auto ignoreDirectories() -> const std::unordered_set<std::string> &
    {
        static const std::unordered_set<std::string> dirs{
            "build"};

        return dirs;
    }

    [[nodiscard]] static auto projectNameSeparator() -> const std::string &
    {
        static const std::string separator = ".";
        return separator;
    }

    [[nodiscard]] static auto skipDirectories() -> const std::unordered_set<std::string> &
    {
        static const std::unordered_set<std::string> dirs{
            "projects",
            "Projects"};

        return dirs;
    }

    [[nodiscard]] static auto squashDirectories() -> const std::unordered_set<std::string> &
    {
        static const std::unordered_set<std::string> dirs{
            "src",
            "srcs",
            "SRC",
            "Src",
            "source",
            "sources",
            "Source",
            "Sources",
            "include",
            "Include",
            "includes",
            "Includes"};

        return dirs;
    }

    [[nodiscard]] static auto testDirectories() -> const std::unordered_set<std::string> &
    {
        static const std::unordered_set<std::string> dirs{
            "test",
            "Test",
            "tests",
            "Tests"};

        return dirs;
    }

private:
    std::string mName;
};
}
