import atest;
import abuild.cache;
import abuild.test_utilities;
import yamlcpp;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("Cache", [] { // NOLINT(cert-err58-cpp)
    test("add_source_file()", [] {
        const ::abuild::TestFile file{"./abuild.cache_test.yaml"};

        {
            ::abuild::Cache cache{file.path()};
            cache.add_source_file(::abuild::File{"main.cpp"}, "my_project");
            cache.add_source_file(::abuild::File{"source.cpp"}, "my_project");
        }

        const auto node = ::YAML::LoadFile(file.path().string());

        expect(node["sources"]["main.cpp"].IsDefined()).to_be(true);
        expect(node["sources"]["source.cpp"].IsDefined()).to_be(true);
    });

    test("add_header_file()", [] {
        const ::abuild::TestFile file{"./abuild.cache_test.yaml"};

        {
            ::abuild::Cache cache{file.path()};
            cache.add_header_file(::abuild::File{"header.hpp"}, "my_project");
            cache.add_header_file(::abuild::File{"include/header.hpp"}, "my_project");
        }

        const auto node = ::YAML::LoadFile(file.path().string());

        expect(node["headers"]["header.hpp"].IsDefined()).to_be(true);
        expect(node["headers"]["include/header.hpp"].IsDefined()).to_be(true);
    });

    test("add project", [] {
        const ::abuild::TestFile file{"./abuild.cache_test.yaml"};

        {
            ::abuild::Cache cache{file.path()};
            cache.add_source_file(::abuild::File{"main.cpp"}, "my.project");
            cache.add_header_file(::abuild::File{"header.hpp"}, "my.project");
        }

        const auto node = ::YAML::LoadFile(file.path().string());

        expect(node["sources"]["main.cpp"]["project"].as<std::string>()).to_be("my.project");
        expect(node["headers"]["header.hpp"]["project"].as<std::string>()).to_be("my.project");
    });

    test("load from file", [] {
        const ::abuild::TestProject testProject{
            "cache_test",
            {{"main.cpp", ""},
             {"my_header.hpp", ""}}};

        {
            ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
            cache.add_source_file(::abuild::File{testProject.root() / "main.cpp"}, "my_project");
            cache.add_header_file(::abuild::File{testProject.root() / "my_header.hpp"}, "my_project");
        }

        ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};

        ::abuild::HeaderFile *header = cache.exact_header_file(testProject.root() / "my_header.hpp");
        ::abuild::SourceFile *source = cache.exact_source_file(testProject.root() / "main.cpp");

        assert_(header).not_to_be(nullptr);
        assert_(source).not_to_be(nullptr);
        expect(header->path).to_be(testProject.root() / "my_header.hpp");
        expect(source->path).to_be(testProject.root() / "main.cpp");

        const ::abuild::Project *project = cache.project("my_project");

        assert_(project->sources.size()).to_be(1U);
        expect(project->sources[0]).to_be(source);
        assert_(project->headers.size()).to_be(1U);
        expect(project->headers[0]).to_be(header);
    });
});
