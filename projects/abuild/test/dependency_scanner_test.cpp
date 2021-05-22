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
});
