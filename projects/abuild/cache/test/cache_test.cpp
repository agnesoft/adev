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
            cache.add_source_file("main.cpp", "my_project");
            cache.add_source_file("source.cpp", "my_project");
        }

        const auto node = ::YAML::LoadFile(file.path().string());

        expect(node["sources"]["main.cpp"].IsDefined()).to_be(true);
        expect(node["sources"]["source.cpp"].IsDefined()).to_be(true);
    });

    test("add_header_file()", [] {
        const ::abuild::TestFile file{"./abuild.cache_test.yaml"};

        {
            ::abuild::Cache cache{file.path()};
            cache.add_header_file("header.hpp", "my_project");
            cache.add_header_file("include/header.hpp", "my_project");
        }

        const auto node = ::YAML::LoadFile(file.path().string());

        expect(node["headers"]["header.hpp"].IsDefined()).to_be(true);
        expect(node["headers"]["include/header.hpp"].IsDefined()).to_be(true);
    });

    test("add project", [] {
        const ::abuild::TestFile file{"./abuild.cache_test.yaml"};

        {
            ::abuild::Cache cache{file.path()};
            cache.add_source_file("main.cpp", "my.project");
            cache.add_header_file("header.hpp", "my.project");
        }

        const auto node = ::YAML::LoadFile(file.path().string());

        expect(node["sources"]["main.cpp"]["project"].as<std::string>()).to_be("my.project");
        expect(node["headers"]["header.hpp"]["project"].as<std::string>()).to_be("my.project");
    });

    test("load from file", [] {
        ::YAML::Node node;
        node["sources"]["main.cpp"]["project"] = "my_project";
        node["headers"]["my_header.hpp"]["project"] = "my_project";

        std::stringstream stream;
        stream << node;
        const ::abuild::TestFile file{"./abuild.cache_test.yaml", stream.str()};

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
