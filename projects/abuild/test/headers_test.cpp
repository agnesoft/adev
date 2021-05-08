import abuild;
import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::Projects::headers()", [] {
    test("root project", [] {
        TestCache testCache;
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp",
                                 "header.hpp"}};

        abuild::BuildCache cache{testCache.file()};
        abuild::DefaultSettings{cache};
        abuild::Settings settings{cache};
        abuild::ProjectScanner{testProject.projectRoot(), cache, settings};
        abuild::Projects projects{cache};

        const std::string header = (testProject.projectRoot() / "header.hpp").string();

        assert_(asVector(projects.headers()))
            .toBe(std::vector<std::string>{
                header});

        expect(projects.headers()[header]["project"].GetString()).toBe(std::string{"build_test_project_scanner"});
        expect(projects.headers()[header]["modified"].GetInt64()).toBe(lastModified(header));
    });

    test("root project with include dir", [] {
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

        const std::string header1 = (testProject.projectRoot() / "include" / "build_test_project_scanner" / "header.hpp").string();
        const std::string header2 = (testProject.projectRoot() / "include" / "someheader.hpp").string();

        assert_(asVector(projects.headers()))
            .toBe(std::vector<std::string>{
                header1,
                header2});

        expect(projects.headers()[header1]["project"].GetString()).toBe(std::string{"build_test_project_scanner"});
        expect(projects.headers()[header1]["modified"].GetInt64()).toBe(lastModified(header1));

        expect(projects.headers()[header2]["project"].GetString()).toBe(std::string{"build_test_project_scanner"});
        expect(projects.headers()[header2]["modified"].GetInt64()).toBe(lastModified(header2));
    });

    test("subprojects with includes", [] {
        TestCache testCache;
        TestProject testProject{"build_test_project_scanner",
                                {"projects/myapp/include/myheader.hpp",
                                 "projects/mylib/include/mylib/mylib.h",
                                 "projects/mylib/mysublib/include/impl/mysublib.hxx"}};

        abuild::BuildCache cache{testCache.file()};
        abuild::DefaultSettings{cache};
        abuild::Settings settings{cache};
        abuild::ProjectScanner{testProject.projectRoot(), cache, settings};
        abuild::Projects projects{cache};

        const std::string header1 = (testProject.projectRoot() / "projects" / "myapp" / "include" / "myheader.hpp").string();
        const std::string header2 = (testProject.projectRoot() / "projects" / "mylib" / "include" / "mylib" / "mylib.h").string();
        const std::string header3 = (testProject.projectRoot() / "projects" / "mylib" / "mysublib" / "include" / "impl" / "mysublib.hxx").string();

        assert_(asVector(projects.headers()))
            .toBe(std::vector<std::string>{
                header1,
                header2,
                header3});

        expect(projects.headers()[header1]["project"].GetString()).toBe(std::string{"myapp"});
        expect(projects.headers()[header1]["modified"].GetInt64()).toBe(lastModified(header1));

        expect(projects.headers()[header2]["project"].GetString()).toBe(std::string{"mylib"});
        expect(projects.headers()[header2]["modified"].GetInt64()).toBe(lastModified(header2));

        expect(projects.headers()[header3]["project"].GetString()).toBe(std::string{"mylib.mysublib"});
        expect(projects.headers()[header3]["modified"].GetInt64()).toBe(lastModified(header3));
    });
});
