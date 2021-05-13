import abuild;
import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::ProjectScanner (projects)", [] {
    test("root project", [] {
        TestCache testCache;
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp",
                                 "source1.cpp",
                                 "source2.cpp",
                                 "header.hpp"}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(asVector(cache.projects()))
            .toBe(std::vector<std::string>{
                "build_test_project_scanner"});

        expect(asVector(cache.projects()["build_test_project_scanner"]["sources"]))
            .toBe(std::vector<std::string>{
                (testProject.projectRoot() / "main.cpp").string(),
                (testProject.projectRoot() / "source1.cpp").string(),
                (testProject.projectRoot() / "source2.cpp").string()});

        expect(asVector(cache.projects()["build_test_project_scanner"]["headers"]))
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
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(asVector(cache.projects()))
            .toBe(std::vector<std::string>{
                "build_test_project_scanner",
                "build_test_project_scanner.test"});

        expect(asVector(cache.projects()["build_test_project_scanner"]["sources"]))
            .toBe(std::vector<std::string>{
                (testProject.projectRoot() / "main.cpp").string()});

        expect(asVector(cache.projects()["build_test_project_scanner"]["headers"]))
            .toBe(std::vector<std::string>{});

        expect(asVector(cache.projects()["build_test_project_scanner.test"]["sources"]))
            .toBe(std::vector<std::string>{
                (testProject.projectRoot() / "test" / "main.cpp").string(),
                (testProject.projectRoot() / "test" / "test.cpp").string()});

        expect(asVector(cache.projects()["build_test_project_scanner.test"]["headers"]))
            .toBe(std::vector<std::string>{});
    });

    test("root project with include dir", [] {
        TestCache testCache;
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp",
                                 "include/someheader.hpp",
                                 "include/build_test_project_scanner/header.hpp"}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(asVector(cache.projects()))
            .toBe(std::vector<std::string>{
                "build_test_project_scanner"});

        expect(asVector(cache.projects()["build_test_project_scanner"]["sources"]))
            .toBe(std::vector<std::string>{
                (testProject.projectRoot() / "main.cpp").string()});

        expect(asVector(cache.projects()["build_test_project_scanner"]["headers"]))
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
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(asVector(cache.projects()))
            .toBe(std::vector<std::string>{
                "my_other_project",
                "my_other_project.test",
                "my_project"});

        expect(asVector(cache.projects()["my_project"]["sources"]))
            .toBe(std::vector<std::string>{
                (testProject.projectRoot() / "projects" / "my_project" / "main.cpp").string()});
        expect(asVector(cache.projects()["my_project"]["headers"]))
            .toBe(std::vector<std::string>{});

        expect(asVector(cache.projects()["my_other_project"]["sources"]))
            .toBe(std::vector<std::string>{
                (testProject.projectRoot() / "projects" / "my_other_project" / "src" / "main.cpp").string()});
        expect(asVector(cache.projects()["my_other_project"]["headers"]))
            .toBe(std::vector<std::string>{});

        expect(asVector(cache.projects()["my_other_project.test"]["sources"]))
            .toBe(std::vector<std::string>{
                (testProject.projectRoot() / "projects" / "my_other_project" / "test" / "main.cpp").string()});
        expect(asVector(cache.projects()["my_other_project.test"]["headers"]))
            .toBe(std::vector<std::string>{});
    });

    test("project with random subdirs in squash dir", [] {
        TestCache testCache;
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp",
                                 "include/random/header.hpp"}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(asVector(cache.projects()))
            .toBe(std::vector<std::string>{
                "build_test_project_scanner"});

        expect(asVector(cache.projects()["build_test_project_scanner"]["sources"]))
            .toBe(std::vector<std::string>{
                (testProject.projectRoot() / "main.cpp").string()});

        expect(asVector(cache.projects()["build_test_project_scanner"]["headers"]))
            .toBe(std::vector<std::string>{(testProject.projectRoot() / "include" / "random" / "header.hpp").string()});
    });

    test("subprojects", [] {
        TestCache testCache;
        TestProject testProject{"build_test_project_scanner",
                                {"projects/my_project/main.cpp",
                                 "projects/my_project/mysubproject/main.cpp",
                                 "projects/my_project/mysubproject/myfurthersubproject/main.cpp"}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(asVector(cache.projects()))
            .toBe(std::vector<std::string>{
                "my_project",
                "my_project.mysubproject",
                "my_project.mysubproject.myfurthersubproject"});

        expect(asVector(cache.projects()["my_project"]["sources"]))
            .toBe(std::vector<std::string>{
                (testProject.projectRoot() / "projects" / "my_project" / "main.cpp").string()});
        expect(asVector(cache.projects()["my_project"]["headers"]))
            .toBe(std::vector<std::string>{});

        expect(asVector(cache.projects()["my_project.mysubproject"]["sources"]))
            .toBe(std::vector<std::string>{
                (testProject.projectRoot() / "projects" / "my_project" / "mysubproject" / "main.cpp").string()});
        expect(asVector(cache.projects()["my_project.mysubproject"]["headers"]))
            .toBe(std::vector<std::string>{});

        expect(asVector(cache.projects()["my_project.mysubproject.myfurthersubproject"]["sources"]))
            .toBe(std::vector<std::string>{
                (testProject.projectRoot() / "projects" / "my_project" / "mysubproject" / "myfurthersubproject" / "main.cpp").string()});
        expect(asVector(cache.projects()["my_project.mysubproject.myfurthersubproject"]["headers"]))
            .toBe(std::vector<std::string>{});
    });

    test("source files in skip dir", [] {
        TestCache testCache;
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp",
                                 "projects/source1.cpp",
                                 "projects/source2.cpp"}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(asVector(cache.projects()))
            .toBe(std::vector<std::string>{
                "build_test_project_scanner"});

        expect(asVector(cache.projects()["build_test_project_scanner"]["sources"]))
            .toBe(std::vector<std::string>{(testProject.projectRoot() / "main.cpp").string(),
                                           (testProject.projectRoot() / "projects" / "source1.cpp").string(),
                                           (testProject.projectRoot() / "projects" / "source2.cpp").string()});

        expect(asVector(cache.projects()["build_test_project_scanner"]["headers"]))
            .toBe(std::vector<std::string>{});
    });

    test("executable type", [] {
        TestCache testCache;
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp"}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(asVector(cache.projects()))
            .toBe(std::vector<std::string>{
                "build_test_project_scanner"});

        expect(cache.projects()["build_test_project_scanner"]["type"].GetString()).toBe("executable");
    });

    test("test executable type", [] {
        TestCache testCache;
        TestProject testProject{"build_test_project_scanner",
                                {"test/source.cpp"}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(asVector(cache.projects()))
            .toBe(std::vector<std::string>{
                "build_test_project_scanner.test"});

        expect(cache.projects()["build_test_project_scanner.test"]["type"].GetString()).toBe("executable");
    });

    test("library type", [] {
        TestCache testCache;
        TestProject testProject{"build_test_project_scanner",
                                {"source.cpp"}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(asVector(cache.projects()))
            .toBe(std::vector<std::string>{
                "build_test_project_scanner"});

        expect(cache.projects()["build_test_project_scanner"]["type"].GetString()).toBe("library");
    });
});