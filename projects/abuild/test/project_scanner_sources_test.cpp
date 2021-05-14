import abuild;
import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::ProjectScanner (sources)", [] {
    test("no source", [] {
        TestCache testCache;
        TestProject testProject{"build_test_project_scanner",
                                {"header.hpp"}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(noexcept(cache.sources())).toBe(true);
        assert_(cache.sources().size()).toBe(0u);
    });

    test("single source", [] {
        TestCache testCache;
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp"}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(cache.sources().size()).toBe(1u);
        assert_(cache.projects().size()).toBe(1u);

        expect(cache.sources()[0].path()).toBe(testProject.projectRoot() / "main.cpp");
        expect(cache.sources()[0].project()).toBe(&cache.projects()[0]);
    });

    test("multiple sources", [] {
        TestCache testCache;
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp",
                                 "source1.cpp",
                                 "source2.cpp"}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(cache.sources().size()).toBe(3u);
        assert_(cache.projects().size()).toBe(1u);

        std::vector<std::pair<std::filesystem::path, std::string>> sources;

        for (const abuild::Source &source : cache.sources())
        {
            sources.push_back({source.path(), source.project()->name()});
        }

        std::sort(sources.begin(), sources.end());

        expect(sources)
            .toBe(std::vector<std::pair<std::filesystem::path, std::string>>{
                {testProject.projectRoot() / "main.cpp", "build_test_project_scanner"},
                {testProject.projectRoot() / "source1.cpp", "build_test_project_scanner"},
                {testProject.projectRoot() / "source2.cpp", "build_test_project_scanner"}});
    });
});
