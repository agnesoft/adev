import abuild;
import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::ProjectScanner (projects)", [] {
    test("no sources project", [] {
        TestProject testProject{"abuild_project_scanner_test",
                                {"main"}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(noexcept(cache.projects())).toBe(true);
        expect(cache.projects().size()).toBe(0u);
    });

    test("single project", [] {
        TestProject testProject{"abuild_project_scanner_test",
                                {"main.cpp"}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(cache.projects().size()).toBe(1u);
        expect(cache.projects()[0].name()).toBe("abuild_project_scanner_test");
    });

    test("test project", [] {
        TestProject testProject{"abuild_project_scanner_test",
                                {"test/main.cpp"}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(cache.projects().size()).toBe(1u);
        expect(cache.projects()[0].name()).toBe("abuild_project_scanner_test.test");
    });

    test("skip directory", [] {
        TestProject testProject{"abuild_project_scanner_test",
                                {"projects/abuild/main.cpp"}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(cache.projects().size()).toBe(1u);
        expect(cache.projects()[0].name()).toBe("abuild");
    });

    test("skip directory with test", [] {
        TestProject testProject{"abuild_project_scanner_test",
                                {"projects/abuild/test/main.cpp"}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(cache.projects().size()).toBe(1u);
        expect(cache.projects()[0].name()).toBe("abuild.test");
    });

    test("sub directories under test", [] {
        TestProject testProject{"abuild_project_scanner_test",
                                {"test/main.cpp",
                                 "test/another/dir/main.cpp",
                                 "test/more/test/main.cpp"}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(cache.projects().size()).toBe(1u);
        expect(cache.projects()[0].name()).toBe("abuild_project_scanner_test.test");
    });

    test("squash directories", [] {
        TestProject testProject{"abuild_project_scanner_test",
                                {"src/main.cpp",
                                 "include/another/dir/main.cpp"}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(cache.projects().size()).toBe(1u);
        expect(cache.projects()[0].name()).toBe("abuild_project_scanner_test");
    });

    test("squash and skip directories", [] {
        TestProject testProject{"abuild_project_scanner_test",
                                {"projects/abuild/src/main.cpp",
                                 "projects/abuild/include/another/dir/main.cpp",
                                 "projects/abuild/include/abuild/main.cpp"}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(cache.projects().size()).toBe(1u);
        expect(cache.projects()[0].name()).toBe("abuild");
    });

    test("multiple projects", [] {
        TestProject testProject{"abuild_project_scanner_test",
                                {"projects/abuild/src/main.cpp",
                                 "projects/abuild/test/src/main.cpp",
                                 "projects/acore/include/header.hpp",
                                 "projects/acore/test/main.cpp",
                                 "projects/atest/atest.cpp",
                                 "projects/atest/test/main.cpp"}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(cache.projects().size()).toBe(6u);
        expect(cache.projects()[0].name()).toBe("abuild");
        expect(cache.projects()[1].name()).toBe("abuild.test");
        expect(cache.projects()[2].name()).toBe("acore");
        expect(cache.projects()[3].name()).toBe("acore.test");
        expect(cache.projects()[4].name()).toBe("atest");
        expect(cache.projects()[5].name()).toBe("atest.test");
    });
});
