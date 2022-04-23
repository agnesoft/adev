#ifndef __clang__
export module abuild.cache:configuration;
export import :configuration_data;
export import :project;
export import :settings;
import :configuration_index;
#endif

namespace abuild
{
//! The `Configuration` represents the build
//! configuration. That is the combination of
//! `Toolchain`, custom `defines` and compiler and
//! linker flags.
export class Configuration
{
public:
    Configuration(std::string name, const Toolchain *toolchain) :
        data{.name = std::move(name), .toolchain = toolchain}
    {
    }

    auto add_dynamic_library(Project *project) -> DynamicLibrary *
    {
        DynamicLibrary *lib = this->data.dynamicLibraries.emplace_back(std::make_unique<DynamicLibrary>()).get();
        lib->path = this->build_dir() / "bin" / (project->name + this->dynamic_library_suffix());
        return lib;
    }

    auto add_executable(Project *project) -> Executable *
    {
        Executable *exe = this->data.executables.emplace_back(std::make_unique<Executable>()).get();
        exe->path = this->build_dir() / "bin" / (project->name + this->executable_suffix());
        return exe;
    }

    auto add_header_unit(CppFile *file, Visibility visibility) -> HeaderUnit *
    {
        HeaderUnit *unit = this->index.header_unit(file);

        if (unit == nullptr)
        {
            unit = this->data.headerUnits.emplace_back(std::make_unique<HeaderUnit>()).get();
            unit->file = file;
            unit->precompiledHeaderUnit.path = this->build_dir() / "header_units" / (file->path.filename().string() + this->precompiled_module_suffix());
            unit->visibility = visibility;
            this->index.insert(unit);
        }

        return unit;
    }

    auto add_module(const std::string &name, TranslationUnit *unit, Visibility visibility) -> Module *
    {
        Module *mod = this->index.module_(name);

        if (mod == nullptr)
        {
            mod = this->data.modules.emplace_back(std::make_unique<Module>()).get();
            mod->name = name;
            mod->precompiledModuleInterface.path = this->build_dir() / "modules" / (name + this->precompiled_module_suffix());
            mod->visibility = visibility;
            this->index.insert(mod);
        }

        mod->translationUnit = unit;
        return mod;
    }

    auto add_module_partition(const std::string &modName, const std::string &name, TranslationUnit *unit, Visibility visibility) -> ModulePartition *
    {
        Module *mod = this->index.module_(modName);

        if (mod == nullptr)
        {
            mod = this->add_module(modName, nullptr, visibility);
        }

        ModulePartition *partition = this->data.modulePartitions.emplace_back(std::make_unique<ModulePartition>()).get();
        partition->name = name;
        partition->precompiledModuleInterface.path = this->build_dir() / "modules" / (modName + '-' + name + this->precompiled_module_suffix());
        partition->mod = mod;
        partition->translationUnit = unit;
        partition->visibility = visibility;
        mod->partitions.push_back(partition);
        return partition;
    }

    auto add_static_library(Project *project) -> StaticLibrary *
    {
        StaticLibrary *lib = this->data.staticLibraries.emplace_back(std::make_unique<StaticLibrary>()).get();
        lib->path = this->build_dir() / "lib" / (project->name + this->static_library_suffix());
        return lib;
    }

    auto add_translation_unit(SourceFile *file) -> TranslationUnit *
    {
        TranslationUnit *unit = this->data.translationUnits.emplace_back(std::make_unique<TranslationUnit>()).get();
        unit->sourceFile = file;
        unit->path = this->build_dir() / file->project->name / (file->path.filename().string() + this->object_file_suffix());
        return unit;
    }

    [[nodiscard]] auto archiver_flags() const noexcept -> const std::vector<Flag> &
    {
        return this->data.archiverFlags;
    }

    [[nodiscard]] auto compiler_flags() const noexcept -> const std::vector<Flag> &
    {
        return this->data.compilerFlags;
    }

    [[nodiscard]] auto defines() const noexcept -> const std::vector<DefineToken> &
    {
        return this->data.defines;
    }

    [[nodiscard]] auto dynamic_libraries() const noexcept -> const std::vector<std::unique_ptr<DynamicLibrary>> &
    {
        return this->data.dynamicLibraries;
    }

    [[nodiscard]] auto executables() const noexcept -> const std::vector<std::unique_ptr<Executable>> &
    {
        return this->data.executables;
    }

    [[nodiscard]] auto header_unit(CppFile *file) const -> HeaderUnit *
    {
        return this->index.header_unit(file);
    }

    [[nodiscard]] auto header_units() const noexcept -> const std::vector<std::unique_ptr<HeaderUnit>> &
    {
        return this->data.headerUnits;
    }

    [[nodiscard]] auto linker_flags() const noexcept -> const std::vector<Flag> &
    {
        return this->data.linkerFlags;
    }

    [[nodiscard]] auto module_(const std::string &name) const -> Module *
    {
        return this->index.module_(name);
    }

    [[nodiscard]] auto modules() const noexcept -> const std::vector<std::unique_ptr<Module>> &
    {
        return this->data.modules;
    }

    [[nodiscard]] auto name() const noexcept -> const std::string &
    {
        return this->data.name;
    }

    auto set_archiver_flags(std::vector<Flag> flags) noexcept -> void
    {
        this->data.archiverFlags = std::move(flags);
    }

    auto set_defines(std::vector<DefineToken> values) noexcept -> void
    {
        this->data.defines = std::move(values);
    }

    auto set_compiler_flags(std::vector<Flag> flags) noexcept -> void
    {
        this->data.compilerFlags = std::move(flags);
    }

    auto set_linker_flags(std::vector<Flag> flags) noexcept -> void
    {
        this->data.linkerFlags = std::move(flags);
    }

    [[nodiscard]] auto static_libraries() const noexcept -> const std::vector<std::unique_ptr<StaticLibrary>> &
    {
        return this->data.staticLibraries;
    }

    [[nodiscard]] auto toolchain() const noexcept -> const Toolchain *
    {
        return this->data.toolchain;
    }

    [[nodiscard]] auto translation_units() const noexcept -> const std::vector<std::unique_ptr<TranslationUnit>> &
    {
        return this->data.translationUnits;
    }

private:
    [[nodiscard]] auto build_dir() const -> std::filesystem::path
    {
        return std::filesystem::path{"build"} / this->data.toolchain->name / this->data.name;
    }

    [[nodiscard]] auto executable_suffix() const -> std::string
    {
        if (this->data.toolchain->abi.platform == ABI::Platform::Windows)
        {
            return ".exe";
        }

        return "";
    }

    [[nodiscard]] auto dynamic_library_suffix() const -> std::string
    {
        if (this->data.toolchain->abi.platform == ABI::Platform::Windows)
        {
            return ".dll";
        }

        return ".so";
    }

    [[nodiscard]] auto object_file_suffix() const -> std::string
    {
        if (this->data.toolchain->abi.platform == ABI::Platform::Windows)
        {
            return ".obj";
        }

        return ".o";
    }

    [[nodiscard]] auto precompiled_module_suffix() const -> std::string
    {
        if (this->data.toolchain->frontend == Toolchain::Frontend::Clang)
        {
            return ".pcm";
        }

        if (this->data.toolchain->frontend == Toolchain::Frontend::MSVC)
        {
            return ".ifc";
        }

        return "";
    }

    [[nodiscard]] auto static_library_suffix() const -> std::string
    {
        if (this->data.toolchain->abi.platform == ABI::Platform::Windows)
        {
            return ".lib";
        }

        return ".a";
    }

    ConfigurationData data;
    ConfigurationIndex index;
};
}
