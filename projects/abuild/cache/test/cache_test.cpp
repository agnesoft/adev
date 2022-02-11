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
        ::abuild::TestFile file{"./abuild.cache_test.yaml"};

        {
            ::abuild::Cache cache{file.path()};
            cache.add_source_file("main.cpp");
            cache.add_source_file("source.cpp");
        }

        const auto node = ::YAML::LoadFile(file.path().string());

        expect(node["sources"]["main.cpp"].IsDefined()).to_be(true);
        expect(node["sources"]["source.cpp"].IsDefined()).to_be(true);
    });

    test("add_header_file()", [] {
        ::abuild::TestFile file{"./abuild.cache_test.yaml"};

        {
            ::abuild::Cache cache{file.path()};
            cache.add_header_file("header.hpp");
            cache.add_header_file("include/header.hpp");
        }

        const auto node = ::YAML::LoadFile(file.path().string());

        expect(node["headers"]["header.hpp"].IsDefined()).to_be(true);
        expect(node["headers"]["include/header.hpp"].IsDefined()).to_be(true);
    });

    test("add_project()", [] {
        ::abuild::TestFile file{"./abuild.cache_test.yaml"};

        {
            ::abuild::Cache cache{file.path()};
            ::abuild::Project *project = cache.add_project("my.project");
            project->sources.push_back(cache.add_source_file("main.cpp"));
            project->headers.push_back(cache.add_header_file("header.hpp"));
        }

        const auto node = ::YAML::LoadFile(file.path().string());

        expect(node["projects"]["my.project"].IsDefined()).to_be(true);
        expect(node["projects"]["my.project"]["sources"].as<std::vector<std::string>>())
            .to_be(std::vector<std::string>{"main.cpp"});
        expect(node["projects"]["my.project"]["headers"].as<std::vector<std::string>>())
            .to_be(std::vector<std::string>{"header.hpp"});
    });

    test("load from file", [] {
        ::YAML::Node node;
        node["projects"]["my_project"]["sources"].push_back("main.cpp");
        node["projects"]["my_project"]["headers"].push_back("my_header.hpp");
        node["sources"]["main.cpp"] = {};
        node["headers"]["my_header.hpp"] = {};

        ::abuild::TestFile file{"./abuild.cache_test.yaml"};
        std::ofstream{file.path()} << node;

        ::abuild::Cache cache{file.path()};

        expect(cache.exact_header_file("my_header.hpp")->path).to_be("my_header.hpp");
        expect(cache.exact_source_file("main.cpp")->path).to_be("main.cpp");

        const ::abuild::Project *project = cache.project("my_project");

        assert_(project->sources.size()).to_be(1U);
        expect(project->sources[0]).to_be(cache.exact_source_file("main.cpp"));
        assert_(project->headers.size()).to_be(1U);
        expect(project->headers[0]).to_be(cache.exact_header_file("my_header.hpp"));
    });
});
