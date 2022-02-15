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
        expect(cache.settings().cppHeaderExtensions).to_be(std::vector<std::string>{".hpp", ".hxx", ".h"});
        expect(cache.settings().cppSourceExtensions).to_be(std::vector<std::string>{".cpp", ".cxx", ".cc", ".ixx"});
        expect(cache.settings().executableFilenames).to_be(std::vector<std::string>{"main", "Main", "WinMain"});
        expect(cache.settings().ignoreDirectories).to_be(std::vector<std::string>{"build"});
        expect(cache.settings().skipDirectories).to_be(std::vector<std::string>{"projects", "Projects"});
        expect(cache.settings().squashDirectories).to_be(std::vector<std::string>{"src", "srcs", "SRC", "Src", "source", "sources", "Source", "Sources", "include", "Include", "includes", "Includes"});
        expect(cache.settings().testDirectories).to_be(std::vector<std::string>{"test", "Test", "tests", "Tests"});
    });
});
