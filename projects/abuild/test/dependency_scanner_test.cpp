import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::DependencyScanner", [] {
    test("local header", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "#include \"header.hpp\""},
                                            {"header.hpp", ""}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        assert_(cache.sources().size()).toBe(1u);
        assert_(cache.headers().size()).toBe(1u);
        assert_(cache.sources()[0]->dependencies().size()).toBe(1u);

        expect(std::get<abuild::IncludeLocalHeaderDependency>(cache.sources()[0]->dependencies()[0]).header).toBe(cache.headers()[0].get());
    });

    test("local header under subdir", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "#include \"include/header.hpp\""},
                                            {"include/header.hpp", ""}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        assert_(cache.sources().size()).toBe(1u);
        assert_(cache.headers().size()).toBe(1u);
        assert_(cache.sources()[0]->dependencies().size()).toBe(1u);

        expect(std::get<abuild::IncludeLocalHeaderDependency>(cache.sources()[0]->dependencies()[0]).header).toBe(cache.headers()[0].get());
    });

    test("local header not found locally", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"source/main.cpp", "#include \"header.hpp\""},
                                            {"include/header.hpp", ""}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        assert_(cache.sources().size()).toBe(1u);
        assert_(cache.headers().size()).toBe(1u);
        assert_(cache.sources()[0]->dependencies().size()).toBe(1u);

        expect(std::get<abuild::IncludeLocalHeaderDependency>(cache.sources()[0]->dependencies()[0]).header).toBe(cache.headers()[0].get());
    });

    test("local header found locally with other candidates", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"source/main.cpp", "#include \"header.hpp\""},
                                            {"header.hpp", ""},
                                            {"include/header.hpp", ""}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        assert_(cache.sources().size()).toBe(1u);
        assert_(cache.sources()[0]->dependencies().size()).toBe(1u);

        expect(std::get<abuild::IncludeLocalHeaderDependency>(cache.sources()[0]->dependencies()[0]).header->path()).toBe(testProject.projectRoot() / "header.hpp");
    });

    test("local header not found locally with other candidates", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"source/main.cpp", "#include \"header.hpp\""},
                                            {"abc/header.hpp", ""},
                                            {"include/header.hpp", ""}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        assert_(cache.sources().size()).toBe(1u);
        assert_(cache.sources()[0]->dependencies().size()).toBe(1u);

        expect(std::get<abuild::IncludeLocalHeaderDependency>(cache.sources()[0]->dependencies()[0]).header->path()).toBe(testProject.projectRoot() / "abc" / "header.hpp");
    });

    test("external header", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"projects/myproject/main.cpp", "#include <header.hpp>"},
                                            {"projects/someproject/header.hpp", ""}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        assert_(cache.sources().size()).toBe(1u);
        assert_(cache.headers().size()).toBe(1u);
        assert_(cache.sources()[0]->dependencies().size()).toBe(1u);

        expect(std::get<abuild::IncludeExternalHeaderDependency>(cache.sources()[0]->dependencies()[0]).header).toBe(cache.headers()[0].get());
    });

    test("local source", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "#include \"source.cpp\""},
                                            {"source.cpp", ""}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        assert_(cache.source("main.cpp") != nullptr).toBe(true);
        assert_(cache.source("source.cpp") != nullptr).toBe(true);
        assert_(cache.source("main.cpp")->dependencies().size()).toBe(1u);

        expect(std::get<abuild::IncludeLocalSourceDependency>(cache.source("main.cpp")->dependencies()[0]).source).toBe(cache.source("source.cpp"));
    });

    test("import local header", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "import \"include/header.hpp\";"},
                                            {"include/header.hpp", ""}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        assert_(cache.sources().size()).toBe(1u);
        assert_(cache.headers().size()).toBe(1u);
        assert_(cache.sources()[0]->dependencies().size()).toBe(1u);

        expect(std::get<abuild::ImportLocalHeaderDependency>(cache.sources()[0]->dependencies()[0]).header).toBe(cache.headers()[0].get());
    });

    test("import external header", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "import <header.hpp>;"},
                                            {"projects/myproject/include/header.hpp", ""}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        assert_(cache.sources().size()).toBe(1u);
        assert_(cache.headers().size()).toBe(1u);
        assert_(cache.sources()[0]->dependencies().size()).toBe(1u);

        expect(std::get<abuild::ImportExternalHeaderDependency>(cache.sources()[0]->dependencies()[0]).header).toBe(cache.headers()[0].get());
    });

    test("import module", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "import mymodule;"},
                                            {"mymodule.cpp", "export module mymodule;"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        assert_(cache.modules().size()).toBe(1u);
        assert_(cache.source("main.cpp") != nullptr).toBe(true);
        assert_(cache.source("main.cpp")->dependencies().size()).toBe(1u);

        expect(std::get<abuild::ImportModuleDependency>(cache.source("main.cpp")->dependencies()[0]).mod).toBe(cache.modules()[0].get());
    });

    test("import module partition", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"mymodule.cpp", "export module mymodule; import : mypartition;"},
                                            {"mypartition.cpp", "module mymodule : mypartition;"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        assert_(cache.modules().size()).toBe(1u);
        assert_(cache.modules()[0]->partitions.size()).toBe(1u);
        assert_(cache.source("mymodule.cpp") != nullptr).toBe(true);
        assert_(cache.source("mymodule.cpp")->dependencies().size()).toBe(1u);

        expect(std::get<abuild::ImportModulePartitionDependency>(cache.source("mymodule.cpp")->dependencies()[0]).partition).toBe(cache.modules()[0]->partitions[0]);
    });

    test("import module partition from another partition", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"mymodule.cpp", "export module mymodule; import : mypartition;"},
                                            {"mypartition.cpp", "module mymodule : mypartition;\nimport : otherpartition;"},
                                            {"otherpartition.cpp", "module mymodule : otherpartition;"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        assert_(cache.modules().size()).toBe(1u);
        assert_(cache.modules()[0]->partitions.size()).toBe(2u);
        assert_(cache.source("mymodule.cpp") != nullptr).toBe(true);
        assert_(cache.source("mypartition.cpp") != nullptr).toBe(true);
        assert_(cache.source("otherpartition.cpp") != nullptr).toBe(true);
        assert_(cache.source("mymodule.cpp")->dependencies().size()).toBe(1u);
        assert_(cache.source("mypartition.cpp")->dependencies().size()).toBe(1u);

        abuild::ModulePartition *partition1 = cache.cppModulePartition(cache.source("mypartition.cpp"));
        abuild::ModulePartition *partition2 = cache.cppModulePartition(cache.source("otherpartition.cpp"));

        expect(std::get<abuild::ImportModulePartitionDependency>(cache.source("mymodule.cpp")->dependencies()[0]).partition).toBe(partition1);
        expect(std::get<abuild::ImportModulePartitionDependency>(cache.source("mypartition.cpp")->dependencies()[0]).partition).toBe(partition2);
    });

    test("header not found", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "#include \"header.hpp\""}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        assert_(cache.warnings().size()).toBe(1u);
        expect(cache.warnings()[0].component).toBe("DependencyScanner");
        expect(cache.warnings()[0].what).toBe("Header 'header.hpp' not found. (" + (testProject.projectRoot() / "main.cpp").string() + ')');
    });

    test("source not found", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "#include \"source.cpp\""}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        assert_(cache.warnings().size()).toBe(1u);
        expect(cache.warnings()[0].component).toBe("DependencyScanner");
        expect(cache.warnings()[0].what).toBe("Source 'source.cpp' not found. (" + (testProject.projectRoot() / "main.cpp").string() + ')');
    });

    test("module not found", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "import mymodule;"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        assert_(cache.warnings().size()).toBe(1u);
        expect(cache.warnings()[0].component).toBe("DependencyScanner");
        expect(cache.warnings()[0].what).toBe("Module 'mymodule' not found. (" + (testProject.projectRoot() / "main.cpp").string() + ')');
    });

    test("module partition not found", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"mymodule.cpp", "export module mymodule;import : mypartition;"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        assert_(cache.warnings().size()).toBe(1u);
        expect(cache.warnings()[0].component).toBe("DependencyScanner");
        expect(cache.warnings()[0].what).toBe("Module partition 'mypartition' not found in module 'mymodule'. (" + (testProject.projectRoot() / "mymodule.cpp").string() + ')');
    });

    test("module partition not found in a module with different name", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"mymodule.cpp", "export module mymodule;import : mypartition;"},
                                            {"other/mypartition.cpp", "module othermodule : mypartition;"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        assert_(cache.warnings().size()).toBe(1u);
        expect(cache.warnings()[0].component).toBe("DependencyScanner");
        expect(cache.warnings()[0].what).toBe("Module partition 'mypartition' not found in module 'mymodule'. (" + (testProject.projectRoot() / "mymodule.cpp").string() + ')');
    });

    test("module partition not found due to missing module", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "import : mypartition;"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};

        assert_(cache.warnings().size()).toBe(1u);
        expect(cache.warnings()[0].component).toBe("DependencyScanner");
        expect(cache.warnings()[0].what).toBe("Module of module partition 'mypartition' not found. (" + (testProject.projectRoot() / "main.cpp").string() + ')');
    });
});
