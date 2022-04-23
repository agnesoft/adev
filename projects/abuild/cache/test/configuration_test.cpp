import atest;
import abuild.cache;
import abuild.test_utilities;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

namespace abuild
{
auto operator<<(std::ostream &stream, const Visibility &visibility) -> std::ostream &
{
    switch (visibility)
    {
    case Visibility::Public:
        return stream << "Public";
    case Visibility::Private:
        return stream << "Private";
    }

    return stream << "Unknown";
}

auto operator<<(std::ostream &stream, const Flag &flag) -> std::ostream &
{
    stream << flag.name;

    if (!flag.value.empty())
    {
        stream << '=' << flag.value;
    }

    if (flag.affectsABI)
    {
        stream << " [abi]";
    }

    return stream;
}

[[nodiscard]] auto operator==(const Flag &left, const Flag &right) -> bool
{
    return left.name == right.name && left.value == right.value && left.affectsABI == right.affectsABI;
}
}

static const auto S = suite("Configuration", [] { // NOLINT(cert-err58-cpp)
    test("add_dynamic_library()", [] {
        ::abuild::Toolchain toolchain;
        ::abuild::Configuration config("release", &toolchain);
        ::abuild::Project project1{.name = "my_lib"};
        ::abuild::Project project2{.name = "Kernel"};

        ::abuild::DynamicLibrary *lib1 = config.add_dynamic_library(&project1);
        ::abuild::DynamicLibrary *lib2 = config.add_dynamic_library(&project2);

        std::vector<::abuild::DynamicLibrary *> libs;
        libs.reserve(config.dynamic_libraries().size());

        for (const std::unique_ptr<::abuild::DynamicLibrary> &lib : config.dynamic_libraries())
        {
            libs.push_back(lib.get());
        }

        expect(libs).to_be(std::vector<::abuild::DynamicLibrary *>{lib1, lib2});
    });

    test("add_executable()", [] {
        ::abuild::Toolchain toolchain;
        ::abuild::Configuration config("release", &toolchain);
        ::abuild::Project project1{.name = "my_app"};
        ::abuild::Project project2{.name = "my_other_app"};

        ::abuild::Executable *exe1 = config.add_executable(&project1);
        ::abuild::Executable *exe2 = config.add_executable(&project2);

        std::vector<::abuild::Executable *> exes;
        exes.reserve(config.executables().size());

        for (const std::unique_ptr<::abuild::Executable> &exe : config.executables())
        {
            exes.push_back(exe.get());
        }

        expect(exes).to_be(std::vector<::abuild::Executable *>{exe1, exe2});
    });

    test("add_static_library()", [] {
        ::abuild::Toolchain toolchain;
        ::abuild::Configuration config("release", &toolchain);
        ::abuild::Project project1{.name = "my_lib"};
        ::abuild::Project project2{.name = "Kernel"};

        ::abuild::StaticLibrary *lib1 = config.add_static_library(&project1);
        ::abuild::StaticLibrary *lib2 = config.add_static_library(&project2);

        std::vector<::abuild::StaticLibrary *> libs;
        libs.reserve(config.static_libraries().size());

        for (const std::unique_ptr<::abuild::StaticLibrary> &lib : config.static_libraries())
        {
            libs.push_back(lib.get());
        }

        expect(libs).to_be(std::vector<::abuild::StaticLibrary *>{lib1, lib2});
    });

    test("add_header_unit()", [] {
        ::abuild::Toolchain toolchain;
        ::abuild::Configuration config("release", &toolchain);
        ::abuild::HeaderFile header1{{{"some_header.hpp"}}};
        ::abuild::HeaderFile header2{{{"source.cpp"}}};

        ::abuild::HeaderUnit *headerUnit1 = config.add_header_unit(&header1, ::abuild::Visibility::Public);
        ::abuild::HeaderUnit *headerUnit2 = config.add_header_unit(&header2, ::abuild::Visibility::Private);
        ::abuild::HeaderUnit *headerUnit3 = config.add_header_unit(&header1, ::abuild::Visibility::Private);

        expect(headerUnit1).to_be(headerUnit3);
        expect(headerUnit1->visibility).to_be(::abuild::Visibility::Public);

        std::vector<::abuild::HeaderUnit *> headerUnits;
        headerUnits.reserve(config.header_units().size());

        for (const std::unique_ptr<::abuild::HeaderUnit> &headerUnit : config.header_units())
        {
            headerUnits.push_back(headerUnit.get());
        }

        expect(headerUnits).to_be(std::vector<::abuild::HeaderUnit *>{headerUnit1, headerUnit2});
    });

    test("add_module()", [] {
        ::abuild::Toolchain toolchain;
        ::abuild::Configuration config("release", &toolchain);
        ::abuild::TranslationUnit unit;
        ::abuild::Module *mod1 = config.add_module("my_mod", nullptr, ::abuild::Visibility::Public);
        ::abuild::Module *mod2 = config.add_module("my_mod", &unit, ::abuild::Visibility::Private);

        expect(mod1).to_be(mod2);
        expect(mod1->visibility).to_be(::abuild::Visibility::Public);
        expect(mod1->translationUnit).to_be(&unit);
        assert_(config.modules().size()).to_be(1U);
        expect(config.modules()[0].get()).to_be(mod1);
    });

    test("add_module_partition()", [] {
        ::abuild::Toolchain toolchain;
        ::abuild::Configuration config("release", &toolchain);
        ::abuild::ModulePartition *partition1 = config.add_module_partition("my_mod", "my_partition", nullptr, ::abuild::Visibility::Public);
        ::abuild::ModulePartition *partition2 = config.add_module_partition("my_mod", "my_other.partition", nullptr, ::abuild::Visibility::Private);

        assert_(config.modules().size()).to_be(1U);
        assert_(config.modules()[0].get()).to_be(partition1->mod);
        expect(partition1->mod->name).to_be("my_mod");
        expect(partition1->mod->visibility).to_be(::abuild::Visibility::Public);
        expect(partition1->name).to_be("my_partition");
        expect(partition1->visibility).to_be(::abuild::Visibility::Public);
        expect(partition2->mod).to_be(partition1->mod);
        expect(partition2->name).to_be("my_other.partition");
        expect(partition2->visibility).to_be(::abuild::Visibility::Private);
        expect(partition1->mod->partitions).to_be(std::vector<::abuild::ModulePartition *>{partition1, partition2});
    });

    test("add_translation_unit()", [] {
        ::abuild::Toolchain toolchain;
        ::abuild::Configuration config("release", &toolchain);

        ::abuild::Project project;
        ::abuild::SourceFile source1{
            {{"main.cpp", 0, true},
             &project}
        };
        ::abuild::SourceFile source2{
            {{"my_source.cpp", 0, true},
             &project}
        };
        ::abuild::SourceFile source3{
            {{"source.cpp", 0, true},
             &project}
        };

        ::abuild::TranslationUnit *unit1 = config.add_translation_unit(&source1);
        ::abuild::TranslationUnit *unit2 = config.add_translation_unit(&source2);
        ::abuild::TranslationUnit *unit3 = config.add_translation_unit(&source3);

        std::vector<::abuild::TranslationUnit *> units;
        units.reserve(config.translation_units().size());

        for (const std::unique_ptr<::abuild::TranslationUnit> &unit : config.translation_units())
        {
            units.push_back(unit.get());
        }

        expect(units).to_be(std::vector<::abuild::TranslationUnit *>{unit1, unit2, unit3});
    });

    test("name", [] {
        expect(::abuild::Configuration{"release", nullptr}.name()).to_be("release");
    });

    test("archiver flags", [] {
        ::abuild::Configuration config("release", nullptr);
        expect(config.archiver_flags()).to_be(std::vector<::abuild::Flag>{});

        std::vector<::abuild::Flag> flags{
            {.name = "/NOLOGO" },
            { .name = "--some_flag",
             .value = "\"a value\"",
             .affectsABI = true}
        };

        config.set_archiver_flags(flags);
        expect(config.archiver_flags()).to_be(flags);
    });

    test("compiler flags", [] {
        ::abuild::Configuration config("release", nullptr);
        expect(config.compiler_flags()).to_be(std::vector<::abuild::Flag>{});

        std::vector<::abuild::Flag> flags{
            {.name = "-c"      },
            { .name = "/O3",
             .value = {},
             .affectsABI = true}
        };

        config.set_compiler_flags(flags);
        expect(config.compiler_flags()).to_be(flags);
    });

    test("linker flags", [] {
        ::abuild::Configuration config("release", nullptr);
        expect(config.linker_flags()).to_be(std::vector<::abuild::Flag>{});

        std::vector<::abuild::Flag> flags{
            {.name = "-libc++"    },
            { .name = "-L",
             .value = "/some/path"}
        };

        config.set_linker_flags(flags);
        expect(config.linker_flags()).to_be(flags);
    });

    test("defines", [] {
        ::abuild::Configuration config("release", nullptr);

        expect(config.defines()).to_be(std::vector<::abuild::DefineToken>{});

        std::vector<::abuild::DefineToken> defines{
            {.name = "_WIN32_"},
            {                     .name = "__clang__"                },
            { .name = "SOME_DEFINE",                 .value = "1"}
        };

        config.set_defines(defines);

        expect(config.defines()).to_be(defines);
    });

    test("toolchain file extensions - windows/msvc", [] {
        ::abuild::Toolchain toolchain;
        toolchain.frontend = ::abuild::Toolchain::Frontend::MSVC;
        toolchain.abi.platform = ::abuild::ABI::Platform::Windows;
        ::abuild::Configuration config("release", &toolchain);

        ::abuild::Project project{.name = "my_project"};
        ::abuild::SourceFile source{
            {{"main.cpp", 0, true},
             &project}
        };

        expect(config.add_static_library(&project)->path.extension().string()).to_be(".lib");
        expect(config.add_dynamic_library(&project)->path.extension().string()).to_be(".dll");
        expect(config.add_executable(&project)->path.extension().string()).to_be(".exe");
        expect(config.add_translation_unit(&source)->path.extension().string()).to_be(".obj");
        expect(config.add_header_unit(&source, ::abuild::Visibility::Public)->precompiledHeaderUnit.path.extension().string()).to_be(".ifc");
        expect(config.add_module("my_module", nullptr, ::abuild::Visibility::Public)->precompiledModuleInterface.path.extension().string()).to_be(".ifc");
        expect(config.add_module_partition("my_module", "my_partition", nullptr, ::abuild::Visibility::Public)->precompiledModuleInterface.path.extension().string()).to_be(".ifc");
    });

    test("toolchain file extensions - windows/clang", [] {
        ::abuild::Toolchain toolchain;
        toolchain.frontend = ::abuild::Toolchain::Frontend::Clang;
        toolchain.abi.platform = ::abuild::ABI::Platform::Windows;
        ::abuild::Configuration config("release", &toolchain);

        ::abuild::Project project{.name = "my_project"};
        ::abuild::SourceFile source{
            {{"main.cpp", 0, true},
             &project}
        };

        expect(config.add_static_library(&project)->path.extension().string()).to_be(".lib");
        expect(config.add_dynamic_library(&project)->path.extension().string()).to_be(".dll");
        expect(config.add_executable(&project)->path.extension().string()).to_be(".exe");
        expect(config.add_translation_unit(&source)->path.extension().string()).to_be(".obj");
        expect(config.add_header_unit(&source, ::abuild::Visibility::Public)->precompiledHeaderUnit.path.extension().string()).to_be(".pcm");
        expect(config.add_module("my_module", nullptr, ::abuild::Visibility::Public)->precompiledModuleInterface.path.extension().string()).to_be(".pcm");
        expect(config.add_module_partition("my_module", "my_partition", nullptr, ::abuild::Visibility::Public)->precompiledModuleInterface.path.extension().string()).to_be(".pcm");
    });

    test("toolchain file extensions - linux/clang", [] {
        ::abuild::Toolchain toolchain;
        toolchain.frontend = ::abuild::Toolchain::Frontend::Clang;
        toolchain.abi.platform = ::abuild::ABI::Platform::Linux;
        ::abuild::Configuration config("release", &toolchain);

        ::abuild::Project project{.name = "my_project"};
        ::abuild::SourceFile source{
            {{"main.cpp", 0, true},
             &project}
        };

        expect(config.add_static_library(&project)->path.extension().string()).to_be(".a");
        expect(config.add_dynamic_library(&project)->path.extension().string()).to_be(".so");
        expect(config.add_executable(&project)->path.extension().string()).to_be("");
        expect(config.add_translation_unit(&source)->path.extension().string()).to_be(".o");
        expect(config.add_header_unit(&source, ::abuild::Visibility::Public)->precompiledHeaderUnit.path.extension().string()).to_be(".pcm");
        expect(config.add_module("my_module", nullptr, ::abuild::Visibility::Public)->precompiledModuleInterface.path.extension().string()).to_be(".pcm");
        expect(config.add_module_partition("my_module", "my_partition", nullptr, ::abuild::Visibility::Public)->precompiledModuleInterface.path.extension().string()).to_be(".pcm");
    });

    test("toolchain file extensions - unix/gcc", [] {
        ::abuild::Toolchain toolchain;
        toolchain.frontend = ::abuild::Toolchain::Frontend::GCC;
        toolchain.abi.platform = ::abuild::ABI::Platform::Linux;
        ::abuild::Configuration config("release", &toolchain);

        ::abuild::Project project{.name = "my_project"};
        ::abuild::SourceFile source{
            {{"main.cpp", 0, true},
             &project}
        };

        expect(config.add_static_library(&project)->path.extension().string()).to_be(".a");
        expect(config.add_dynamic_library(&project)->path.extension().string()).to_be(".so");
        expect(config.add_executable(&project)->path.extension().string()).to_be("");
        expect(config.add_translation_unit(&source)->path.extension().string()).to_be(".o");
        expect(config.add_header_unit(&source, ::abuild::Visibility::Public)->precompiledHeaderUnit.path.extension().string()).to_be(".cpp");
        expect(config.add_module("my_module", nullptr, ::abuild::Visibility::Public)->precompiledModuleInterface.path.extension().string()).to_be("");
        expect(config.add_module_partition("my_module", "my_partition", nullptr, ::abuild::Visibility::Public)->precompiledModuleInterface.path.extension().string()).to_be("");
    });

    test("module lookup", [] {
        ::abuild::Toolchain toolchain;
        ::abuild::Configuration config("release", &toolchain);

        config.add_module("my_mod", nullptr, ::abuild::Visibility::Public);
        ::abuild::Module *mod2 = config.add_module("some.mod", nullptr, ::abuild::Visibility::Private);
        config.add_module("astl", nullptr, ::abuild::Visibility::Public);

        expect(config.module_("some.mod")).to_be(mod2);
    });

    test("header lookup", [] {
        ::abuild::Toolchain toolchain;
        ::abuild::Configuration config("release", &toolchain);

        ::abuild::CppFile file1;
        ::abuild::CppFile file2;
        ::abuild::CppFile file3;

        config.add_header_unit(&file1, ::abuild::Visibility::Public);
        ::abuild::HeaderUnit *headerUnit = config.add_header_unit(&file2, ::abuild::Visibility::Public);
        config.add_header_unit(&file3, ::abuild::Visibility::Public);

        expect(config.header_unit(&file2)).to_be(headerUnit);
    });
});
