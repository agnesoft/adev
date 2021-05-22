import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::Header", [] {
    test("type traits", [] {
        expect(std::is_default_constructible_v<abuild::Header>).toBe(false);
        expect(std::is_copy_constructible_v<abuild::Header>).toBe(true);
        expect(std::is_nothrow_move_constructible_v<abuild::Header>).toBe(true);
        expect(std::is_copy_assignable_v<abuild::Header>).toBe(true);
        expect(std::is_nothrow_move_assignable_v<abuild::Header>).toBe(true);
        expect(std::is_nothrow_destructible_v<abuild::Header>).toBe(true);
    });

    test("add header", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"header.hpp"}};

        abuild::BuildCache cache;
        cache.addHeader(testProject.projectRoot() / "header.hpp", "build_test_project_scanner");

        assert_(cache.headers().size()).toBe(1u);
        expect(cache.headers()[0]->path()).toBe(testProject.projectRoot() / "header.hpp");
        expect(cache.headers()[0]->project()->name()).toBe("build_test_project_scanner");
    });

    test("add headers", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"header.hpp",
                                 "projects/abuild/include/abuild/header.hpp",
                                 "projects/atest/include/atest.hpp"}};

        abuild::BuildCache cache;
        cache.addHeader(testProject.projectRoot() / "header.hpp", "build_test_project_scanner");
        cache.addHeader(testProject.projectRoot() / "projects" / "abuild" / "include" / "abuild" / "header.hpp", "abuild");
        cache.addHeader(testProject.projectRoot() / "projects" / "atest" / "include" / "atest.hpp", "atest");

        assert_(cache.headers().size()).toBe(3u);
        expect(cache.headers()[0]->path()).toBe(testProject.projectRoot() / "header.hpp");
        expect(cache.headers()[0]->project()->name()).toBe("build_test_project_scanner");

        expect(cache.headers()[1]->path()).toBe(testProject.projectRoot() / "projects" / "abuild" / "include" / "abuild" / "header.hpp");
        expect(cache.headers()[1]->project()->name()).toBe("abuild");

        expect(cache.headers()[2]->path()).toBe(testProject.projectRoot() / "projects" / "atest" / "include" / "atest.hpp");
        expect(cache.headers()[2]->project()->name()).toBe("atest");
    });

    test("lookup header", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"header.hpp",
                                 "projects/abuild/include/abuild/header.hpp",
                                 "projects/atest/include/atest.hpp"}};

        abuild::BuildCache cache;
        cache.addHeader(testProject.projectRoot() / "header.hpp", "build_test_project_scanner");
        cache.addHeader(testProject.projectRoot() / "projects" / "abuild" / "include" / "abuild" / "header.hpp", "abuild");
        cache.addHeader(testProject.projectRoot() / "projects" / "atest" / "include" / "atest.hpp", "atest");

        const abuild::Header *header = cache.header("atest.hpp");

        assert_(header != nullptr).toBe(true);
        expect(header->path()).toBe(testProject.projectRoot() / "projects" / "atest" / "include" / "atest.hpp");
        expect(header->project()->name()).toBe("atest");
    });

    test("lookup header with the same name by name", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"header.hpp",
                                 "projects/abuild/include/abuild/header.hpp",
                                 "projects/atest/include/atest.hpp"}};

        abuild::BuildCache cache;
        cache.addHeader(testProject.projectRoot() / "header.hpp", "build_test_project_scanner");
        cache.addHeader(testProject.projectRoot() / "projects" / "abuild" / "include" / "abuild" / "header.hpp", "abuild");
        cache.addHeader(testProject.projectRoot() / "projects" / "atest" / "include" / "atest.hpp", "atest");

        const abuild::Header *header = cache.header("header.hpp");

        assert_(header != nullptr).toBe(true);
        expect(header->path()).toBe(testProject.projectRoot() / "header.hpp");
        expect(header->project()->name()).toBe("build_test_project_scanner");
    });

    test("lookup header with the same name by path", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"header.hpp",
                                 "projects/abuild/include/abuild/header.hpp",
                                 "projects/atest/include/atest.hpp"}};

        abuild::BuildCache cache;
        cache.addHeader(testProject.projectRoot() / "header.hpp", "build_test_project_scanner");
        cache.addHeader(testProject.projectRoot() / "projects" / "abuild" / "include" / "abuild" / "header.hpp", "abuild");
        cache.addHeader(testProject.projectRoot() / "projects" / "atest" / "include" / "atest.hpp", "atest");

        const abuild::Header *header = cache.header("abuild/header.hpp");

        assert_(header != nullptr).toBe(true);
        expect(header->path()).toBe(testProject.projectRoot() / "projects" / "abuild" / "include" / "abuild" / "header.hpp");
        expect(header->project()->name()).toBe("abuild");
    });

    test("lookup header with the same name by incomplete path", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"header.hpp",
                                 "projects/abuild/include/abuild/header.hpp",
                                 "projects/atest/include/atest.hpp"}};

        abuild::BuildCache cache;
        cache.addHeader(testProject.projectRoot() / "header.hpp", "build_test_project_scanner");
        cache.addHeader(testProject.projectRoot() / "projects" / "abuild" / "include" / "abuild" / "header.hpp", "abuild");
        cache.addHeader(testProject.projectRoot() / "projects" / "atest" / "include" / "atest.hpp", "atest");

        expect(cache.header("build/header.hpp")).toBe(nullptr);
    });

    test("lookup header with hint", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"include/header.hpp",
                                 "projects/abuild/include/header.hpp",
                                 "projects/atest/include/atest.hpp"}};

        abuild::BuildCache cache;
        cache.addHeader(testProject.projectRoot() / "include" / "header.hpp", "build_test_project_scanner");
        cache.addHeader(testProject.projectRoot() / "projects" / "abuild" / "include" / "header.hpp", "abuild");
        cache.addHeader(testProject.projectRoot() / "projects" / "atest" / "include" / "atest.hpp", "atest");

        const abuild::Header *header = cache.header("include/header.hpp", testProject.projectRoot() / "projects" / "abuild");

        assert_(header != nullptr).toBe(true);
        expect(header->path()).toBe(testProject.projectRoot() / "projects" / "abuild" / "include" / "header.hpp");
        expect(header->project()->name()).toBe("abuild");
    });

    test("lookup header with incorrect hint", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"include/header.hpp",
                                 "projects/abuild/include/header.hpp",
                                 "projects/atest/include/atest.hpp"}};

        abuild::BuildCache cache;
        cache.addHeader(testProject.projectRoot() / "include" / "header.hpp", "build_test_project_scanner");
        cache.addHeader(testProject.projectRoot() / "projects" / "abuild" / "include" / "header.hpp", "abuild");
        cache.addHeader(testProject.projectRoot() / "projects" / "atest" / "include" / "atest.hpp", "atest");

        const abuild::Header *header = cache.header("include/header.hpp", testProject.projectRoot() / "projects" / "atest");

        assert_(header != nullptr).toBe(true);
        expect(header->path()).toBe(testProject.projectRoot() / "include" / "header.hpp");
        expect(header->project()->name()).toBe("build_test_project_scanner");
    });
});
