import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::ProjectScanner (headers)", [] {
    test("no header", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp"}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(noexcept(cache.headers())).toBe(true);
        assert_(cache.headers().size()).toBe(0u);
    });

    test("single header", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"header.hpp"}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        assert_(cache.headers().size()).toBe(1u);
        assert_(cache.projects().size()).toBe(1u);

        expect(cache.headers()[0]->path()).toBe(testProject.projectRoot() / "header.hpp");
        expect(cache.headers()[0]->project()).toBe(cache.projects()[0].get());
    });

    test("multiple headers", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"header1.hpp",
                                 "header2.hpp",
                                 "header3.hpp"}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        std::vector<std::pair<std::filesystem::path, std::string>> headers;

        for (const std::unique_ptr<abuild::Header> &header : cache.headers())
        {
            headers.push_back({header->path(), header->project()->name()});
        }

        std::sort(headers.begin(), headers.end());

        expect(headers)
            .toBe(std::vector<std::pair<std::filesystem::path, std::string>>{
                {testProject.projectRoot() / "header1.hpp", "build_test_project_scanner"},
                {testProject.projectRoot() / "header2.hpp", "build_test_project_scanner"},
                {testProject.projectRoot() / "header3.hpp", "build_test_project_scanner"}});
    });

    test("using include dirs", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"projects/abuild/header1.hpp",
                                 "projects/acore/include/subdir/header2.hpp",
                                 "projects/atest/include/atest/header3.hpp"}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        std::vector<std::pair<std::filesystem::path, std::string>> headers;

        for (const std::unique_ptr<abuild::Header> &header : cache.headers())
        {
            headers.push_back({header->path(), header->project()->name()});
        }

        std::sort(headers.begin(), headers.end());

        expect(headers)
            .toBe(std::vector<std::pair<std::filesystem::path, std::string>>{
                {testProject.projectRoot() / "projects" / "abuild" / "header1.hpp", "abuild"},
                {testProject.projectRoot() / "projects" / "acore" / "include" / "subdir" / "header2.hpp", "acore"},
                {testProject.projectRoot() / "projects" / "atest" / "include" / "atest" / "header3.hpp", "atest"}});
    });

    test("multiple headers per project", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"projects/abuild/header1.hpp",
                                 "projects/abuild/header2.hpp",
                                 "projects/acore/include/header1.hpp",
                                 "projects/acore/include/header2.hpp",
                                 "projects/atest/header1.hpp",
                                 "projects/atest/header2.hpp",
                                 "projects/atest/header3.hpp"}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};

        std::vector<std::pair<std::filesystem::path, std::string>> headers;

        for (const std::unique_ptr<abuild::Header> &header : cache.headers())
        {
            headers.push_back({header->path(), header->project()->name()});
        }

        std::sort(headers.begin(), headers.end());

        expect(headers)
            .toBe(std::vector<std::pair<std::filesystem::path, std::string>>{
                {testProject.projectRoot() / "projects" / "abuild" / "header1.hpp", "abuild"},
                {testProject.projectRoot() / "projects" / "abuild" / "header2.hpp", "abuild"},
                {testProject.projectRoot() / "projects" / "acore" / "include" / "header1.hpp", "acore"},
                {testProject.projectRoot() / "projects" / "acore" / "include" / "header2.hpp", "acore"},
                {testProject.projectRoot() / "projects" / "atest" / "header1.hpp", "atest"},
                {testProject.projectRoot() / "projects" / "atest" / "header2.hpp", "atest"},
                {testProject.projectRoot() / "projects" / "atest" / "header3.hpp", "atest"}});
    });
});
