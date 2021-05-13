import abuild;
import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::ProjectScanner", [] {
    test("single project", [] {
        TestProject testProject{"abuild_project_scanner_test",
                                {"main.cpp"}};

        BuildCache cache;
        ProjectScanner{cache};

        assert_(noexcept(cache.projects())).toBe(true);

        const std::vector<Project> &projects = cache.projects();

        assert_(projects.size()).toBe(1);
        expect(projects[0].name()).toBe("abuild_project_scanner_test");
    });
});
