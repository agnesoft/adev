import atest;
import abuild.cache;
import abuild.test_utilities;
import yamlcpp;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("Cache", [] { // NOLINT(cert-err58-cpp)
    test("add_configuration()", [] {
        const ::abuild::TestFile file{"./abuild.cache_test.yaml"};
        ::abuild::Cache cache{file.path()};

        ::abuild::Toolchain *gcc = cache.add_toolchain("gcc");
        ::abuild::Configuration *release = cache.add_configuration(gcc, "release");
        ::abuild::Configuration *debug = cache.add_configuration(gcc, "debug");
        ::abuild::Configuration *addressSanitizer = cache.add_configuration(gcc, "address");

        std::vector<::abuild::Configuration *> configurations;
        configurations.reserve(cache.configurations().size());

        for (const std::unique_ptr<::abuild::Configuration> &configuration : cache.configurations())
        {
            configurations.push_back(configuration.get());
        }

        expect(configurations).to_be(std::vector<::abuild::Configuration *>{release, debug, addressSanitizer});
        expect(cache.configuration(gcc, "release")).to_be(release);
        expect(cache.configuration(gcc, "debug")).to_be(debug);
        expect(cache.configuration(gcc, "address")).to_be(addressSanitizer);
        expect(cache.configuration(gcc, "memory")).to_be(nullptr);
        expect(cache.configuration(nullptr, "release")).to_be(nullptr);
        expect(cache.configuration(nullptr, "")).to_be(nullptr);
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

    test("add_toolchain()", [] {
        const ::abuild::TestFile file{"./abuild.cache_test.yaml"};

        ::abuild::Cache cache{file.path()};
        ::abuild::Toolchain *gcc = cache.add_toolchain("gcc_x64");
        ::abuild::Toolchain *msvc = cache.add_toolchain("msvc_x64");
        ::abuild::Toolchain *clang = cache.add_toolchain("clang_x64");

        std::vector<::abuild::Toolchain *> toolchains;
        toolchains.reserve(cache.toolchains().size());

        for (const std::unique_ptr<::abuild::Toolchain> &toolchain : cache.toolchains())
        {
            toolchains.push_back(toolchain.get());
        }

        expect(toolchains).to_be(std::vector<::abuild::Toolchain *>{gcc, msvc, clang});

        expect(cache.toolchain("gcc_x64")).to_be(gcc);
        expect(cache.toolchain("msvc_x64")).to_be(msvc);
        expect(cache.toolchain("clang_x64")).to_be(clang);
        expect(cache.toolchain("gcc")).to_be(nullptr);
        expect(cache.toolchain("gcc_x32")).to_be(nullptr);
        expect(cache.toolchain("")).to_be(nullptr);
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
        const ::abuild::TestProject testProject{
            "cache_test",
            {{"main.cpp", ""},
              {"my_header.hpp", ""}}
        };

        {
            ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
            cache.add_source_file(testProject.root() / "main.cpp", "my_project");
            cache.add_header_file(testProject.root() / "my_header.hpp", "my_project");
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

    test("load with missing files", [] {
        const ::abuild::TestProject testProject{
            "cache_test",
            {{"main.cpp", ""},
              {"source.cpp", ""},
              {"my_header.hpp", ""},
              {"header.hpp", ""}}
        };

        {
            ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
            cache.add_source_file(testProject.root() / "main.cpp", "my_project");
            cache.add_source_file(testProject.root() / "source.cpp", "my_project");
            cache.add_header_file(testProject.root() / "my_header.hpp", "my_project");
            cache.add_header_file(testProject.root() / "header.hpp", "my_project");

            assert_(cache.source_files().size()).to_be(2U);
            assert_(cache.header_files().size()).to_be(2U);
        }

        std::filesystem::remove(testProject.root() / "source.cpp");
        std::filesystem::remove(testProject.root() / "header.hpp");

        ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};

        expect(cache.source_files().size()).to_be(1U);
        expect(cache.source_files()[0]->path).to_be(testProject.root() / "main.cpp");
        expect(cache.header_files().size()).to_be(1U);
        expect(cache.header_files()[0]->path).to_be(testProject.root() / "my_header.hpp");
    });
});
