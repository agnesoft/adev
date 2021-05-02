import abuild;
import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::Projects", [] {
    test("type traits", [] {
        expect(std::is_default_constructible_v<abuild::Projects>).toBe(false);
        expect(std::is_copy_constructible_v<abuild::Projects>).toBe(true);
        expect(std::is_nothrow_move_constructible_v<abuild::Projects>).toBe(true);
        expect(std::is_copy_assignable_v<abuild::Projects>).toBe(false);
        expect(std::is_nothrow_move_assignable_v<abuild::Projects>).toBe(false);
        expect(std::is_nothrow_destructible_v<abuild::Projects>).toBe(true);
    });

    test("root project", [] {
        TestCache testCache;
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp",
                                 "source1.cpp",
                                 "source2.cpp",
                                 "header.hpp"}};

        abuild::BuildCache cache{testCache.file()};
        abuild::DefaultSettings{cache};
        abuild::Settings settings{cache};
        abuild::ProjectScanner{testProject.projectRoot(), cache, settings};
        abuild::Projects projects{cache};

        assert_(asVector(projects.projects()))
            .toBe(std::vector<std::string>{
                "build_test_project_scanner"});

        expect(asVector(projects.projects()["build_test_project_scanner"]["sources"]))
            .toBe(std::vector<std::string>{
                (testProject.projectRoot() / "main.cpp").string(),
                (testProject.projectRoot() / "source1.cpp").string(),
                (testProject.projectRoot() / "source2.cpp").string()});

        expect(asVector(projects.projects()["build_test_project_scanner"]["headers"]))
            .toBe(std::vector<std::string>{
                (testProject.projectRoot() / "header.hpp").string()});
    });

    test("root project with test", [] {
        TestCache testCache;
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp",
                                 "test/main.cpp",
                                 "test/test.cpp"}};

        abuild::BuildCache cache{testCache.file()};
        abuild::DefaultSettings{cache};
        abuild::Settings settings{cache};
        abuild::ProjectScanner{testProject.projectRoot(), cache, settings};
        abuild::Projects projects{cache};

        assert_(asVector(projects.projects()))
            .toBe(std::vector<std::string>{
                "build_test_project_scanner",
                "build_test_project_scanner.test"});

        expect(asVector(projects.projects()["build_test_project_scanner"]["sources"]))
            .toBe(std::vector<std::string>{
                (testProject.projectRoot() / "main.cpp").string()});

        expect(asVector(projects.projects()["build_test_project_scanner"]["headers"]))
            .toBe(std::vector<std::string>{});

        expect(asVector(projects.projects()["build_test_project_scanner.test"]["sources"]))
            .toBe(std::vector<std::string>{
                (testProject.projectRoot() / "test" / "main.cpp").string(),
                (testProject.projectRoot() / "test" / "test.cpp").string()});

        expect(asVector(projects.projects()["build_test_project_scanner.test"]["headers"]))
            .toBe(std::vector<std::string>{});
    });

    test("root project with include", [] {
        TestCache testCache;
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp",
                                 "include/someheader.hpp",
                                 "include/build_test_project_scanner/header.hpp"}};

        abuild::BuildCache cache{testCache.file()};
        abuild::DefaultSettings{cache};
        abuild::Settings settings{cache};
        abuild::ProjectScanner{testProject.projectRoot(), cache, settings};
        abuild::Projects projects{cache};

        assert_(asVector(projects.projects()))
            .toBe(std::vector<std::string>{
                "build_test_project_scanner"});

        expect(asVector(projects.projects()["build_test_project_scanner"]["sources"]))
            .toBe(std::vector<std::string>{
                (testProject.projectRoot() / "main.cpp").string()});

        expect(asVector(projects.projects()["build_test_project_scanner"]["headers"]))
            .toBe(std::vector<std::string>{
                (testProject.projectRoot() / "include" / "build_test_project_scanner" / "header.hpp").string(),
                (testProject.projectRoot() / "include" / "someheader.hpp").string()});
    });

    test("projects under skip directory", [] {
        TestCache testCache;
        TestProject testProject{"build_test_project_scanner",
                                {"projects/my_project/main.cpp",
                                 "projects/my_other_project/src/main.cpp",
                                 "projects/my_other_project/test/main.cpp"}};

        abuild::BuildCache cache{testCache.file()};
        abuild::DefaultSettings{cache};
        abuild::Settings settings{cache};
        abuild::ProjectScanner{testProject.projectRoot(), cache, settings};
        abuild::Projects projects{cache};

        assert_(asVector(projects.projects()))
            .toBe(std::vector<std::string>{
                "my_other_project",
                "my_other_project.test",
                "my_project"});

        expect(asVector(projects.projects()["my_project"]["sources"]))
            .toBe(std::vector<std::string>{
                (testProject.projectRoot() / "projects" / "my_project" / "main.cpp").string()});
        expect(asVector(projects.projects()["my_project"]["headers"]))
            .toBe(std::vector<std::string>{});

        expect(asVector(projects.projects()["my_other_project"]["sources"]))
            .toBe(std::vector<std::string>{
                (testProject.projectRoot() / "projects" / "my_other_project" / "src" / "main.cpp").string()});
        expect(asVector(projects.projects()["my_other_project"]["headers"]))
            .toBe(std::vector<std::string>{});

        expect(asVector(projects.projects()["my_other_project.test"]["sources"]))
            .toBe(std::vector<std::string>{
                (testProject.projectRoot() / "projects" / "my_other_project" / "test" / "main.cpp").string()});
        expect(asVector(projects.projects()["my_other_project.test"]["headers"]))
            .toBe(std::vector<std::string>{});
    });
});
