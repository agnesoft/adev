import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::CodeScanner (sources)", [] {
    test("include STL headers", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "#include <vector>\n#include <string>"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        assert_(cache.sources().size()).toBe(1u);
        assert_(cache.sources()[0]->dependencies().size()).toBe(2u);

        auto dep1 = std::get<abuild::IncludeSTLHeaderDependency>(cache.sources()[0]->dependencies()[0]);

        expect(dep1.name).toBe("vector");
        expect(dep1.visibility).toBe(abuild::DependencyVisibility::Public);

        auto dep2 = std::get<abuild::IncludeSTLHeaderDependency>(cache.sources()[0]->dependencies()[1]);

        expect(dep2.name).toBe("string");
        expect(dep2.visibility).toBe(abuild::DependencyVisibility::Public);
    });

    test("import STL headers", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "import <vector>;\nexport import <string>;"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        assert_(cache.sources().size()).toBe(1u);
        assert_(cache.sources()[0]->dependencies().size()).toBe(2u);

        auto dep1 = std::get<abuild::ImportSTLHeaderDependency>(cache.sources()[0]->dependencies()[0]);

        expect(dep1.name).toBe("vector");
        expect(dep1.visibility).toBe(abuild::DependencyVisibility::Private);

        auto dep2 = std::get<abuild::ImportSTLHeaderDependency>(cache.sources()[0]->dependencies()[1]);

        expect(dep2.name).toBe("string");
        expect(dep2.visibility).toBe(abuild::DependencyVisibility::Public);
    });

    test("include local headers", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "#include \"header.hpp\"\n#include \"other_header.hpp\""}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        assert_(cache.sources().size()).toBe(1u);
        assert_(cache.sources()[0]->dependencies().size()).toBe(2u);

        auto dep1 = std::get<abuild::IncludeLocalHeaderDependency>(cache.sources()[0]->dependencies()[0]);

        expect(dep1.name).toBe("header.hpp");
        expect(dep1.visibility).toBe(abuild::DependencyVisibility::Public);
        expect(dep1.header).toBe(nullptr);

        auto dep2 = std::get<abuild::IncludeLocalHeaderDependency>(cache.sources()[0]->dependencies()[1]);

        expect(dep2.name).toBe("other_header.hpp");
        expect(dep2.visibility).toBe(abuild::DependencyVisibility::Public);
        expect(dep2.header).toBe(nullptr);
    });

    test("include local source", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "#include \"mysource.cpp\""}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        assert_(cache.sources().size()).toBe(1u);
        assert_(cache.sources()[0]->dependencies().size()).toBe(1u);

        auto dep1 = std::get<abuild::IncludeLocalSourceDependency>(cache.sources()[0]->dependencies()[0]);

        expect(dep1.name).toBe("mysource.cpp");
        expect(dep1.visibility).toBe(abuild::DependencyVisibility::Public);
        expect(dep1.source).toBe(nullptr);
    });

    test("include external headers", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "#include <myproject/header.hpp>"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        assert_(cache.sources().size()).toBe(1u);
        assert_(cache.sources()[0]->dependencies().size()).toBe(1u);

        auto dep1 = std::get<abuild::IncludeExternalHeaderDependency>(cache.sources()[0]->dependencies()[0]);

        expect(dep1.name).toBe("myproject/header.hpp");
        expect(dep1.visibility).toBe(abuild::DependencyVisibility::Public);
        expect(dep1.header).toBe(nullptr);
    });

    test("include external source", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "#include <source.cpp>"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        assert_(cache.sources().size()).toBe(1u);
        assert_(cache.sources()[0]->dependencies().size()).toBe(1u);

        auto dep1 = std::get<abuild::IncludeExternalSourceDependency>(cache.sources()[0]->dependencies()[0]);

        expect(dep1.name).toBe("source.cpp");
        expect(dep1.visibility).toBe(abuild::DependencyVisibility::Public);
        expect(dep1.source).toBe(nullptr);
    });

    test("import local headers", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "import \"header.hpp\";\nexport import \"other_header.hpp\";"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        assert_(cache.sources().size()).toBe(1u);
        assert_(cache.sources()[0]->dependencies().size()).toBe(2u);

        auto dep1 = std::get<abuild::ImportLocalHeaderDependency>(cache.sources()[0]->dependencies()[0]);

        expect(dep1.name).toBe("header.hpp");
        expect(dep1.visibility).toBe(abuild::DependencyVisibility::Private);
        expect(dep1.header).toBe(nullptr);

        auto dep2 = std::get<abuild::ImportLocalHeaderDependency>(cache.sources()[0]->dependencies()[1]);

        expect(dep2.name).toBe("other_header.hpp");
        expect(dep2.visibility).toBe(abuild::DependencyVisibility::Public);
        expect(dep2.header).toBe(nullptr);
    });

    test("import local source", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "import \"source.cpp\";"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        assert_(cache.sources().size()).toBe(1u);
        expect(cache.sources()[0]->dependencies().size()).toBe(0u);

        assert_(cache.warnings().size()).toBe(1u);
        expect(cache.warnings()[0].component).toBe("CodeScanner");
        expect(cache.warnings()[0].what).toBe("Importing 'source.cpp' (source) is unsupported. Only headers can be imported. Ignoring. (" + (testProject.projectRoot() / "main.cpp").string() + ')');
    });

    test("import external header", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "export import <myproject/header.hpp>;"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        assert_(cache.sources().size()).toBe(1u);
        assert_(cache.sources()[0]->dependencies().size()).toBe(1u);

        auto dep1 = std::get<abuild::ImportExternalHeaderDependency>(cache.sources()[0]->dependencies()[0]);

        expect(dep1.name).toBe("myproject/header.hpp");
        expect(dep1.visibility).toBe(abuild::DependencyVisibility::Public);
        expect(dep1.header).toBe(nullptr);
    });

    test("import external source", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "import <source.cpp>;"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        assert_(cache.sources().size()).toBe(1u);
        expect(cache.sources()[0]->dependencies().size()).toBe(0u);

        assert_(cache.warnings().size()).toBe(1u);
        expect(cache.warnings()[0].component).toBe("CodeScanner");
        expect(cache.warnings()[0].what).toBe("Importing 'source.cpp' (source) is unsupported. Only headers can be imported. Ignoring. (" + (testProject.projectRoot() / "main.cpp").string() + ')');
    });

    test("import modules", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "import mymodule;\nexport import othermodule;"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        assert_(cache.sources().size()).toBe(1u);
        assert_(cache.sources()[0]->dependencies().size()).toBe(2u);

        auto dep1 = std::get<abuild::ImportModuleDependency>(cache.sources()[0]->dependencies()[0]);

        expect(dep1.name).toBe("mymodule");
        expect(dep1.visibility).toBe(abuild::DependencyVisibility::Private);
        expect(dep1.mod).toBe(nullptr);

        auto dep2 = std::get<abuild::ImportModuleDependency>(cache.sources()[0]->dependencies()[1]);

        expect(dep2.name).toBe("othermodule");
        expect(dep2.visibility).toBe(abuild::DependencyVisibility::Public);
        expect(dep2.mod).toBe(nullptr);
    });

    test("import module partition", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "import : mypartition;\nexport import : otherpartition;"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        assert_(cache.sources().size()).toBe(1u);
        assert_(cache.sources()[0]->dependencies().size()).toBe(2u);

        auto dep1 = std::get<abuild::ImportModulePartitionDependency>(cache.sources()[0]->dependencies()[0]);

        expect(dep1.name).toBe("mypartition");
        expect(dep1.visibility).toBe(abuild::DependencyVisibility::Private);
        expect(dep1.partition).toBe(nullptr);

        auto dep2 = std::get<abuild::ImportModulePartitionDependency>(cache.sources()[0]->dependencies()[1]);

        expect(dep2.name).toBe("otherpartition");
        expect(dep2.visibility).toBe(abuild::DependencyVisibility::Public);
        expect(dep2.partition).toBe(nullptr);
    });
});