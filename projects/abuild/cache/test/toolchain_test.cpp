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

auto operator<<(std::ostream &stream, const ABI &abi) -> std::ostream &
{
    return stream << "{ " << abi.architecture << " | " << abi.bitness << " | " << abi.platform << " }";
}

auto operator<<(std::ostream &stream, const Toolchain &toolchain) -> std::ostream &
{
    return stream << "{ " << toolchain.name << " (" << toolchain.frontend << ')'
                  << ", C: " << toolchain.cCompiler.string()
                  << ", C++: " << toolchain.cppCompiler.string()
                  << ", Linker: " << toolchain.linker.string()
                  << ", Archiver: " << toolchain.archiver.string()
                  << ", ABI: " << toolchain.abi << " }";
}

[[nodiscard]] auto operator==(const ABI &left, const ABI &right) -> bool
{
    return left.architecture == right.architecture
        && left.bitness == right.bitness
        && left.platform == right.platform;
}

[[nodiscard]] auto operator==(const Toolchain &left, const Toolchain &right) -> bool
{
    return left.name == right.name
        && left.frontend == right.frontend
        && left.cCompiler == right.cCompiler
        && left.cppCompiler == right.cppCompiler
        && left.linker == right.linker
        && left.archiver == right.archiver
        && left.abi == right.abi;
}
}

static const auto S = suite("Toolchain", [] { // NOLINT(cert-err58-cpp)
    test("toolchain", [] {
        const ::abuild::TestProject testProject{
            "cache_test",
            {{"usr/bin/gcc", ""},
              {"usr/bin/g++", ""},
              {"usr/bin/ld", ""},
              {"usr/bin/ar", ""}}
        };

        const ::abuild::Toolchain toolchain{
            .name = "gcc",
            .frontend = ::abuild::Toolchain::Frontend::GCC,
            .cCompiler = testProject.root() / "usr/bin/gcc",
            .cppCompiler = testProject.root() / "usr/bin/g++",
            .linker = testProject.root() / "usr/bin/ld",
            .archiver = testProject.root() / "usr/bin/ar",
            .abi = ::abuild::ABI{.architecture = ::abuild::ABI::Architecture::X86,
                                 .bitness = ::abuild::ABI::Bitness::X64,
                                 .platform = ::abuild::ABI::Platform::Linux}
        };

        {
            ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
            expect(cache.toolchain()).to_be(::abuild::Toolchain{});
            cache.set_toolchain(::abuild::Toolchain{toolchain});
            expect(cache.toolchain()).to_be(toolchain);
        }

        ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
        expect(cache.toolchain()).to_be(toolchain);
    });
});