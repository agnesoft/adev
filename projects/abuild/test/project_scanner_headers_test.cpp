import abuild;
import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::ProjectScanner (headers)", [] {
    test("no header", [] {
        TestCache testCache;
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp"}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(noexcept(cache.headers())).toBe(true);
        assert_(cache.headers().size()).toBe(0u);
    });

    test("single header", [] {
        TestCache testCache;
        TestProject testProject{"build_test_project_scanner",
                                {"header.hpp"}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(cache.headers().size()).toBe(1u);
        assert_(cache.projects().size()).toBe(1u);

        expect(cache.headers()[0].path()).toBe(testProject.projectRoot() / "header.hpp");
        expect(cache.headers()[0].project()).toBe(&cache.projects()[0]);
    });
});
