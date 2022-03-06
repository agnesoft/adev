import atest;
import abuild.scanners;
import abuild.test_utilities;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

namespace abuild
{
auto operator<<(std::ostream &stream, const ::abuild::Project::Type &type) -> std::ostream &
{
    switch (type)
    {
    case Project::Type::StaticLibrary:
        return stream << "StaticLibrary (" << static_cast<std::underlying_type_t<::abuild::Project::Type>>(type) << ')';
    case Project::Type::DynamicLibrary:
        return stream << "DynamicLibrary (" << static_cast<std::underlying_type_t<::abuild::Project::Type>>(type) << ')';
    case Project::Type::Executable:
        return stream << "Executable (" << static_cast<std::underlying_type_t<::abuild::Project::Type>>(type) << ')';
    }

    throw std::logic_error{"Unknown enum value of ::abuild::Project::Type (" + std::to_string(static_cast<std::underlying_type_t<::abuild::Project::Type>>(type)) + ')'};
}
}

static const auto S = suite("ProjectScanner", [] { // NOLINT(cert-err58-cpp)
    test("single file project", [] {
        const ::abuild::TestProject testProject{
            "project_scanner_test",
            {{"main.cpp", ""}}};

        ::abuild::Cache cache{testProject.root() / "abuild.scanners_test.yaml"};
        ::abuild::ProjectScanner{cache}.scan();

        ::abuild::Project *project = cache.project("project_scanner_test");

        assert_(project).not_to_be(nullptr);
        expect(project->name).to_be("project_scanner_test");
        expect(project->type).to_be(::abuild::Project::Type::Executable);
        assert_(project->sources.size()).to_be(1U);
        expect(project->sources[0]->path).to_be("project_scanner_test/main.cpp");
    });

    test("skip dirs", [] {
        const ::abuild::TestProject testProject{
            "project_scanner_test",
            {{"projects/myapp/main.cpp", ""},
             {"projects/mylib/my_lib.cpp", ""}}};

        ::abuild::Cache cache{testProject.root() / "abuild.scanners_test.yaml"};
        ::abuild::ProjectScanner{cache}.scan();

        ::abuild::Project *project = cache.project("myapp");

        assert_(project).not_to_be(nullptr);
        expect(project->name).to_be("myapp");
        expect(project->type).to_be(::abuild::Project::Type::Executable);
        assert_(project->sources.size()).to_be(1U);
        expect(project->sources[0]->path).to_be("project_scanner_test/projects/myapp/main.cpp");

        project = cache.project("mylib");

        assert_(project).not_to_be(nullptr);
        expect(project->name).to_be("mylib");
        expect(project->type).to_be(::abuild::Project::Type::StaticLibrary);
        assert_(project->sources.size()).to_be(1U);
        expect(project->sources[0]->path).to_be("project_scanner_test/projects/mylib/my_lib.cpp");
    });

    test("squash dirs", [] {
        const ::abuild::TestProject testProject{
            "project_scanner_test",
            {{"myapp/src/main.cpp", ""},
             {"myapp/src/src/source.cpp", ""},
             {"myapp/source.cpp", ""}}};

        ::abuild::Cache cache{testProject.root() / "abuild.scanners_test.yaml"};
        ::abuild::ProjectScanner{cache}.scan();

        ::abuild::Project *project = cache.project("myapp");

        assert_(project).not_to_be(nullptr);
        expect(project->name).to_be("myapp");
        expect(project->type).to_be(::abuild::Project::Type::Executable);
        assert_(project->sources.size()).to_be(3U);
        expect(project->sources[0]->path).to_be("project_scanner_test/myapp/source.cpp");
        expect(project->sources[1]->path).to_be("project_scanner_test/myapp/src/main.cpp");
        expect(project->sources[2]->path).to_be("project_scanner_test/myapp/src/src/source.cpp");
    });

    test("sub project", [] {
        const ::abuild::TestProject testProject{
            "project_scanner_test",
            {{"myapp/main.cpp", ""},
             {"myapp/subapp/main.cpp", ""}}};

        ::abuild::Cache cache{testProject.root() / "abuild.scanners_test.yaml"};
        ::abuild::ProjectScanner{cache}.scan();

        ::abuild::Project *project = cache.project("myapp");

        assert_(project).not_to_be(nullptr);
        expect(project->name).to_be("myapp");
        expect(project->type).to_be(::abuild::Project::Type::Executable);
        assert_(project->sources.size()).to_be(1U);
        expect(project->sources[0]->path).to_be("project_scanner_test/myapp/main.cpp");

        project = cache.project("myapp.subapp");
        assert_(project).not_to_be(nullptr);
        expect(project->name).to_be("myapp.subapp");
        expect(project->type).to_be(::abuild::Project::Type::Executable);
        assert_(project->sources.size()).to_be(1U);
        expect(project->sources[0]->path).to_be("project_scanner_test/myapp/subapp/main.cpp");
    });

    test("ignore dirs", [] {
        const ::abuild::TestProject testProject{
            "project_scanner_test",
            {{"build/myapp/main.cpp", ""},
             {".somedir/header.hpp", ""},
             {"someapp/.hidden/source.cpp", ""}}};

        ::abuild::Cache cache{testProject.root() / "abuild.scanners_test.yaml"};
        ::abuild::ProjectScanner{cache}.scan();

        expect(cache.projects().size()).to_be(0U);
    });

    test("test project", [] {
        const ::abuild::TestProject testProject{
            "project_scanner_test",
            {{"myapp/main.cpp", ""},
             {"myapp/test/main.cpp", ""}}};

        ::abuild::Cache cache{testProject.root() / "abuild.scanners_test.yaml"};
        ::abuild::ProjectScanner{cache}.scan();

        ::abuild::Project *project = cache.project("myapp");

        assert_(project).not_to_be(nullptr);
        expect(project->name).to_be("myapp");
        expect(project->type).to_be(::abuild::Project::Type::Executable);
        assert_(project->sources.size()).to_be(1U);
        expect(project->sources[0]->path).to_be("project_scanner_test/myapp/main.cpp");

        project = cache.project("myapp.test");
        assert_(project).not_to_be(nullptr);
        expect(project->name).to_be("myapp.test");
        expect(project->type).to_be(::abuild::Project::Type::Executable);
        assert_(project->sources.size()).to_be(1U);
        expect(project->sources[0]->path).to_be("project_scanner_test/myapp/test/main.cpp");
    });

    test("headers", [] {
        const ::abuild::TestProject testProject{
            "project_scanner_test",
            {{"mylib/mylib.cpp", ""},
             {"mylib/include/some_header.hpp", ""},
             {"mylib/include/mylib/some_other_header.hpp", ""}}};

        ::abuild::Cache cache{testProject.root() / "abuild.scanners_test.yaml"};
        ::abuild::ProjectScanner{cache}.scan();

        ::abuild::Project *project = cache.project("mylib");

        assert_(project).not_to_be(nullptr);
        expect(project->name).to_be("mylib");
        expect(project->type).to_be(::abuild::Project::Type::StaticLibrary);
        assert_(project->headers.size()).to_be(2U);
        expect(project->headers[0]->path).to_be("project_scanner_test/mylib/include/mylib/some_other_header.hpp");
        expect(project->headers[1]->path).to_be("project_scanner_test/mylib/include/some_header.hpp");
    });

    test("tokens", [] {
        const ::abuild::TestProject testProject{
            "project_scanner_test",
            {{"mylib/mylib.cpp", "export module mylib;\nimport other.module;"}}};

        ::abuild::Cache cache{testProject.root() / "abuild.scanners_test.yaml"};
        ::abuild::ProjectScanner{cache}.scan();

        ::abuild::Project *project = cache.project("mylib");

        assert_(project).not_to_be(nullptr);
        expect(project->name).to_be("mylib");
        expect(project->type).to_be(::abuild::Project::Type::StaticLibrary);
        assert_(project->sources.size()).to_be(1U);
        expect(project->sources[0]->tokens).to_be(std::vector<::abuild::Token>{::abuild::ModuleToken{.name = "mylib", .exported = true}, ::abuild::ImportModuleToken{.name = "other.module"}});
    });

    test("tokens", [] {
        const ::abuild::TestProject testProject{
            "project_scanner_test",
            {{"mylib/mylib.cpp", "export module mylib;\nimport other.module;"}}};

        ::abuild::Cache cache{testProject.root() / "abuild.scanners_test.yaml"};
        ::abuild::ProjectScanner{cache}.scan();

        ::abuild::Project *project = cache.project("mylib");

        assert_(project).not_to_be(nullptr);
        assert_(project->sources.size()).to_be(1U);
        expect(project->sources[0]->tokens).to_be(std::vector<::abuild::Token>{::abuild::ModuleToken{.name = "mylib", .exported = true}, ::abuild::ImportModuleToken{.name = "other.module"}});
    });

    test("skip cached unchanged file", [] {
        const ::abuild::TestProject testProject{
            "project_scanner_test",
            {{"mylib/mylib.cpp", "export module mylib;\nimport other.module;"},
             {"mylib/mylib.hpp", "#include <vector>"}}};

        {
            ::abuild::Cache cache{testProject.root() / "abuild.scanners_test.yaml"};
            ::abuild::ProjectScanner{cache}.scan();

            auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(testProject.root() / "mylib/mylib.cpp").time_since_epoch()).count();
            std::fstream{testProject.root() / "mylib/mylib.cpp", static_cast<unsigned int>(std::ios::in) | static_cast<unsigned int>(std::ios::out) | static_cast<unsigned int>(std::ios::trunc)};
            std::filesystem::last_write_time(testProject.root() / "mylib/mylib.cpp", {});
            assert_(std::filesystem::file_size(testProject.root() / "mylib/mylib.cpp")).to_be(0U);
            auto currentTimestamp = std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(testProject.root() / "mylib/mylib.cpp").time_since_epoch()).count();
            assert_(currentTimestamp).to_be(timestamp);

            timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(testProject.root() / "mylib/mylib.hpp").time_since_epoch()).count();
            std::fstream{testProject.root() / "mylib/mylib.hpp", static_cast<unsigned int>(std::ios::in) | static_cast<unsigned int>(std::ios::out) | static_cast<unsigned int>(std::ios::trunc)};
            std::filesystem::last_write_time(testProject.root() / "mylib/mylib.hpp", {});
            assert_(std::filesystem::file_size(testProject.root() / "mylib/mylib.hpp")).to_be(0U);
            currentTimestamp = std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(testProject.root() / "mylib/mylib.hpp").time_since_epoch()).count();
            assert_(currentTimestamp).to_be(timestamp);
        }

        ::abuild::Cache cache{testProject.root() / "abuild.scanners_test.yaml"};
        ::abuild::ProjectScanner{cache}.scan();

        ::abuild::Project *project = cache.project("mylib");

        assert_(project).not_to_be(nullptr);
        assert_(project->sources.size()).to_be(1U);
        expect(project->sources[0]->tokens).to_be(std::vector<::abuild::Token>{::abuild::ModuleToken{.name = "mylib", .exported = true}, ::abuild::ImportModuleToken{.name = "other.module"}});

        assert_(project->headers.size()).to_be(1U);
        expect(project->headers[0]->tokens).to_be(std::vector<::abuild::Token>{::abuild::IncludeExternalToken{.name = "vector"}});
    });

    test("update cached file", [] {
        const ::abuild::TestProject testProject{
            "project_scanner_test",
            {{"mylib/mylib.cpp", "export module mylib;\nimport other.module;"},
             {"mylib/mylib.hpp", "#include <vector>"}}};

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
        expect(project->sources[0]->tokens).to_be(std::vector<::abuild::Token>{::abuild::ModuleToken{.name = "mylib2", .exported = true}});

        assert_(project->headers.size()).to_be(1U);
        expect(project->headers[0]->tokens).to_be(std::vector<::abuild::Token>{::abuild::IncludeExternalToken{.name = "string"}});
    });
});
