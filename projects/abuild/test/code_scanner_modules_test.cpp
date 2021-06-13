import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::CodeScanner (modules)", [] {
    test("module interface", [] {
        TestProjectWithContent testProject{"abuild_code_scanner_test",
                                           {{"mymodule.cpp", "module mymodule;"}}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};
        abuild::CodeScanner{cache};

        assert_(cache.modules().size()).toBe(1u);
        expect(cache.modules()[0]->name).toBe("mymodule");
        expect(cache.modules()[0]->visibility).toBe(abuild::ModuleVisibility::Private);
        expect(cache.modules()[0]->source->path()).toBe(testProject.projectRoot() / "mymodule.cpp");
    });

    test("exported module interface", [] {
        TestProjectWithContent testProject{"abuild_code_scanner_test",
                                           {{"mymodule.cpp", "export module mymodule;"}}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};
        abuild::CodeScanner{cache};

        assert_(cache.modules().size()).toBe(1u);
        expect(cache.modules()[0]->name).toBe("mymodule");
        expect(cache.modules()[0]->visibility).toBe(abuild::ModuleVisibility::Public);
        expect(cache.modules()[0]->source->path()).toBe(testProject.projectRoot() / "mymodule.cpp");
    });

    test("module partition", [] {
        TestProjectWithContent testProject{"abuild_code_scanner_test",
                                           {{"mymodule_partition.cpp", "module mymodule : mypartition;"}}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};
        abuild::CodeScanner{cache};

        assert_(cache.modules().size()).toBe(1u);
        assert_(cache.modules()[0]->partitions.size()).toBe(1u);
        expect(cache.modules()[0]->name).toBe("mymodule");
        expect(cache.modules()[0]->partitions[0]->name).toBe("mypartition");
        expect(cache.modules()[0]->partitions[0]->visibility).toBe(abuild::ModuleVisibility::Private);
        expect(cache.modules()[0]->partitions[0]->source->path()).toBe(testProject.projectRoot() / "mymodule_partition.cpp");
    });

    test("module partitions", [] {
        TestCache testCache;
        TestProjectWithContent testProject{"abuild_code_scanner_test",
                                           {{"mymodule_partition.cpp", "module mymodule : mypartition;"},
                                            {"mymodule_otherpartition.cpp", "export module mymodule : myotherpartition;"}}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};
        abuild::CodeScanner{cache};

        assert_(cache.modules().size()).toBe(1u);
        assert_(cache.modules()[0]->partitions.size()).toBe(2u);
        expect(cache.modules()[0]->name).toBe("mymodule");

        std::vector<abuild::ModulePartition *> &partitions = cache.modules()[0]->partitions;

        std::sort(partitions.begin(), partitions.end(), [](const abuild::ModulePartition *left, const abuild::ModulePartition *right) {
            return left->name < right->name;
        });

        expect(partitions[1]->name).toBe("mypartition");
        expect(partitions[1]->visibility).toBe(abuild::ModuleVisibility::Private);
        expect(partitions[1]->source->path()).toBe(testProject.projectRoot() / "mymodule_partition.cpp");

        expect(partitions[0]->name).toBe("myotherpartition");
        expect(partitions[0]->visibility).toBe(abuild::ModuleVisibility::Public);
        expect(partitions[0]->source->path()).toBe(testProject.projectRoot() / "mymodule_otherpartition.cpp");
    });

    test("module in header", [] {
        TestProjectWithContent testProject{"abuild_code_scanner_test",
                                           {{"header.hpp", "export module mymodule;"}}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};
        abuild::CodeScanner{cache};

        assert_(cache.headers().size()).toBe(1u);
        expect(cache.headers()[0]->dependencies().size()).toBe(0u);
        assert_(cache.warnings().size()).toBe(1u);

        expect(cache.warnings()[0].component).toBe("CodeScanner");
        expect(cache.warnings()[0].what).toBe("Declaring modules in headers is unsupported. Ignoring. (" + (testProject.projectRoot() / "header.hpp").string() + ')');
    });

    test("module partition in header", [] {
        TestProjectWithContent testProject{"abuild_code_scanner_test",
                                           {{"header.hpp", "module mymodule : mypartition;"}}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};
        abuild::CodeScanner{cache};

        assert_(cache.headers().size()).toBe(1u);
        expect(cache.headers()[0]->dependencies().size()).toBe(0u);
        assert_(cache.warnings().size()).toBe(1u);

        expect(cache.warnings()[0].component).toBe("CodeScanner");
        expect(cache.warnings()[0].what).toBe("Declaring module partitions in headers is unsupported. Ignoring. (" + (testProject.projectRoot() / "header.hpp").string() + ')');
    });
});