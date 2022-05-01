import atest;
import abuild.cache;
import abuild.test_utilities;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

namespace abuild
{
auto operator<<(std::ostream &stream, const Project::Type &type) -> std::ostream &
{
    switch (type)
    {
    case Project::Type::Executable:
        return stream << "Executable";
    case Project::Type::DynamicLibrary:
        return stream << "DynamicLibrary";
    case Project::Type::StaticLibrary:
        return stream << "StaticLibrary";
    };

    return stream << "Unknown";
}
}

static const auto S = suite("Project", [] { // NOLINT(cert-err58-cpp)
    test("add project", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::string name = "my_project";
        const std::filesystem::path path = "some/path/to/binary.exe";

        {
            ::abuild::Cache cache{testFile.path()};
            ::abuild::Project *project = cache.add_project(name);
            assert_(project).not_to_be(nullptr);
            expect(project->name).to_be(name);
            project->linkedFile.path = path;
        }

        ::abuild::Cache cache{testFile.path()};
        ::abuild::Project *project = cache.project(name);
        assert_(project).not_to_be(nullptr);
        expect(project->name).to_be(name);
        expect(project->linkedFile.path).to_be(path);
    });

    test("project type", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};

        {
            ::abuild::Cache cache{testFile.path()};

            ::abuild::Project *project = cache.add_project("project1");
            assert_(project).not_to_be(nullptr);
            project->type = ::abuild::Project::Type::Executable;

            project = cache.add_project("project2");
            assert_(project).not_to_be(nullptr);
            project->type = ::abuild::Project::Type::DynamicLibrary;

            project = cache.add_project("project3");
            assert_(project).not_to_be(nullptr);
            project->type = ::abuild::Project::Type::StaticLibrary;
        }

        ::abuild::Cache cache{testFile.path()};
        ::abuild::Project *project1 = cache.project("project1");
        assert_(project1).not_to_be(nullptr);
        expect(project1->type).to_be(::abuild::Project::Type::Executable);

        ::abuild::Project *project2 = cache.project("project2");
        assert_(project2).not_to_be(nullptr);
        expect(project2->type).to_be(::abuild::Project::Type::DynamicLibrary);

        ::abuild::Project *project3 = cache.project("project3");
        assert_(project3).not_to_be(nullptr);
        expect(project3->type).to_be(::abuild::Project::Type::StaticLibrary);
    });

    test("with files", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::string name = "my_project";

        {
            ::abuild::Cache cache{"./abuild.cache_test.yaml"};
            ::abuild::Project *project = cache.add_project(name);
            assert_(project).not_to_be(nullptr);
            project->sources.push_back(cache.add_source_file("my_project/main.cpp"));
            project->sources.push_back(cache.add_source_file("my_project/source1.cpp"));
            project->sources.push_back(cache.add_source_file("my_project/source2.cpp"));
            project->headers.push_back(cache.add_header_file("my_project/header1.cpp"));
            project->headers.push_back(cache.add_header_file("my_project/header2.cpp"));
        }

        ::abuild::Cache cache{"./abuild.cache_test.yaml"};
        ::abuild::Project *project = cache.project(name);
        assert_(project).not_to_be(nullptr);

        expect(project->sources)
            .to_be(std::vector<::abuild::SourceFile *>{
                cache.exact_source_file("my_project/main.cpp"),
                cache.exact_source_file("my_project/source1.cpp"),
                cache.exact_source_file("my_project/source2.cpp")});

        expect(project->headers)
            .to_be(std::vector<::abuild::HeaderFile *>{
                cache.exact_header_file("my_project/header1.cpp"),
                cache.exact_header_file("my_project/header2.cpp")});
    });

    test("projects", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::string projectName1 = "project1";
        const std::string projectName2 = "project2";
        const std::string projectName3 = "project3";

        {
            ::abuild::Cache cache{"./abuild.cache_test.yaml"};
            ::abuild::Project *project1 = cache.add_project(projectName1);
            ::abuild::Project *project2 = cache.add_project(projectName2);
            ::abuild::Project *project3 = cache.add_project(projectName3);
            assert_(project1).not_to_be(nullptr);
            assert_(project2).not_to_be(nullptr);
            assert_(project3).not_to_be(nullptr);
        }

        ::abuild::Cache cache{"./abuild.cache_test.yaml"};
        ::abuild::Project *project1 = cache.project(projectName1);
        ::abuild::Project *project2 = cache.project(projectName2);
        ::abuild::Project *project3 = cache.project(projectName3);
        assert_(project1).not_to_be(nullptr);
        assert_(project2).not_to_be(nullptr);
        assert_(project3).not_to_be(nullptr);
        assert_(cache.projects().size()).to_be(3U);

        expect(cache.projects()[0].get()).to_be(project1);
        expect(cache.projects()[1].get()).to_be(project2);
        expect(cache.projects()[2].get()).to_be(project3);
    });
});
