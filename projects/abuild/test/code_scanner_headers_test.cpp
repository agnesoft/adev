import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::CodeScanner (headers)", [] {
    test("include STL headers", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"header.hpp", "#include <vector>\n#include <string>"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        assert_(cache.headers().size()).toBe(1u);
        assert_(cache.headers()[0]->dependencies().size()).toBe(2u);

        auto dep1 = std::get<abuild::IncludeSTLHeaderDependency>(cache.headers()[0]->dependencies()[0]);

        expect(dep1.name).toBe("vector");
        expect(dep1.visibility).toBe(abuild::DependencyVisibility::Public);

        auto dep2 = std::get<abuild::IncludeSTLHeaderDependency>(cache.headers()[0]->dependencies()[1]);

        expect(dep2.name).toBe("string");
        expect(dep2.visibility).toBe(abuild::DependencyVisibility::Public);
    });

    test("import STL headers", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"header.hpp", "import <vector>;\nexport import <string>;"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        assert_(cache.headers().size()).toBe(1u);
        assert_(cache.headers()[0]->dependencies().size()).toBe(2u);

        auto dep1 = std::get<abuild::ImportSTLHeaderDependency>(cache.headers()[0]->dependencies()[0]);

        expect(dep1.name).toBe("vector");
        expect(dep1.visibility).toBe(abuild::DependencyVisibility::Private);

        auto dep2 = std::get<abuild::ImportSTLHeaderDependency>(cache.headers()[0]->dependencies()[1]);

        expect(dep2.name).toBe("string");
        expect(dep2.visibility).toBe(abuild::DependencyVisibility::Public);
    });

    test("include local headers", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"header.hpp", "#include \"header.hpp\"\n#include \"other_header.hpp\""}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        assert_(cache.headers().size()).toBe(1u);
        assert_(cache.headers()[0]->dependencies().size()).toBe(2u);

        auto dep1 = std::get<abuild::IncludeLocalHeaderDependency>(cache.headers()[0]->dependencies()[0]);

        expect(dep1.name).toBe("header.hpp");
        expect(dep1.visibility).toBe(abuild::DependencyVisibility::Public);
        expect(dep1.header).toBe(nullptr);

        auto dep2 = std::get<abuild::IncludeLocalHeaderDependency>(cache.headers()[0]->dependencies()[1]);

        expect(dep2.name).toBe("other_header.hpp");
        expect(dep2.visibility).toBe(abuild::DependencyVisibility::Public);
        expect(dep2.header).toBe(nullptr);
    });

    test("include local source", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"header.hpp", "#include \"mysource.cpp\""}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        assert_(cache.headers().size()).toBe(1u);
        assert_(cache.headers()[0]->dependencies().size()).toBe(1u);

        auto dep1 = std::get<abuild::IncludeLocalSourceDependency>(cache.headers()[0]->dependencies()[0]);

        expect(dep1.name).toBe("mysource.cpp");
        expect(dep1.visibility).toBe(abuild::DependencyVisibility::Public);
        expect(dep1.source).toBe(nullptr);
    });

    test("include external headers", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"header.hpp", "#include <myproject/header.hpp>"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        assert_(cache.headers().size()).toBe(1u);
        assert_(cache.headers()[0]->dependencies().size()).toBe(1u);

        auto dep1 = std::get<abuild::IncludeExternalHeaderDependency>(cache.headers()[0]->dependencies()[0]);

        expect(dep1.name).toBe("myproject/header.hpp");
        expect(dep1.visibility).toBe(abuild::DependencyVisibility::Public);
        expect(dep1.header).toBe(nullptr);
    });

    test("include external source", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"header.hpp", "#include <source.cpp>"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        assert_(cache.headers().size()).toBe(1u);
        assert_(cache.headers()[0]->dependencies().size()).toBe(1u);

        auto dep1 = std::get<abuild::IncludeExternalSourceDependency>(cache.headers()[0]->dependencies()[0]);

        expect(dep1.name).toBe("source.cpp");
        expect(dep1.visibility).toBe(abuild::DependencyVisibility::Public);
        expect(dep1.source).toBe(nullptr);
    });

    test("import local headers", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"header.hpp", "import \"header.hpp\";\nexport import \"other_header.hpp\";"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        assert_(cache.headers().size()).toBe(1u);
        assert_(cache.headers()[0]->dependencies().size()).toBe(2u);

        auto dep1 = std::get<abuild::ImportLocalHeaderDependency>(cache.headers()[0]->dependencies()[0]);

        expect(dep1.name).toBe("header.hpp");
        expect(dep1.visibility).toBe(abuild::DependencyVisibility::Private);
        expect(dep1.header).toBe(nullptr);

        auto dep2 = std::get<abuild::ImportLocalHeaderDependency>(cache.headers()[0]->dependencies()[1]);

        expect(dep2.name).toBe("other_header.hpp");
        expect(dep2.visibility).toBe(abuild::DependencyVisibility::Public);
        expect(dep2.header).toBe(nullptr);
    });

    test("import local source", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"header.hpp", "import \"source.cpp\";"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        assert_(cache.headers().size()).toBe(1u);
        expect(cache.headers()[0]->dependencies().size()).toBe(0u);

        assert_(cache.warnings().size()).toBe(1u);
        expect(cache.warnings()[0].component).toBe("CodeScanner");
        expect(cache.warnings()[0].what).toBe("Importing 'source.cpp' (source) is unsupported. Only headers can be imported. Ignoring. (" + (testProject.projectRoot() / "header.hpp").string() + ')');
    });

    test("import external header", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"header.hpp", "export import <myproject/header.hpp>;"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        assert_(cache.headers().size()).toBe(1u);
        assert_(cache.headers()[0]->dependencies().size()).toBe(1u);

        auto dep1 = std::get<abuild::ImportExternalHeaderDependency>(cache.headers()[0]->dependencies()[0]);

        expect(dep1.name).toBe("myproject/header.hpp");
        expect(dep1.visibility).toBe(abuild::DependencyVisibility::Public);
        expect(dep1.header).toBe(nullptr);
    });

    test("import external source", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"header.hpp", "import <source.cpp>;"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        assert_(cache.headers().size()).toBe(1u);
        expect(cache.headers()[0]->dependencies().size()).toBe(0u);

        assert_(cache.warnings().size()).toBe(1u);
        expect(cache.warnings()[0].component).toBe("CodeScanner");
        expect(cache.warnings()[0].what).toBe("Importing 'source.cpp' (source) is unsupported. Only headers can be imported. Ignoring. (" + (testProject.projectRoot() / "header.hpp").string() + ')');
    });

    test("import modules", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"header.hpp", "import mymodule;\nexport import othermodule;"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        assert_(cache.headers().size()).toBe(1u);
        assert_(cache.headers()[0]->dependencies().size()).toBe(2u);

        auto dep1 = std::get<abuild::ImportModuleDependency>(cache.headers()[0]->dependencies()[0]);

        expect(dep1.name).toBe("mymodule");
        expect(dep1.visibility).toBe(abuild::DependencyVisibility::Private);
        expect(dep1.mod).toBe(nullptr);

        auto dep2 = std::get<abuild::ImportModuleDependency>(cache.headers()[0]->dependencies()[1]);

        expect(dep2.name).toBe("othermodule");
        expect(dep2.visibility).toBe(abuild::DependencyVisibility::Public);
        expect(dep2.mod).toBe(nullptr);
    });

    test("import module partition", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"header.hpp", "import : mypartition;\nexport import : otherpartition;"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};

        assert_(cache.headers().size()).toBe(1u);
        expect(cache.headers()[0]->dependencies().size()).toBe(0u);
        assert_(cache.warnings().size()).toBe(2u);

        expect(cache.warnings()[0].component).toBe("CodeScanner");
        expect(cache.warnings()[0].what).toBe("Importing module partitions in headers is unsupported. Ignoring. (" + (testProject.projectRoot() / "header.hpp").string() + ')');

        expect(cache.warnings()[1].component).toBe("CodeScanner");
        expect(cache.warnings()[1].what).toBe("Importing module partitions in headers is unsupported. Ignoring. (" + (testProject.projectRoot() / "header.hpp").string() + ')');
    });
});