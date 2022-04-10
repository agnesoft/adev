#ifndef __clang__
export module abuild.cache:configuration;
export import :configuration_data;
export import :project;
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
        std::unique_ptr<DynamicLibrary> lib = std::make_unique<DynamicLibrary>();
        lib->path = this->build_dir() / "bin" / (project->name + this->dynamic_library_suffix());

        return this->data.dynamicLibraries.emplace_back(std::move(lib)).get();
    }

    auto add_executable(Project *project) -> Executable *
    {
        std::unique_ptr<Executable> exe = std::make_unique<Executable>();
        exe->path = this->build_dir() / "bin" / (project->name + this->executable_suffix());

        return this->data.executables.emplace_back(std::move(exe)).get();
    }

    auto add_header_unit(CppFile *file, Visibility visibility) -> HeaderUnit *
    {
        HeaderUnit *unit = this->index.header_unit(file);

        if (unit == nullptr)
        {
            std::unique_ptr<HeaderUnit> header = std::make_unique<HeaderUnit>();
            header->file = file;
            header->precompiledHeaderUnit.path = this->build_dir() / "header_units" / (file->path.filename().string() + this->precompiled_module_suffix());
            header->visibility = visibility;
            unit = this->data.headerUnits.emplace_back(std::move(header)).get();
        }

        return unit;
    }

    auto add_module(std::string name, TranslationUnit *unit, Visibility visibility) -> Module *
    {
        Module *modPtr = this->index.module_(name);

        if (modPtr == nullptr)
        {
            std::unique_ptr<Module> mod = std::make_unique<Module>();
            mod->name = std::move(name);
            mod->precompiledModuleInterface.path = this->build_dir() / "modules" / (name + this->precompiled_module_suffix());
            mod->visibility = visibility;
            modPtr = this->data.modules.emplace_back(std::move(mod)).get();
            this->index.insert(modPtr);
        }

        modPtr->translationUnit = unit;
        return modPtr;
    }

    auto add_module_partition(const std::string &mod, std::string name, TranslationUnit *unit, Visibility visibility) -> ModulePartition *
    {
        Module *modPtr = this->index.module_(mod);

        if (modPtr == nullptr)
        {
            modPtr = this->add_module(mod, nullptr, visibility);
        }

        std::unique_ptr<ModulePartition> partition = std::make_unique<ModulePartition>();
        partition->name = std::move(name);
        partition->precompiledModuleInterface.path = this->build_dir() / "modules" / (mod + '-' + name + this->precompiled_module_suffix());
        partition->mod = modPtr;
        partition->translationUnit = unit;
        partition->visibility = visibility;

        ModulePartition *partitionPtr = this->data.modulePartitions.emplace_back(std::move(partition)).get();
        modPtr->partitions.push_back(partitionPtr);
        this->index.insert(partitionPtr);
        return partitionPtr;
    }

    auto add_static_library(Project *project) -> StaticLibrary *
    {
        auto lib = std::make_unique<StaticLibrary>();
        lib->path = this->build_dir() / "lib" / (project->name + this->dynamic_library_suffix());

        return this->data.staticLibraries.emplace_back(std::move(lib)).get();
    }

    auto add_translation_unit(SourceFile *file) -> TranslationUnit *
    {
        auto unit = std::make_unique<TranslationUnit>();
        unit->sourceFile = file;
        unit->path = this->build_dir() / file->project->name / (file->path.filename().string() + this->object_file_suffix());

        return this->data.translationUnits.emplace_back(std::move(unit)).get();
    }

    [[nodiscard]] auto name() const noexcept -> const std::string &
    {
        return this->data.name;
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

    [[nodiscard]] auto header_units() const noexcept -> const std::vector<std::unique_ptr<HeaderUnit>> &
    {
        return this->data.headerUnits;
    }

    [[nodiscard]] auto linker_flags() const noexcept -> const std::vector<Flag> &
    {
        return this->data.linkerFlags;
    }

    [[nodiscard]] auto modules() const noexcept -> const std::vector<std::unique_ptr<Module>> &
    {
        return this->data.modules;
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
        if (this->data.toolchain->abi.frontend == ABI::Frontend::Clang)
        {
            return ".pcm";
        }

        if (this->data.toolchain->abi.frontend == ABI::Frontend::MSVC)
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
