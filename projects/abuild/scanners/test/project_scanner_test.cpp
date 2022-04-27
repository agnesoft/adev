import atest;
import abuild.scanners;
import abuild.test_utilities;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("ProjectScanner", [] { // NOLINT(cert-err58-cpp, cppcoreguidelines-interfaces-global-init)
    test("single file project", [] {
        const ::abuild::TestProject testProject{
            "project_scanner_test",
            {{"main.cpp", ""}}};

        ::abuild::Cache cache{testProject.root() / "abuild.scanners_test.yaml"};
        ::abuild::ProjectScanner{cache}.scan();
        ::abuild::Project *project = cache.project("project_scanner_test");

        assert_(project).not_to_be(nullptr);
        expect(project->name).to_be("project_scanner_test");
        assert_(project->sources.size()).to_be(1U);
        expect(project->sources[0]->path).to_be("project_scanner_test/main.cpp");
    });

    test("skip dirs", [] {
        const ::abuild::TestProject testProject{
            "project_scanner_test",
            {{"projects/myapp/main.cpp", ""},
              {"projects/mylib/my_lib.cpp", ""}}
        };

        ::abuild::Cache cache{testProject.root() / "abuild.scanners_test.yaml"};
        ::abuild::ProjectScanner{cache}.scan();

        ::abuild::Project *project = cache.project("myapp");

        assert_(project).not_to_be(nullptr);
        expect(project->name).to_be("myapp");
        assert_(project->sources.size()).to_be(1U);
        expect(project->sources[0]->path).to_be("project_scanner_test/projects/myapp/main.cpp");

        project = cache.project("mylib");

        assert_(project).not_to_be(nullptr);
        expect(project->name).to_be("mylib");
        assert_(project->sources.size()).to_be(1U);
        expect(project->sources[0]->path).to_be("project_scanner_test/projects/mylib/my_lib.cpp");
    });

    test("squash dirs", [] {
        const ::abuild::TestProject testProject{
            "project_scanner_test",
            {{"myapp/src/main.cpp", ""},
              {"myapp/src/src/source.cpp", ""},
              {"myapp/source.cpp", ""}}
        };

        ::abuild::Cache cache{testProject.root() / "abuild.scanners_test.yaml"};
        ::abuild::ProjectScanner{cache}.scan();

        ::abuild::Project *project = cache.project("myapp");

        assert_(project).not_to_be(nullptr);
        expect(project->name).to_be("myapp");
        assert_(project->sources.size()).to_be(3U);

        std::vector<std::filesystem::path> sourcePaths;
        sourcePaths.reserve(3);

        for (const auto &source : project->sources)
        {
            sourcePaths.push_back(source->path);
        }

        std::sort(sourcePaths.begin(), sourcePaths.end());

        expect(sourcePaths[0]).to_be("project_scanner_test/myapp/source.cpp");
        expect(sourcePaths[1]).to_be("project_scanner_test/myapp/src/main.cpp");
        expect(sourcePaths[2]).to_be("project_scanner_test/myapp/src/src/source.cpp");
    });

    test("sub project", [] {
        const ::abuild::TestProject testProject{
            "project_scanner_test",
            {{"myapp/main.cpp", ""},
              {"myapp/subapp/main.cpp", ""}}
        };

        ::abuild::Cache cache{testProject.root() / "abuild.scanners_test.yaml"};
        ::abuild::ProjectScanner{cache}.scan();

        ::abuild::Project *project = cache.project("myapp");

        assert_(project).not_to_be(nullptr);
        expect(project->name).to_be("myapp");
        assert_(project->sources.size()).to_be(1U);
        expect(project->sources[0]->path).to_be("project_scanner_test/myapp/main.cpp");

        project = cache.project("myapp.subapp");
        assert_(project).not_to_be(nullptr);
        expect(project->name).to_be("myapp.subapp");
        assert_(project->sources.size()).to_be(1U);
        expect(project->sources[0]->path).to_be("project_scanner_test/myapp/subapp/main.cpp");
    });

    test("ignore dirs", [] {
        const ::abuild::TestProject testProject{
            "project_scanner_test",
            {{"build/myapp/main.cpp", ""},
              {".somedir/header.hpp", ""},
              {"someapp/.hidden/source.cpp", ""}}
        };

        ::abuild::Cache cache{testProject.root() / "abuild.scanners_test.yaml"};
        ::abuild::ProjectScanner{cache}.scan();

        expect(cache.projects().size()).to_be(0U);
    });

    test("test project", [] {
        const ::abuild::TestProject testProject{
            "project_scanner_test",
            {{"myapp/main.cpp", ""},
              {"myapp/test/main.cpp", ""}}
        };

        ::abuild::Cache cache{testProject.root() / "abuild.scanners_test.yaml"};
        ::abuild::ProjectScanner{cache}.scan();

        ::abuild::Project *project = cache.project("myapp");

        assert_(project).not_to_be(nullptr);
        expect(project->name).to_be("myapp");
        assert_(project->sources.size()).to_be(1U);
        expect(project->sources[0]->path).to_be("project_scanner_test/myapp/main.cpp");

        project = cache.project("myapp.test");
        assert_(project).not_to_be(nullptr);
        expect(project->name).to_be("myapp.test");
        assert_(project->sources.size()).to_be(1U);
        expect(project->sources[0]->path).to_be("project_scanner_test/myapp/test/main.cpp");
    });

    test("headers", [] {
        const ::abuild::TestProject testProject{
            "project_scanner_test",
            {{"mylib/mylib.cpp", ""},
              {"mylib/include/some_header.hpp", ""},
              {"mylib/include/mylib/some_other_header.hpp", ""}}
        };

        ::abuild::Cache cache{testProject.root() / "abuild.scanners_test.yaml"};
        ::abuild::ProjectScanner{cache}.scan();

        ::abuild::Project *project = cache.project("mylib");

        assert_(project).not_to_be(nullptr);
        expect(project->name).to_be("mylib");
        assert_(project->headers.size()).to_be(2U);

        std::vector<std::filesystem::path> headerPaths;
        headerPaths.reserve(2U);

        for (const auto &header : project->headers)
        {
            headerPaths.push_back(header->path);
        }

        std::sort(headerPaths.begin(), headerPaths.end());

        expect(headerPaths[0]).to_be("project_scanner_test/mylib/include/mylib/some_other_header.hpp");
        expect(headerPaths[1]).to_be("project_scanner_test/mylib/include/some_header.hpp");
    });

    test("tokens in source", [] {
        const ::abuild::TestProject testProject{
            "project_scanner_test",
            {{"mylib/mylib.cpp", "export module mylib;\nimport other.module;"}}};

        ::abuild::Cache cache{testProject.root() / "abuild.scanners_test.yaml"};
        ::abuild::ProjectScanner{cache}.scan();

        ::abuild::Project *project = cache.project("mylib");

        assert_(project).not_to_be(nullptr);
        expect(project->name).to_be("mylib");
        assert_(project->sources.size()).to_be(1U);
        expect(project->sources[0]->tokens).to_be(std::vector<::abuild::Token>{
            ::abuild::ModuleToken{.name = "mylib", .exported = true},
            ::abuild::ImportModuleToken{.name = "other.module"              }
        });
    });

    test("tokens in header", [] {
        const ::abuild::TestProject testProject{
            "project_scanner_test",
            {{"mylib/mylib.hpp", "#include <vector>"}}};

        ::abuild::Cache cache{testProject.root() / "abuild.scanners_test.yaml"};
        ::abuild::ProjectScanner{cache}.scan();

        ::abuild::Project *project = cache.project("mylib");

        assert_(project).not_to_be(nullptr);
        assert_(project->headers.size()).to_be(1U);
        expect(project->headers[0]->tokens).to_be(std::vector<::abuild::Token>{::abuild::IncludeExternalToken{.name = "vector"}});
    });

    test("skip cached unchanged file", [] {
        const ::abuild::TestProject testProject{
            "project_scanner_test",
            {{"mylib/mylib.cpp", "export module mylib;\nimport other.module;"},
              {"mylib/mylib.hpp", "#include <vector>"}}
        };

        {
            ::abuild::Cache cache{testProject.root() / "abuild.scanners_test.yaml"};
            ::abuild::ProjectScanner{cache}.scan();

            auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(testProject.root() / "mylib/mylib.cpp").time_since_epoch()).count();
            {
                std::fstream eraseFile{testProject.root() / "mylib/mylib.cpp", static_cast<unsigned int>(std::ios::in) | static_cast<unsigned int>(std::ios::out) | static_cast<unsigned int>(std::ios::trunc)};
            }
            std::filesystem::last_write_time(testProject.root() / "mylib/mylib.cpp", std::filesystem::file_time_type{std::chrono::seconds{timestamp}});
            auto currentTimestamp = std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(testProject.root() / "mylib/mylib.cpp").time_since_epoch()).count();
            assert_(std::filesystem::file_size(testProject.root() / "mylib/mylib.cpp")).to_be(0U);
            assert_(currentTimestamp).to_be(timestamp);

            timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(testProject.root() / "mylib/mylib.hpp").time_since_epoch()).count();
            {
                std::fstream eraseFile{testProject.root() / "mylib/mylib.hpp", static_cast<unsigned int>(std::ios::in) | static_cast<unsigned int>(std::ios::out) | static_cast<unsigned int>(std::ios::trunc)};
            }
            std::filesystem::last_write_time(testProject.root() / "mylib/mylib.hpp", std::filesystem::file_time_type{std::chrono::seconds{timestamp}});
            assert_(std::filesystem::file_size(testProject.root() / "mylib/mylib.hpp")).to_be(0U);
            currentTimestamp = std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(testProject.root() / "mylib/mylib.hpp").time_since_epoch()).count();
            assert_(currentTimestamp).to_be(timestamp);
        }

        ::abuild::Cache cache{testProject.root() / "abuild.scanners_test.yaml"};
        ::abuild::ProjectScanner{cache}.scan();

        ::abuild::Project *project = cache.project("mylib");

        assert_(project).not_to_be(nullptr);
        assert_(project->sources.size()).to_be(1U);
        expect(project->sources[0]->tokens).to_be(std::vector<::abuild::Token>{
            ::abuild::ModuleToken{.name = "mylib", .exported = true},
            ::abuild::ImportModuleToken{.name = "other.module"              }
        });

        assert_(project->headers.size()).to_be(1U);
        expect(project->headers[0]->tokens).to_be(std::vector<::abuild::Token>{::abuild::IncludeExternalToken{.name = "vector"}});
    });

    test("update cached file", [] {
        const ::abuild::TestProject testProject{
            "project_scanner_test",
            {{"mylib/mylib.cpp", "export module mylib;\nimport other.module;"},
              {"mylib/mylib.hpp", "#include <vector>"}}
        };

        {
            ::abuild::Cache cache{testProject.root() / "abuild.scanners_test.yaml"};
            ::abuild::ProjectScanner{cache}.scan();

            std::fstream{testProject.root() / "mylib/mylib.cpp", static_cast<unsigned int>(std::ios::in) | static_cast<unsigned int>(std::ios::out) | static_cast<unsigned int>(std::ios::trunc)} << "export module mylib2;";
            std::filesystem::last_write_time(testProject.root() / "mylib/mylib.cpp", std::filesystem::file_time_type{std::chrono::seconds{1}});
            auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(testProject.root() / "mylib/mylib.cpp").time_since_epoch()).count();
            assert_(timestamp).to_be(1);

            std::fstream{testProject.root() / "mylib/mylib.hpp", static_cast<unsigned int>(std::ios::in) | static_cast<unsigned int>(std::ios::out) | static_cast<unsigned int>(std::ios::trunc)} << "#include <string>";
            std::filesystem::last_write_time(testProject.root() / "mylib/mylib.hpp", std::filesystem::file_time_type{std::chrono::seconds{1}});
            timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(testProject.root() / "mylib/mylib.hpp").time_since_epoch()).count();
            assert_(timestamp).to_be(1);
        }

        ::abuild::Cache cache{testProject.root() / "abuild.scanners_test.yaml"};
        ::abuild::ProjectScanner{cache}.scan();

        ::abuild::Project *project = cache.project("mylib");

        assert_(project).not_to_be(nullptr);
        assert_(project->sources.size()).to_be(1U);
        expect(project->sources[0]->tokens).to_be(std::vector<::abuild::Token>{
            ::abuild::ModuleToken{.name = "mylib2", .exported = true}
        });

        assert_(project->headers.size()).to_be(1U);
        expect(project->headers[0]->tokens).to_be(std::vector<::abuild::Token>{::abuild::IncludeExternalToken{.name = "string"}});
    });
});
