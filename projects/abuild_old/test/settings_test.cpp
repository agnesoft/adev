import abuild;
import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::Settings", [] {
    test("cppSourceExtensions()", [] {
        TestCache testCache;
        abuild::BuildCache cache{testCache.file()};

        expect(asVector(cache.cppSourceExtensions()))
            .toBe(std::vector<std::string>{".cc",
                                           ".cpp",
                                           ".cxx",
                                           ".ixx"});
    });

    test("cppHeaderExtensions()", [] {
        TestCache testCache;
        abuild::BuildCache cache{testCache.file()};

        expect(asVector(cache.cppHeaderExtensions()))
            .toBe(std::vector<std::string>{".h",
                                           ".hpp",
                                           ".hxx"});
    });

    test("ignoreDirectories()", [] {
        TestCache testCache;
        abuild::BuildCache cache{testCache.file()};

        expect(asVector(cache.ignoreDirectories()))
            .toBe(std::vector<std::string>{"build"});
    });

    test("projectNameSeparator()", [] {
        TestCache testCache;
        abuild::BuildCache cache{testCache.file()};

        expect(cache.projectNameSeparator().GetString()).toBe(".");
    });

    test("skipDirectories()", [] {
        TestCache testCache;
        abuild::BuildCache cache{testCache.file()};

        expect(asVector(cache.skipDirectories()))
            .toBe(std::vector<std::string>{"Projects",
                                           "projects"});
    });

    test("squashDirectories()", [] {
        TestCache testCache;
        abuild::BuildCache cache{testCache.file()};

        expect(asVector(cache.squashDirectories()))
            .toBe(std::vector<std::string>{"Include",
                                           "Includes",
                                           "SRC",
                                           "Source",
                                           "Sources",
                                           "Src",
                                           "include",
                                           "includes",
                                           "source",
                                           "sources",
                                           "src",
                                           "srcs"});
    });

    test("testDirectories()", [] {
        TestCache testCache;
        abuild::BuildCache cache{testCache.file()};

        expect(asVector(cache.testDirectories()))
            .toBe(std::vector<std::string>{"Test",
                                           "Tests",
                                           "test",
                                           "tests"});
    });
});
