import atest;
import abuild.cache;
import abuild.test_utilities;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("Cache", [] { // NOLINT(cert-err58-cpp)
    test("add_source_file()", [] {
        ::abuild::TestFile file{"./abuild.cache_test_.yaml"};

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
        ::abuild::TestFile file{"./abuild.cache_test_.yaml"};

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
        ::abuild::TestFile file{"./abuild.cache_test_.yaml"};

        {
            ::abuild::Cache cache{file.path()};
            ::abuild::Project &project = cache.add_project("my.project");
            project.sources.push_back(&cache.add_source_file("main.cpp"));
            project.headers.push_back(&cache.add_header_file("header.hpp"));
        }

        const auto node = ::YAML::LoadFile(file.path().string());

        expect(node["projects"]["my.project"].IsDefined()).to_be(true);
        expect(node["projects"]["my.project"]["sources"].as<std::vector<std::string>>())
            .to_be(std::vector<std::string>{"main.cpp"});
        expect(node["projects"]["my.project"]["headers"].as<std::vector<std::string>>())
            .to_be(std::vector<std::string>{"header.hpp"});
    });
});
