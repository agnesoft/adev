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
//! `Toolchain`, `defines`, compiler and
//! linker flags, detected modules etc.
export class Configuration
{
public:
    //! Constructs the `Configuration` with the
    //! `name` and `toolchain`.
    Configuration(std::string name, const Toolchain *toolchain) :
        data{.name = std::move(name), .toolchain = toolchain}
    {
    }

    //! Adds dynamic library to be created from
    //! the project.
    auto add_dynamic_library(Project *project) -> DynamicLibrary *
    {
        DynamicLibrary *lib = this->data.dynamicLibraries.emplace_back(std::make_unique<DynamicLibrary>()).get();
        lib->path = this->build_dir() / "bin" / (project->name + this->dynamic_library_suffix());
        return lib;
    }

    //! Adds executable to be created from the
    //! project.
    auto add_executable(Project *project) -> Executable *
    {
        Executable *exe = this->data.executables.emplace_back(std::make_unique<Executable>()).get();
        exe->path = this->build_dir() / "bin" / (project->name + this->executable_suffix());
        return exe;
    }

    //! Adds header unit associated with `file`.
    //! The same `file` can be added multiple
    //! times as a header unit but only one
    //! instance of the header unit will be
    //! created. The header unit can be looked up
    //! with header_unit().
    auto add_header_unit(CppFile *file) -> HeaderUnit *
    {
        HeaderUnit *unit = this->index.header_unit(file);

        if (unit == nullptr)
        {
            unit = this->data.headerUnits.emplace_back(std::make_unique<HeaderUnit>()).get();
            unit->file = file;
            unit->precompiledHeaderUnit.path = this->build_dir() / "header_units" / (file->path.filename().string() + this->precompiled_module_suffix());
            this->index.insert(unit);
        }

        return unit;
    }

    //! Adds C++ module `name` associated with
    //! `unit` with the given `visibility`. The
    //! module can be added multiple times but
    //! only one instanace of the module will be
    //! created (however the `unit` is always
    //! updated for it). The module can be looked
    //! up with module_().
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

    //! Adds C++ module partition `name` to the
    //! module `modName`, associates it with
    //! `unit` with the given `visibility`. If the
    //! module of the partition does not exist it
    //! is created (with no associated
    //! `TranslationUnit` that can be later
    //! updated with call to `add_module()`).
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

    //! Adds static library to be created from the
    //! project.
    auto add_static_library(Project *project) -> StaticLibrary *
    {
        StaticLibrary *lib = this->data.staticLibraries.emplace_back(std::make_unique<StaticLibrary>()).get();
        lib->path = this->build_dir() / "lib" / (project->name + this->static_library_suffix());
        return lib;
    }

    //! Adds translation unit for `file` to the
    //! configuration.
    auto add_translation_unit(SourceFile *file) -> TranslationUnit *
    {
        TranslationUnit *unit = this->data.translationUnits.emplace_back(std::make_unique<TranslationUnit>()).get();
        unit->sourceFile = file;
        unit->path = this->build_dir() / file->project->name / (file->path.filename().string() + this->object_file_suffix());
        return unit;
    }

    //! Returns the list of flags to be passed to all
    //! archiver calls.
    [[nodiscard]] auto archiver_flags() const noexcept -> const std::vector<Flag> &
    {
        return this->data.archiverFlags;
    }

    //! Returns the list of flags to be passed to
    //! all compiler calls.
    [[nodiscard]] auto compiler_flags() const noexcept -> const std::vector<Flag> &
    {
        return this->data.compilerFlags;
    }

    //! Returns the list of defines.
    [[nodiscard]] auto defines() const noexcept -> const std::vector<DefineToken> &
    {
        return this->data.defines;
    }

    //! Returns the added dynamic libraries.
    [[nodiscard]] auto dynamic_libraries() const noexcept -> const std::vector<std::unique_ptr<DynamicLibrary>> &
    {
        return this->data.dynamicLibraries;
    }

    //! Returns the added executables.
    [[nodiscard]] auto executables() const noexcept -> const std::vector<std::unique_ptr<Executable>> &
    {
        return this->data.executables;
    }

    //! Looks up a header unit associated with
    //! `file`. The file can be looked up in
    //! `Cache` with either exact path or partial
    //! path.
    [[nodiscard]] auto header_unit(CppFile *file) const -> HeaderUnit *
    {
        return this->index.header_unit(file);
    }

    //! Returns the added header units.
    [[nodiscard]] auto header_units() const noexcept -> const std::vector<std::unique_ptr<HeaderUnit>> &
    {
        return this->data.headerUnits;
    }

    //! Returns the list of flags to be passed to
    //! every linker call.
    [[nodiscard]] auto linker_flags() const noexcept -> const std::vector<Flag> &
    {
        return this->data.linkerFlags;
    }

    //! Looks up a `Module` by `name.
    [[nodiscard]] auto module_(const std::string &name) const -> Module * // NOLINT(readability-identifier-naming)
    {
        return this->index.module_(name);
    }

    //! Returns the added modules.
    [[nodiscard]] auto modules() const noexcept -> const std::vector<std::unique_ptr<Module>> &
    {
        return this->data.modules;
    }

    //! Returns the name of the configuration.
    [[nodiscard]] auto name() const noexcept -> const std::string &
    {
        return this->data.name;
    }

    //! Sets the flags to be passed to the
    //! archiver calls.
    auto set_archiver_flags(std::vector<Flag> flags) noexcept -> void
    {
        this->data.archiverFlags = std::move(flags);
    }

    //! Sets the defines.
    auto set_defines(std::vector<DefineToken> values) noexcept -> void
    {
        this->data.defines = std::move(values);
    }

    //! Sets the flags to be passed to the
    //! compiler calls.
    auto set_compiler_flags(std::vector<Flag> flags) noexcept -> void
    {
        this->data.compilerFlags = std::move(flags);
    }

    //! Sets the flags to be passed to the linker
    //! calls.
    auto set_linker_flags(std::vector<Flag> flags) noexcept -> void
    {
        this->data.linkerFlags = std::move(flags);
    }

    //! Returns the added static libraries.
    [[nodiscard]] auto static_libraries() const noexcept -> const std::vector<std::unique_ptr<StaticLibrary>> &
    {
        return this->data.staticLibraries;
    }

    //! Returns the toolchain of this
    //! configuration.
    [[nodiscard]] auto toolchain() const noexcept -> const Toolchain *
    {
        return this->data.toolchain;
    }

    //! Returns the added translation units.
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
