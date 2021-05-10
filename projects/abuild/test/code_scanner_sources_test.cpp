import abuild;
import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::CodeScanner (sources)", [] {
    test("include external headers", [] {
        TestCache testCache;
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "#include <vector>\n#include <string>"}}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        const std::string source = (testProject.projectRoot() / "main.cpp").string();

        assert_(asVector(cache.sources()))
            .toBe(std::vector<std::string>{
                source});

        expect(asVector(cache.sources()[source]["includes_external"]))
            .toBe(std::vector<std::string>{
                "string",
                "vector"});
    });

    test("include local headers", [] {
        TestCache testCache;
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "#include \"header.hpp\"\n#include \"other_header.hpp\""}}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        const std::string source = (testProject.projectRoot() / "main.cpp").string();

        assert_(asVector(cache.sources()))
            .toBe(std::vector<std::string>{
                source});

        expect(asVector(cache.sources()[source]["includes_local"]))
            .toBe(std::vector<std::string>{
                "header.hpp",
                "other_header.hpp"});
    });

    test("import local headers", [] {
        TestCache testCache;
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "import \"header.hpp\";\nexport import \"other_header.hpp\";"}}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        const std::string source = (testProject.projectRoot() / "main.cpp").string();

        assert_(asVector(cache.sources()))
            .toBe(std::vector<std::string>{
                source});

        expect(asVector(cache.sources()[source]["import_includes_local"]))
            .toBe(std::vector<std::string>{
                "header.hpp",
                "other_header.hpp"});
    });

    test("import external header", [] {
        TestCache testCache;
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "import <vector>;\nexport import <string>;"}}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        const std::string source = (testProject.projectRoot() / "main.cpp").string();

        assert_(asVector(cache.sources()))
            .toBe(std::vector<std::string>{
                source});

        expect(asVector(cache.sources()[source]["import_includes_external"]))
            .toBe(std::vector<std::string>{
                "string",
                "vector"});
    });

    test("import modules", [] {
        TestCache testCache;
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "import mymodule;\nexport import othermodule;"}}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        const std::string source = (testProject.projectRoot() / "main.cpp").string();

        assert_(asVector(cache.sources()))
            .toBe(std::vector<std::string>{
                source});

        expect(asVector(cache.sources()[source]["import_modules"]))
            .toBe(std::vector<std::string>{
                "mymodule",
                "othermodule"});
    });

    test("import module partition", [] {
        TestCache testCache;
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "import : mymodule;\nexport import : othermodule;"}}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        const std::string source = (testProject.projectRoot() / "main.cpp").string();

        assert_(asVector(cache.sources()))
            .toBe(std::vector<std::string>{
                source});

        expect(asVector(cache.sources()[source]["import_module_partitions"]))
            .toBe(std::vector<std::string>{
                "mymodule",
                "othermodule"});
    });

    test("module interface", [] {
        TestCache testCache;
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"mymodule.cpp", "module mymodule;"}}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        const std::string source = (testProject.projectRoot() / "mymodule.cpp").string();

        assert_(asVector(cache.sources()))
            .toBe(std::vector<std::string>{
                source});

        expect(asVector(cache.sources()[source]["modules"]))
            .toBe(std::vector<std::string>{
                "mymodule"});

        assert_(asVector(cache.modules()))
            .toBe(std::vector<std::string>{
                "mymodule"});

        expect(cache.modules()["mymodule"]["file"].GetString()).toBe(source);
        expect(asVector(cache.modules()["mymodule"]["partitions"])).toBe(std::vector<std::string>{});
        expect(cache.modules()["mymodule"]["exported"].GetBool()).toBe(false);
    });

    test("exported module interface", [] {
        TestCache testCache;
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"mymodule.cpp", "export module mymodule;"}}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        const std::string source = (testProject.projectRoot() / "mymodule.cpp").string();

        assert_(asVector(cache.sources()))
            .toBe(std::vector<std::string>{
                source});

        expect(asVector(cache.sources()[source]["modules"]))
            .toBe(std::vector<std::string>{
                "mymodule"});

        assert_(asVector(cache.modules()))
            .toBe(std::vector<std::string>{
                "mymodule"});

        expect(cache.modules()["mymodule"]["file"].GetString()).toBe(source);
        expect(asVector(cache.modules()["mymodule"]["partitions"])).toBe(std::vector<std::string>{});
        expect(cache.modules()["mymodule"]["exported"].GetBool()).toBe(true);
    });

    test("module partition", [] {
        TestCache testCache;
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"mymodule_partition.cpp", "module mymodule : mypartition;"}}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        const std::string source = (testProject.projectRoot() / "mymodule_partition.cpp").string();

        assert_(asVector(cache.sources()))
            .toBe(std::vector<std::string>{
                source});

        expect(asVector(cache.sources()[source]["modules"]))
            .toBe(std::vector<std::string>{
                "mymodule"});

        expect(asVector(cache.sources()[source]["modules_partitions"]))
            .toBe(std::vector<std::string>{
                "mypartition"});

        assert_(asVector(cache.modules()))
            .toBe(std::vector<std::string>{
                "mymodule"});

        expect(cache.modules()["mymodule"]["partitions"]["mypartition"].GetString()).toBe(source);
    });

    test("module partitions", [] {
        TestCache testCache;
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"mymodule_partition.cpp", "module mymodule : mypartition;"},
                                            {"mymodule_otherpartition.cpp", "export module mymodule : myotherpartition;"}}};

        abuild::BuildCache cache{testCache.file()};
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        const std::string source1 = (testProject.projectRoot() / "mymodule_partition.cpp").string();
        const std::string source2 = (testProject.projectRoot() / "mymodule_otherpartition.cpp").string();

        assert_(asVector(cache.modules()))
            .toBe(std::vector<std::string>{
                "mymodule"});

        expect(cache.modules()["mymodule"]["partitions"]["mypartition"].GetString()).toBe(source1);
        expect(cache.modules()["mymodule"]["partitions"]["myotherpartition"].GetString()).toBe(source2);
    });
});