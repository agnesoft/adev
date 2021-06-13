import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::ProjectScanner (sources)", [] {
    test("no source", [] {
        TestProject testProject{"abuild_project_scanner_test",
                                {"header.hpp"}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};

        assert_(noexcept(cache.sources())).toBe(true);
        assert_(cache.sources().size()).toBe(0u);
    });

    test("single source", [] {
        TestProject testProject{"abuild_project_scanner_test",
                                {"main.cpp"}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};

        assert_(cache.sources().size()).toBe(1u);
        assert_(cache.projects().size()).toBe(1u);

        expect(cache.sources()[0]->path()).toBe(testProject.projectRoot() / "main.cpp");
        expect(cache.sources()[0]->project()).toBe(cache.projects()[0].get());
    });

    test("multiple sources", [] {
        TestProject testProject{"abuild_project_scanner_test",
                                {"main.cpp",
                                 "source1.cpp",
                                 "source2.cpp"}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};

        std::vector<std::pair<std::filesystem::path, std::string>> sources;

        for (const std::unique_ptr<abuild::Source> &source : cache.sources())
        {
            sources.push_back({source->path(), source->project()->name()});
        }

        std::sort(sources.begin(), sources.end());

        expect(sources)
            .toBe(std::vector<std::pair<std::filesystem::path, std::string>>{
                {testProject.projectRoot() / "main.cpp", "abuild_project_scanner_test"},
                {testProject.projectRoot() / "source1.cpp", "abuild_project_scanner_test"},
                {testProject.projectRoot() / "source2.cpp", "abuild_project_scanner_test"}});
    });

    test("using skip & squash dirs", [] {
        TestProject testProject{"abuild_project_scanner_test",
                                {"projects/abuild/main.cpp",
                                 "projects/acore/src/source1.cpp",
                                 "projects/atest/source2.cpp"}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};

        std::vector<std::pair<std::filesystem::path, std::string>> sources;

        for (const std::unique_ptr<abuild::Source> &source : cache.sources())
        {
            sources.push_back({source->path(), source->project()->name()});
        }

        std::sort(sources.begin(), sources.end());

        expect(sources)
            .toBe(std::vector<std::pair<std::filesystem::path, std::string>>{
                {testProject.projectRoot() / "projects" / "abuild" / "main.cpp", "abuild"},
                {testProject.projectRoot() / "projects" / "acore" / "src" / "source1.cpp", "acore"},
                {testProject.projectRoot() / "projects" / "atest" / "source2.cpp", "atest"}});
    });

    test("multiple sources per project", [] {
        TestProject testProject{"abuild_project_scanner_test",
                                {"projects/abuild/main.cpp",
                                 "projects/abuild/source1.cpp",
                                 "projects/acore/src/source1.cpp",
                                 "projects/acore/src/source2.cpp",
                                 "projects/atest/source1.cpp",
                                 "projects/atest/source2.cpp",
                                 "projects/atest/source3.cpp"}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};

        std::vector<std::pair<std::filesystem::path, std::string>> sources;

        for (const std::unique_ptr<abuild::Source> &source : cache.sources())
        {
            sources.push_back({source->path(), source->project()->name()});
        }

        std::sort(sources.begin(), sources.end());

        expect(sources)
            .toBe(std::vector<std::pair<std::filesystem::path, std::string>>{
                {testProject.projectRoot() / "projects" / "abuild" / "main.cpp", "abuild"},
                {testProject.projectRoot() / "projects" / "abuild" / "source1.cpp", "abuild"},
                {testProject.projectRoot() / "projects" / "acore" / "src" / "source1.cpp", "acore"},
                {testProject.projectRoot() / "projects" / "acore" / "src" / "source2.cpp", "acore"},
                {testProject.projectRoot() / "projects" / "atest" / "source1.cpp", "atest"},
                {testProject.projectRoot() / "projects" / "atest" / "source2.cpp", "atest"},
                {testProject.projectRoot() / "projects" / "atest" / "source3.cpp", "atest"}});
    });
});
