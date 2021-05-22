import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::Source", [] {
    test("type traits", [] {
        expect(std::is_default_constructible_v<abuild::Source>).toBe(false);
        expect(std::is_copy_constructible_v<abuild::Source>).toBe(true);
        expect(std::is_nothrow_move_constructible_v<abuild::Source>).toBe(true);
        expect(std::is_copy_assignable_v<abuild::Source>).toBe(true);
        expect(std::is_nothrow_move_assignable_v<abuild::Source>).toBe(true);
        expect(std::is_nothrow_destructible_v<abuild::Source>).toBe(true);
    });

    test("add source", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp"}};

        abuild::BuildCache cache;
        cache.addSource(testProject.projectRoot() / "main.cpp", "build_test_project_scanner");

        assert_(cache.sources().size()).toBe(1u);
        expect(cache.sources()[0]->path()).toBe(testProject.projectRoot() / "main.cpp");
        expect(cache.sources()[0]->project()->name()).toBe("build_test_project_scanner");
    });

    test("add sources", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp",
                                 "projects/abuild/main.cpp",
                                 "projects/atest/atest.cpp"}};

        abuild::BuildCache cache;
        cache.addSource(testProject.projectRoot() / "main.cpp", "build_test_project_scanner");
        cache.addSource(testProject.projectRoot() / "projects" / "abuild" / "main.cpp", "abuild");
        cache.addSource(testProject.projectRoot() / "projects" / "atest" / "atest.cpp", "atest");

        assert_(cache.sources().size()).toBe(3u);
        expect(cache.sources()[0]->path()).toBe(testProject.projectRoot() / "main.cpp");
        expect(cache.sources()[0]->project()->name()).toBe("build_test_project_scanner");

        expect(cache.sources()[1]->path()).toBe(testProject.projectRoot() / "projects" / "abuild" / "main.cpp");
        expect(cache.sources()[1]->project()->name()).toBe("abuild");

        expect(cache.sources()[2]->path()).toBe(testProject.projectRoot() / "projects" / "atest" / "atest.cpp");
        expect(cache.sources()[2]->project()->name()).toBe("atest");
    });

    test("lookup source", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp",
                                 "projects/abuild/abuild.cpp",
                                 "projects/atest/atest.cpp"}};

        abuild::BuildCache cache;
        cache.addSource(testProject.projectRoot() / "main.cpp", "build_test_project_scanner");
        cache.addSource(testProject.projectRoot() / "projects" / "abuild" / "abuild.cpp", "abuild");
        cache.addSource(testProject.projectRoot() / "projects" / "atest" / "atest.cpp", "atest");

        const abuild::Source *source = cache.source("abuild.cpp");

        assert_(source != nullptr).toBe(true);
        expect(source->path()).toBe(testProject.projectRoot() / "projects" / "abuild" / "abuild.cpp");
        expect(source->project()->name()).toBe("abuild");
    });

    test("lookup source with the same name by name", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp",
                                 "projects/abuild/main.cpp",
                                 "projects/atest/atest.cpp"}};

        abuild::BuildCache cache;
        cache.addSource(testProject.projectRoot() / "main.cpp", "build_test_project_scanner");
        cache.addSource(testProject.projectRoot() / "projects" / "abuild" / "main.cpp", "abuild");
        cache.addSource(testProject.projectRoot() / "projects" / "atest" / "atest.cpp", "atest");

        abuild::Source *source = cache.source("main.cpp");

        assert_(source != nullptr).toBe(true);
        expect(source->path()).toBe(testProject.projectRoot() / "main.cpp");
        expect(source->project()->name()).toBe("build_test_project_scanner");
    });

    test("lookup source with the same name by path", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp",
                                 "projects/abuild/main.cpp",
                                 "projects/atest/atest.cpp"}};

        abuild::BuildCache cache;
        cache.addSource(testProject.projectRoot() / "main.cpp", "build_test_project_scanner");
        cache.addSource(testProject.projectRoot() / "projects" / "abuild" / "main.cpp", "abuild");
        cache.addSource(testProject.projectRoot() / "projects" / "atest" / "atest.cpp", "atest");

        abuild::Source *source = cache.source("abuild/main.cpp");

        assert_(source != nullptr).toBe(true);
        expect(source->path()).toBe(testProject.projectRoot() / "projects" / "abuild" / "main.cpp");
        expect(source->project()->name()).toBe("abuild");
    });

    test("lookup source with the same name by incomplete path", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp",
                                 "projects/abuild/main.cpp",
                                 "projects/atest/atest.cpp"}};

        abuild::BuildCache cache;
        cache.addSource(testProject.projectRoot() / "main.cpp", "build_test_project_scanner");
        cache.addSource(testProject.projectRoot() / "projects" / "abuild" / "main.cpp", "abuild");
        cache.addSource(testProject.projectRoot() / "projects" / "atest" / "atest.cpp", "atest");

        expect(cache.source("build/main.cpp")).toBe(nullptr);
    });

    test("lookup source with hint", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp",
                                 "projects/abuild/main.cpp",
                                 "projects/atest/atest.cpp"}};

        abuild::BuildCache cache;
        cache.addSource(testProject.projectRoot() / "main.cpp", "build_test_project_scanner");
        cache.addSource(testProject.projectRoot() / "projects" / "abuild" / "main.cpp", "abuild");
        cache.addSource(testProject.projectRoot() / "projects" / "atest" / "atest.cpp", "atest");

        abuild::Source *source = cache.source("main.cpp", testProject.projectRoot() / "projects" / "abuild");

        assert_(source != nullptr).toBe(true);
        expect(source->path()).toBe(testProject.projectRoot() / "projects" / "abuild" / "main.cpp");
        expect(source->project()->name()).toBe("abuild");
    });

    test("lookup source with incorrect hint", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp",
                                 "projects/abuild/main.cpp",
                                 "projects/atest/atest.cpp"}};

        abuild::BuildCache cache;
        cache.addSource(testProject.projectRoot() / "main.cpp", "build_test_project_scanner");
        cache.addSource(testProject.projectRoot() / "projects" / "abuild" / "main.cpp", "abuild");
        cache.addSource(testProject.projectRoot() / "projects" / "atest" / "atest.cpp", "atest");

        abuild::Source *source = cache.source("main.cpp", testProject.projectRoot() / "projects" / "atest");

        assert_(source != nullptr).toBe(true);
        expect(source->path()).toBe(testProject.projectRoot() / "main.cpp");
        expect(source->project()->name()).toBe("build_test_project_scanner");
    });
});
