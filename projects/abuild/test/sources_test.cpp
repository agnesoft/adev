import abuild;
import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::Projects::sources())", [] {
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

        const std::string source = (testProject.projectRoot() / "main.cpp").string();

        assert_(asVector(projects.sources()))
            .toBe(std::vector<std::string>{
                source});

        expect(projects.sources()[source]["project"].GetString()).toBe(std::string{"build_test_project_scanner"});
        expect(projects.sources()[source]["modified"].GetInt64()).toBe(lastModified(source));
    });

    test("root project with src dir", [] {
        TestCache testCache;
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp",
                                 "src/source.hpp",
                                 "src/build_test_project_scanner/othersource.cpp"}};

        abuild::BuildCache cache{testCache.file()};
        abuild::DefaultSettings{cache};
        abuild::Settings settings{cache};
        abuild::ProjectScanner{testProject.projectRoot(), cache, settings};
        abuild::Projects projects{cache};

        const std::string source1 = (testProject.projectRoot() / "main.cpp").string();
        const std::string source2 = (testProject.projectRoot() / "src" / "build_test_project_scanner" / "othersource.cpp").string();

        assert_(asVector(projects.sources()))
            .toBe(std::vector<std::string>{
                source1,
                source2});

        expect(projects.sources()[source1]["project"].GetString()).toBe(std::string{"build_test_project_scanner"});
        expect(projects.sources()[source1]["modified"].GetInt64()).toBe(lastModified(source1));

        expect(projects.sources()[source2]["project"].GetString()).toBe(std::string{"build_test_project_scanner"});
        expect(projects.sources()[source2]["modified"].GetInt64()).toBe(lastModified(source2));
    });

    test("subprojects with sources", [] {
        TestCache testCache;
        TestProject testProject{"build_test_project_scanner",
                                {"projects/myapp/src/myheader.cpp",
                                 "projects/mylib/mylib.cc",
                                 "projects/mylib/mysublib/projects/mysublib.cxx"}};

        abuild::BuildCache cache{testCache.file()};
        abuild::DefaultSettings{cache};
        abuild::Settings settings{cache};
        abuild::ProjectScanner{testProject.projectRoot(), cache, settings};
        abuild::Projects projects{cache};

        const std::string source1 = (testProject.projectRoot() / "projects" / "myapp" / "src" / "myheader.cpp").string();
        const std::string source2 = (testProject.projectRoot() / "projects" / "mylib" / "mylib.cc").string();
        const std::string source3 = (testProject.projectRoot() / "projects" / "mylib" / "mysublib" / "projects" / "mysublib.cxx").string();

        assert_(asVector(projects.sources()))
            .toBe(std::vector<std::string>{
                source1,
                source2,
                source3});

        expect(projects.sources()[source1]["project"].GetString()).toBe(std::string{"myapp"});
        expect(projects.sources()[source1]["modified"].GetInt64()).toBe(lastModified(source1));

        expect(projects.sources()[source2]["project"].GetString()).toBe(std::string{"mylib"});
        expect(projects.sources()[source2]["modified"].GetInt64()).toBe(lastModified(source2));

        expect(projects.sources()[source3]["project"].GetString()).toBe(std::string{"mylib.mysublib"});
        expect(projects.sources()[source3]["modified"].GetInt64()).toBe(lastModified(source3));
    });
});
