#ifndef __clang__
export module abuild.cache : settings;
export import astl;
#endif

namespace abuild
{
//! Holds general settings regarding project &
//! code scanners.
export struct Settings
{
    //! Project separator to use for subprojects.
    //! E.g. `my_project/sub1` -> `my_project.sub1`
    std::string projectNameSeparator = ".";

    //! Extensions of files that will be
    //! considered headers and will not be
    //! compiled directly.
    std::unordered_set<std::string> cppHeaderExtensions = {".hpp", ".hxx", ".h"};

    //! Extensions of files that will be
    //! considered sources and that will be
    //! compiled directly.
    std::unordered_set<std::string> cppSourceExtensions = {".cpp", ".cxx", ".cc", ".ixx"};

    //! Names of source files (without extension)
    //! that indicates the containing project is
    //! to be an executable.
    std::unordered_set<std::string> executableFilenames = {"main", "Main", "WinMain"};

    //! Directories to ignore.
    std::unordered_set<std::string> ignoreDirectories = {"build"};

    //! Directories not to consider as project
    //! directories. Does not affect
    //! subdirectories that will be scanned
    //! normally.
    std::unordered_set<std::string> skipDirectories = {"projects", "Projects"};

    //! Directories considered part of the
    //! containing project. E.g. `my_project` ->
    //! `my_project`, `my_project/src` ->
    //! `my_project`.
    std::unordered_set<std::string> squashDirectories = {"src", "srcs", "SRC", "Src", "source", "sources", "Source", "Sources", "include", "Include", "includes", "Includes"};

    //! Directores considered as containing test
    //! project of the containing project.
    std::unordered_set<std::string> testDirectories = {"test", "Test", "tests", "Tests"};
};
}
