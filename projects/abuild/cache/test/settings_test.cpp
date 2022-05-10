import atest;
import abuild.cache;
import abuild.test_utilities;
import yamlcpp;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("Settings", [] { // NOLINT(cert-err58-cpp)
    test("defaults", [] {
        const ::abuild::TestFile file{"./abuild.cache_test.yaml"};
        const ::abuild::Cache cache{file.path()};

        expect(cache.settings().projectNameSeparator).to_be(".");
        expect(cache.settings().cppHeaderExtensions).to_be(std::unordered_set<std::string>{".hpp", ".hxx", ".h"});
        expect(cache.settings().cppSourceExtensions).to_be(std::unordered_set<std::string>{".cpp", ".cxx", ".cc", ".ixx"});
        expect(cache.settings().executableFilenames).to_be(std::unordered_set<std::string>{"main", "Main", "WinMain"});
        expect(cache.settings().ignoreDirectories).to_be(std::unordered_set<std::string>{"build"});
        expect(cache.settings().skipDirectories).to_be(std::unordered_set<std::string>{"projects", "Projects"});
        expect(cache.settings().squashDirectories).to_be(std::unordered_set<std::string>{"src", "srcs", "SRC", "Src", "source", "sources", "Source", "Sources", "include", "Include", "includes", "Includes"});
        expect(cache.settings().testDirectories).to_be(std::unordered_set<std::string>{"test", "Test", "tests", "Tests"});
    });

    test("set settings", [] {
        const ::abuild::TestFile file{"./abuild.cache_test.yaml"};

        {
            ::abuild::Cache cache{file.path()};
            cache.set_settings(::abuild::Settings{
                .projectNameSeparator = "_",
                .cppHeaderExtensions = {".a",          ".b"  },
                .cppSourceExtensions = {".c",          ".d",   ".e"},
                .executableFilenames = {".out"            },
                .ignoreDirectories = {"dir",         "dir2",              "dir3"},
                .skipDirectories = {".git"            },
                .squashDirectories = {"source"     },
                .testDirectories = {"no_test", "not_a_test"}
            });
        }

        const ::abuild::Cache cache{file.path()};
        expect(cache.settings().projectNameSeparator).to_be("_");
        expect(cache.settings().cppHeaderExtensions).to_be(std::unordered_set<std::string>{".a", ".b"});
        expect(cache.settings().cppSourceExtensions).to_be(std::unordered_set<std::string>{".c", ".d", ".e"});
        expect(cache.settings().executableFilenames).to_be(std::unordered_set<std::string>{".out"});
        expect(cache.settings().ignoreDirectories).to_be(std::unordered_set<std::string>{"dir", "dir2", "dir3"});
        expect(cache.settings().skipDirectories).to_be(std::unordered_set<std::string>{".git"});
        expect(cache.settings().squashDirectories).to_be(std::unordered_set<std::string>{"source"});
        expect(cache.settings().testDirectories).to_be(std::unordered_set<std::string>{"no_test", "not_a_test"});
    });
});
