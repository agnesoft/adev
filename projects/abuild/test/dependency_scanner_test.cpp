import abuild;
import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::DependencyScanner (sources)", [] {
    test("include external headers (STL)", [] {
        TestCache testCache;
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "#include <vector>\n#include <string>"}}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        const std::string source = (testProject.projectRoot() / "main.cpp").string();

        assert_(asVector(cache.sources()[source]["includes_external"]))
            .toBe(std::vector<std::string>{
                "string",
                "vector"});

        expect(cache.sources()[source]["includes_external"]["vector"].GetString()).toBe("STL");
        expect(cache.sources()[source]["includes_external"]["string"].GetString()).toBe("STL");
    });

    test("include external header", [] {
        TestCache testCache;
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "#include <myheader.hpp>\n#include <sublib/myheader.hpp>"},
                                            {"projects/mylib/include/myheader.hpp", ""},
                                            {"projects/mylib/include/sublib/myheader.hpp", ""}}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        const std::string source = (testProject.projectRoot() / "main.cpp").string();
        const std::string header1 = (testProject.projectRoot() / "projects" / "mylib" / "include" / "myheader.hpp").string();
        const std::string header2 = (testProject.projectRoot() / "projects" / "mylib" / "include" / "sublib" / "myheader.hpp").string();

        assert_(asVector(cache.sources()[source]["includes_external"]))
            .toBe(std::vector<std::string>{"myheader.hpp", "sublib/myheader.hpp"});

        expect(cache.sources()[source]["includes_external"]["myheader.hpp"].GetString()).toBe(header1);
        expect(cache.sources()[source]["includes_external"]["sublib/myheader.hpp"].GetString()).toBe(header2);
    });

    test("include local header", [] {
        TestCache testCache;
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "#include \"myheader.hpp\"\n#include \"include/myheader.hpp\""},
                                            {"myheader.hpp", ""},
                                            {"include/myheader.hpp", ""},
                                            {"projects/a/myheader.hpp", ""},
                                            {"projects/b/include/myheader.hpp", ""}}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        const std::string source = (testProject.projectRoot() / "main.cpp").string();
        const std::string header1 = (testProject.projectRoot() / "myheader.hpp").string();
        const std::string header2 = (testProject.projectRoot() / "include" / "myheader.hpp").string();

        assert_(asVector(cache.sources()[source]["includes_local"]))
            .toBe(std::vector<std::string>{"include/myheader.hpp", "myheader.hpp"});

        expect(cache.sources()[source]["includes_local"]["myheader.hpp"].GetString()).toBe(header1);
        expect(cache.sources()[source]["includes_local"]["include/myheader.hpp"].GetString()).toBe(header2);
    });

    test("import local header", [] {
        TestCache testCache;
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "import \"myheader.hpp\";\nimport \"include/myheader.hpp\";"},
                                            {"myheader.hpp", ""},
                                            {"include/myheader.hpp", ""},
                                            {"projects/a/myheader.hpp", ""},
                                            {"projects/b/include/myheader.hpp", ""}}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        const std::string source = (testProject.projectRoot() / "main.cpp").string();
        const std::string header1 = (testProject.projectRoot() / "myheader.hpp").string();
        const std::string header2 = (testProject.projectRoot() / "include" / "myheader.hpp").string();

        assert_(asVector(cache.sources()[source]["import_includes_local"]))
            .toBe(std::vector<std::string>{"include/myheader.hpp", "myheader.hpp"});

        expect(cache.sources()[source]["import_includes_local"]["myheader.hpp"].GetString()).toBe(header1);
        expect(cache.sources()[source]["import_includes_local"]["include/myheader.hpp"].GetString()).toBe(header2);
    });

    test("import external headers (STL)", [] {
        TestCache testCache;
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "import <vector>;\nimport <string>;"}}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        const std::string source = (testProject.projectRoot() / "main.cpp").string();

        assert_(asVector(cache.sources()[source]["import_includes_external"]))
            .toBe(std::vector<std::string>{
                "string",
                "vector"});

        expect(cache.sources()[source]["import_includes_external"]["vector"].GetString()).toBe("STL");
        expect(cache.sources()[source]["import_includes_external"]["string"].GetString()).toBe("STL");
    });

    test("import external header", [] {
        TestCache testCache;
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "import <myheader.hpp>;\nimport <sublib/myheader.hpp>;"},
                                            {"projects/mylib/include/myheader.hpp", ""},
                                            {"projects/mylib/include/sublib/myheader.hpp", ""}}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        const std::string source = (testProject.projectRoot() / "main.cpp").string();
        const std::string header1 = (testProject.projectRoot() / "projects" / "mylib" / "include" / "myheader.hpp").string();
        const std::string header2 = (testProject.projectRoot() / "projects" / "mylib" / "include" / "sublib" / "myheader.hpp").string();

        assert_(asVector(cache.sources()[source]["import_includes_external"]))
            .toBe(std::vector<std::string>{"myheader.hpp", "sublib/myheader.hpp"});

        expect(cache.sources()[source]["import_includes_external"]["myheader.hpp"].GetString()).toBe(header1);
        expect(cache.sources()[source]["import_includes_external"]["sublib/myheader.hpp"].GetString()).toBe(header2);
    });

    test("import module", [] {
        TestCache testCache;
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "import mymodule;\nimport missing_module;"},
                                            {"mymodule.cpp", "export module mymodule;"}}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        const std::string source = (testProject.projectRoot() / "main.cpp").string();
        const std::string mod = (testProject.projectRoot() / "mymodule.cpp").string();

        assert_(asVector(cache.sources()[source]["import_modules"]))
            .toBe(std::vector<std::string>{"missing_module", "mymodule"});

        expect(cache.sources()[source]["import_modules"]["mymodule"].GetString()).toBe(mod);
        expect(cache.sources()[source]["import_modules"]["missing_module"].GetString()).toBe(std::string{});
    });

    test("import module partition", [] {
        TestCache testCache;
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"mymodule.cpp", "export module mymodule;import : mypartition;import : missing_partition;"},
                                            {"mypartition.cpp", "module mymodule : mypartition;"}}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        const std::string mod = (testProject.projectRoot() / "mymodule.cpp").string();
        const std::string partition = (testProject.projectRoot() / "mypartition.cpp").string();

        assert_(asVector(cache.sources()[mod]["import_module_partitions"]))
            .toBe(std::vector<std::string>{"missing_partition", "mypartition"});

        expect(cache.sources()[mod]["import_module_partitions"]["mypartition"].GetString()).toBe(partition);
        expect(cache.sources()[mod]["import_module_partitions"]["missing_partition"].GetString()).toBe(std::string{});
    });

    test("import module partition without module", [] {
        TestCache testCache;
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"mymodule.cpp", "import : mypartition;import : missing_partition;"},
                                            {"mypartition.cpp", "module mymodule : mypartition;"}}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        const std::string mod = (testProject.projectRoot() / "mymodule.cpp").string();
        const std::string partition = (testProject.projectRoot() / "mypartition.cpp").string();

        assert_(asVector(cache.sources()[mod]["import_module_partitions"]))
            .toBe(std::vector<std::string>{"missing_partition", "mypartition"});

        expect(cache.sources()[mod]["import_module_partitions"]["mypartition"].GetString()).toBe(std::string{});
        expect(cache.sources()[mod]["import_module_partitions"]["missing_partition"].GetString()).toBe(std::string{});
    });
});
