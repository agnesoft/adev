import abuild;
import abuild_test_cache;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::Settings", [] {
    test("type traits", [] {
        expect(std::is_default_constructible_v<abuild::Settings>).toBe(false);
        expect(std::is_copy_constructible_v<abuild::Settings>).toBe(true);
        expect(std::is_nothrow_move_constructible_v<abuild::Settings>).toBe(true);
        expect(std::is_copy_assignable_v<abuild::Settings>).toBe(false);
        expect(std::is_nothrow_move_assignable_v<abuild::Settings>).toBe(false);
        expect(std::is_nothrow_destructible_v<abuild::Settings>).toBe(true);
    });

    test("cppSourceExtensions()", [] {
        TestCache testCache;
        abuild::BuildCache cache{testCache.file()};
        abuild::DefaultSettings{cache};
        abuild::Settings settings{cache};

        expect(asVector(settings.cppSourceExtensions()))
            .toBe(std::vector<std::string>{".cpp",
                                           ".cxx",
                                           ".cc",
                                           ".ixx"});
    });

    test("cppHeaderExtensions()", [] {
        TestCache testCache;
        abuild::BuildCache cache{testCache.file()};
        abuild::DefaultSettings{cache};
        abuild::Settings settings{cache};

        expect(asVector(settings.cppHeaderExtensions()))
            .toBe(std::vector<std::string>{".hpp",
                                           ".hxx",
                                           ".h"});
    });

    test("ignoreDirectories()", [] {
        TestCache testCache;
        abuild::BuildCache cache{testCache.file()};
        abuild::DefaultSettings{cache};
        abuild::Settings settings{cache};

        expect(asVector(settings.ignoreDirectories()))
            .toBe(std::vector<std::string>{"build"});
    });

    test("projectNameSeparator()", [] {
        TestCache testCache;
        abuild::BuildCache cache{testCache.file()};
        abuild::DefaultSettings{cache};
        abuild::Settings settings{cache};

        expect(settings.projectNameSeparator().GetString()).toBe(".");
    });

    test("skipDirectories()", [] {
        TestCache testCache;
        abuild::BuildCache cache{testCache.file()};
        abuild::DefaultSettings{cache};
        abuild::Settings settings{cache};

        expect(asVector(settings.skipDirectories()))
            .toBe(std::vector<std::string>{"projects",
                                           "Projects"});
    });

    test("squashDirectories()", [] {
        TestCache testCache;
        abuild::BuildCache cache{testCache.file()};
        abuild::DefaultSettings{cache};
        abuild::Settings settings{cache};

        expect(asVector(settings.squashDirectories()))
            .toBe(std::vector<std::string>{"src",
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
                                           "Includes"});
    });

    test("testDirectories()", [] {
        TestCache testCache;
        abuild::BuildCache cache{testCache.file()};
        abuild::DefaultSettings{cache};
        abuild::Settings settings{cache};

        expect(asVector(settings.testDirectories()))
            .toBe(std::vector<std::string>{"test",
                                           "Test",
                                           "tests",
                                           "Tests"});
    });
});
