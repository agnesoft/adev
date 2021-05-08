import abuild;
import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::Projects::modules()", [] {
    test("module interface", [] {
        TestCache testCache;
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"mymodule.cpp", "module mymodule;"}}};

        abuild::BuildCache cache{testCache.file()};
        abuild::DefaultSettings{cache};
        abuild::Settings settings{cache};
        abuild::ProjectScanner{testProject.projectRoot(), cache, settings};
        abuild::CodeScanner{cache};
        abuild::Projects projects{cache};

        const std::string source = (testProject.projectRoot() / "mymodule.cpp").string();

        assert_(asVector(projects.sources()))
            .toBe(std::vector<std::string>{
                source});

        expect(asVector(projects.sources()[source]["modules"]))
            .toBe(std::vector<std::string>{
                "mymodule"});

        assert_(asVector(projects.modules()))
            .toBe(std::vector<std::string>{
                "mymodule"});

        expect(projects.modules()["mymodule"]["file"].GetString()).toBe(source);
        expect(asVector(projects.modules()["mymodule"]["partitions"])).toBe(std::vector<std::string>{});
        expect(projects.modules()["mymodule"]["exported"].GetBool()).toBe(false);
    });

    test("exported module interface", [] {
        TestCache testCache;
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"mymodule.cpp", "export module mymodule;"}}};

        abuild::BuildCache cache{testCache.file()};
        abuild::DefaultSettings{cache};
        abuild::Settings settings{cache};
        abuild::ProjectScanner{testProject.projectRoot(), cache, settings};
        abuild::CodeScanner{cache};
        abuild::Projects projects{cache};

        const std::string source = (testProject.projectRoot() / "mymodule.cpp").string();

        assert_(asVector(projects.sources()))
            .toBe(std::vector<std::string>{
                source});

        expect(asVector(projects.sources()[source]["modules"]))
            .toBe(std::vector<std::string>{
                "mymodule"});

        assert_(asVector(projects.modules()))
            .toBe(std::vector<std::string>{
                "mymodule"});

        expect(projects.modules()["mymodule"]["file"].GetString()).toBe(source);
        expect(asVector(projects.modules()["mymodule"]["partitions"])).toBe(std::vector<std::string>{});
        expect(projects.modules()["mymodule"]["exported"].GetBool()).toBe(true);
    });

    test("module partition", [] {
        TestCache testCache;
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"mymodule_partition.cpp", "module mymodule : mypartition;"}}};

        abuild::BuildCache cache{testCache.file()};
        abuild::DefaultSettings{cache};
        abuild::Settings settings{cache};
        abuild::ProjectScanner{testProject.projectRoot(), cache, settings};
        abuild::CodeScanner{cache};
        abuild::Projects projects{cache};

        const std::string source = (testProject.projectRoot() / "mymodule_partition.cpp").string();

        assert_(asVector(projects.sources()))
            .toBe(std::vector<std::string>{
                source});

        expect(asVector(projects.sources()[source]["modules"]))
            .toBe(std::vector<std::string>{
                "mymodule"});

        expect(asVector(projects.sources()[source]["modules_partitions"]))
            .toBe(std::vector<std::string>{
                "mypartition"});

        assert_(asVector(projects.modules()))
            .toBe(std::vector<std::string>{
                "mymodule"});

        expect(projects.modules()["mymodule"]["partitions"]["mypartition"].GetString()).toBe(source);
    });

    test("module partitions", [] {
        TestCache testCache;
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"mymodule_partition.cpp", "module mymodule : mypartition;"},
                                            {"mymodule_otherpartition.cpp", "export module mymodule : myotherpartition;"}}};

        abuild::BuildCache cache{testCache.file()};
        abuild::DefaultSettings{cache};
        abuild::Settings settings{cache};
        abuild::ProjectScanner{testProject.projectRoot(), cache, settings};
        abuild::CodeScanner{cache};
        abuild::Projects projects{cache};

        const std::string source1 = (testProject.projectRoot() / "mymodule_partition.cpp").string();
        const std::string source2 = (testProject.projectRoot() / "mymodule_otherpartition.cpp").string();

        assert_(asVector(projects.modules()))
            .toBe(std::vector<std::string>{
                "mymodule"});

        expect(projects.modules()["mymodule"]["partitions"]["mypartition"].GetString()).toBe(source1);
        expect(projects.modules()["mymodule"]["partitions"]["myotherpartition"].GetString()).toBe(source2);
    });
});
