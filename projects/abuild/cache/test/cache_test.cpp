import atest;
import abuild.cache;
import abuild.test_utilities;
import yamlcpp;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

namespace abuild
{
auto operator<<(std::ostream &stream, const Toolchain::Frontend &frontend) -> std::ostream &
{
    switch (frontend)
    {
    case Toolchain::Frontend::Clang:
        return stream << "Clang";
    case Toolchain::Frontend::GCC:
        return stream << "GCC";
    case Toolchain::Frontend::MSVC:
        return stream << "MSVC";
    };

    return stream << "Unknown";
}

auto operator<<(std::ostream &stream, const ABI::Architecture &architecture) -> std::ostream &
{
    switch (architecture)
    {
    case ABI::Architecture::X86:
        return stream << "X86";
    case ABI::Architecture::ARM:
        return stream << "ARM";
    };

    return stream << "Unknown";
}

auto operator<<(std::ostream &stream, const ABI::Bitness &bitness) -> std::ostream &
{
    switch (bitness)
    {
    case ABI::Bitness::X32:
        return stream << "X32";
    case ABI::Bitness::X64:
        return stream << "X64";
    };

    return stream << "Unknown";
}

auto operator<<(std::ostream &stream, const ABI::Platform &platform) -> std::ostream &
{
    switch (platform)
    {
    case ABI::Platform::Linux:
        return stream << "Linux";
    case ABI::Platform::Unix:
        return stream << "Unix";
    case ABI::Platform::Windows:
        return stream << "Windows";
    };

    return stream << "Unknown";
}
}

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

    test("load toolchains", [] {
        const ::abuild::TestProject testProject{
            "cache_test",
            {{"usr/bin/gcc", ""},
              {"usr/bin/g++", ""},
              {"usr/bin/ld", ""},
              {"usr/bin/clang", ""},
              {"usr/bin/clang++", ""},
              {"usr/bin/lld", ""},
              {R"(Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.31.31103\bin\Hostx86\x86\cl.exe)", ""},
              {R"(Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.31.31103\bin\Hostx86\x86\link.exe)", ""}}
        };

        {
            ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
            ::abuild::Toolchain *gcc = cache.add_toolchain("gcc_x64");
            gcc->cCompiler = testProject.root() / "usr/bin/gcc";
            gcc->cppCompiler = testProject.root() / "usr/bin/g++";
            gcc->linker = testProject.root() / "usr/bin/ld";
            gcc->archiver = testProject.root() / "usr/bin/ar";
            gcc->abi = ::abuild::ABI{.architecture = ::abuild::ABI::Architecture::X86,
                                     .bitness = ::abuild::ABI::Bitness::X64,
                                     .platform = ::abuild::ABI::Platform::Linux};

            ::abuild::Toolchain *msvc = cache.add_toolchain("msvc2022_x86");
            msvc->frontend = ::abuild::Toolchain::Frontend::MSVC;
            msvc->cCompiler = testProject.root() / R"(Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.31.31103\bin\Hostx86\x86\cl.exe)";
            msvc->cppCompiler = testProject.root() / R"(Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.31.31103\bin\Hostx86\x86\cl.exe)";
            msvc->linker = testProject.root() / R"(Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.31.31103\bin\Hostx86\x86\link.exe)";
            msvc->abi = ::abuild::ABI{.architecture = ::abuild::ABI::Architecture::X86,
                                      .bitness = ::abuild::ABI::Bitness::X32,
                                      .platform = ::abuild::ABI::Platform::Windows};

            ::abuild::Toolchain *clang = cache.add_toolchain("clang_x64");
            clang->frontend = ::abuild::Toolchain::Frontend::Clang;
            clang->cCompiler = testProject.root() / "usr/bin/clang";
            clang->cppCompiler = testProject.root() / "usr/bin/clang++";
            clang->linker = testProject.root() / "usr/bin/lld";
            clang->archiver = testProject.root() / "usr/bin/llvm-ar";
            clang->abi = ::abuild::ABI{.architecture = ::abuild::ABI::Architecture::ARM,
                                       .bitness = ::abuild::ABI::Bitness::X64,
                                       .platform = ::abuild::ABI::Platform::Unix};
        }

        ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};

        assert_(cache.toolchains().size()).to_be(3U);

        expect(cache.toolchains()[0]->name).to_be("gcc_x64");
        expect(cache.toolchains()[0]->frontend).to_be(::abuild::Toolchain::Frontend::GCC);
        expect(cache.toolchains()[0]->cCompiler).to_be(testProject.root() / "usr/bin/gcc");
        expect(cache.toolchains()[0]->cppCompiler).to_be(testProject.root() / "usr/bin/g++");
        expect(cache.toolchains()[0]->linker).to_be(testProject.root() / "usr/bin/ld");
        expect(cache.toolchains()[0]->archiver).to_be(testProject.root() / "usr/bin/ar");
        expect(cache.toolchains()[0]->abi.architecture).to_be(::abuild::ABI::Architecture::X86);
        expect(cache.toolchains()[0]->abi.bitness).to_be(::abuild::ABI::Bitness::X64);
        expect(cache.toolchains()[0]->abi.platform).to_be(::abuild::ABI::Platform::Linux);

        expect(cache.toolchains()[1]->name).to_be("msvc2022_x86");
        expect(cache.toolchains()[1]->frontend).to_be(::abuild::Toolchain::Frontend::MSVC);
        expect(cache.toolchains()[1]->cCompiler).to_be(testProject.root() / R"(Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.31.31103\bin\Hostx86\x86\cl.exe)");
        expect(cache.toolchains()[1]->cppCompiler).to_be(testProject.root() / R"(Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.31.31103\bin\Hostx86\x86\cl.exe)");
        expect(cache.toolchains()[1]->linker).to_be(testProject.root() / R"(Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.31.31103\bin\Hostx86\x86\link.exe)");
        expect(cache.toolchains()[1]->archiver).to_be(std::filesystem::path{});
        expect(cache.toolchains()[1]->abi.architecture).to_be(::abuild::ABI::Architecture::X86);
        expect(cache.toolchains()[1]->abi.bitness).to_be(::abuild::ABI::Bitness::X32);
        expect(cache.toolchains()[1]->abi.platform).to_be(::abuild::ABI::Platform::Windows);

        expect(cache.toolchains()[2]->name).to_be("clang_x64");
        expect(cache.toolchains()[2]->frontend).to_be(::abuild::Toolchain::Frontend::Clang);
        expect(cache.toolchains()[2]->cCompiler).to_be(testProject.root() / "usr/bin/clang");
        expect(cache.toolchains()[2]->cppCompiler).to_be(testProject.root() / "usr/bin/clang++");
        expect(cache.toolchains()[2]->linker).to_be(testProject.root() / "usr/bin/lld");
        expect(cache.toolchains()[2]->archiver).to_be(testProject.root() / "usr/bin/llvm-ar");
        expect(cache.toolchains()[2]->abi.architecture).to_be(::abuild::ABI::Architecture::ARM);
        expect(cache.toolchains()[2]->abi.bitness).to_be(::abuild::ABI::Bitness::X64);
        expect(cache.toolchains()[2]->abi.platform).to_be(::abuild::ABI::Platform::Unix);
    });

    test("load missing toolchains", [] {
        const ::abuild::TestProject testProject{
            "cache_test",
            {{"usr/bin/g++", ""},
              {"usr/bin/ld", ""},
              {"usr/bin/clang", ""},
              {"usr/bin/lld", ""},
              {R"(Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.31.31103\bin\Hostx86\x86\cl.exe)", ""}}
        };

        {
            ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
            ::abuild::Toolchain *gcc = cache.add_toolchain("gcc_x64");
            gcc->cCompiler = testProject.root() / "usr/bin/gcc";
            gcc->cppCompiler = testProject.root() / "usr/bin/g++";
            gcc->linker = testProject.root() / "usr/bin/ld";

            ::abuild::Toolchain *msvc = cache.add_toolchain("msvc2022_x86");
            msvc->cCompiler = testProject.root() / R"(Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.31.31103\bin\Hostx86\x86\cl.exe)";
            msvc->cppCompiler = testProject.root() / R"(Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.31.31103\bin\Hostx86\x86\cl.exe)";
            msvc->linker = testProject.root() / R"(Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.31.31103\bin\Hostx86\x86\link.exe)";

            ::abuild::Toolchain *clang = cache.add_toolchain("clang_x64");
            clang->cCompiler = testProject.root() / "usr/bin/clang";
            clang->cppCompiler = testProject.root() / "usr/bin/clang++";
            clang->linker = testProject.root() / "usr/bin/lld";
        }

        ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};

        assert_(cache.toolchains().size()).to_be(0U);
    });
});
