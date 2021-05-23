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
        expect(cache.projects()[0]->name()).toBe("abuild_project_scanner_test");
    });

    test("test project", [] {
        TestProject testProject{"abuild_project_scanner_test",
                                {"test/main.cpp"}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(cache.projects().size()).toBe(1u);
        expect(cache.projects()[0]->name()).toBe("abuild_project_scanner_test.test");
    });

    test("skip directory", [] {
        TestProject testProject{"abuild_project_scanner_test",
                                {"projects/abuild/main.cpp"}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(cache.projects().size()).toBe(1u);
        expect(cache.projects()[0]->name()).toBe("abuild");
    });

    test("skip directory with test", [] {
        TestProject testProject{"abuild_project_scanner_test",
                                {"projects/abuild/test/main.cpp"}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(cache.projects().size()).toBe(1u);
        expect(cache.projects()[0]->name()).toBe("abuild.test");
    });

    test("sub directories under test", [] {
        TestProject testProject{"abuild_project_scanner_test",
                                {"test/main.cpp",
                                 "test/another/dir/main.cpp",
                                 "test/more/test/main.cpp"}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(cache.projects().size()).toBe(1u);
        expect(cache.projects()[0]->name()).toBe("abuild_project_scanner_test.test");
    });

    test("squash directories", [] {
        TestProject testProject{"abuild_project_scanner_test",
                                {"src/main.cpp",
                                 "include/another/dir/main.cpp"}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(cache.projects().size()).toBe(1u);
        expect(cache.projects()[0]->name()).toBe("abuild_project_scanner_test");
    });

    test("squash and skip directories", [] {
        TestProject testProject{"abuild_project_scanner_test",
                                {"projects/abuild/src/main.cpp",
                                 "projects/abuild/include/another/dir/main.cpp",
                                 "projects/abuild/include/abuild/main.cpp"}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(cache.projects().size()).toBe(1u);
        expect(cache.projects()[0]->name()).toBe("abuild");
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

        std::vector<std::string> actualProjects;

        for (const std::unique_ptr<abuild::Project> &project : cache.projects())
        {
            actualProjects.push_back(project->name());
        }

        std::sort(actualProjects.begin(), actualProjects.end());

        expect(actualProjects)
            .toBe(std::vector<std::string>{
                "abuild",
                "abuild.test",
                "acore",
                "acore.test",
                "atest",
                "atest.test"});
    });

    test("project type", [] {
        TestProject testProject{"abuild_project_scanner_test",
                                {"projects/abuild/src/main.cpp",
                                 "projects/abuild/test/src/some_test.cpp",
                                 "projects/acore/include/header.hpp",
                                 "projects/acore/test/other_test.cpp",
                                 "projects/atest/atest.cpp",
                                 "projects/atest/test/src/main.cpp"}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        std::vector<std::pair<std::string, abuild::Project::Type>> actualProjects;

        for (const std::unique_ptr<abuild::Project> &project : cache.projects())
        {
            actualProjects.push_back({project->name(), project->type()});
        }

        std::sort(actualProjects.begin(), actualProjects.end(), [](const std::pair<std::string, abuild::Project::Type> &left, const std::pair<std::string, abuild::Project::Type> &right) {
            return left.first < right.first;
        });

        expect(actualProjects)
            .toBe(std::vector<std::pair<std::string, abuild::Project::Type>>{
                {"abuild", abuild::Project::Type::Executable},
                {"abuild.test", abuild::Project::Type::Executable},
                {"acore", abuild::Project::Type::Library},
                {"acore.test", abuild::Project::Type::Executable},
                {"atest", abuild::Project::Type::Library},
                {"atest.test", abuild::Project::Type::Executable}});
    });
});
