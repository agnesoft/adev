#ifndef __clang__
module abuild.cache:cache_reader;
import :cache_impl;
import yamlcpp;
#endif

namespace abuild
{
//! \private
class CacheReader
{
public:
    explicit CacheReader(const std::filesystem::path &path, CacheImpl &cache) :
        cache{cache},
        root{::YAML::LoadFile(path.string())}
    {
    }

    auto read_archiver_flags() -> void
    {
        this->cache.set_archiver_flags(CacheReader::read_flags(this->root["archiver_flags"]));
    }

    auto read_bare_header_files() -> void
    {
        const ::YAML::Node node = this->root["headers"];

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            this->read_bare_header_file(it->first.as<std::string>(), it->second);
        }
    }

    auto read_bare_source_files() -> void
    {
        const ::YAML::Node node = this->root["sources"];

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            this->read_bare_source_file(it->first.as<std::string>(), it->second);
        }
    }

    auto read_defines() -> void
    {
        this->cache.set_defines(CacheReader::read_defines(this->root["defines"]));
    }

    auto read_compiler_flags() -> void
    {
        this->cache.set_compiler_flags(CacheReader::read_flags(this->root["compiler_flags"]));
    }

    auto read_configuration_name() -> void
    {
        this->cache.set_configuration_name(this->root["name"].as<std::string>());
    }

    auto read_header_files() -> void
    {
        const ::YAML::Node node = this->root["headers"];

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            this->read_header_file(it->first.as<std::string>(), it->second);
        }
    }

    auto read_header_units() -> void
    {
        const ::YAML::Node node = this->root["headers_units"];

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            this->read_header_unit(it->first.as<std::string>(), it->second);
        }
    }

    auto read_linker_flags() -> void
    {
        this->cache.set_linker_flags(CacheReader::read_flags(this->root["linker_flags"]));
    }

    auto read_modules() -> void
    {
        const ::YAML::Node node = this->root["modules"];

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            this->read_module(it->first.as<std::string>(), it->second);
        }
    }

    auto read_project_root() -> void
    {
        this->cache.set_project_root(this->root["project_root"].as<std::string>());
    }

    auto read_projects() -> void
    {
        const ::YAML::Node node = this->root["projects"];

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            this->read_project(it->first.as<std::string>(), it->second);
        }
    }

    auto read_settings() -> void
    {
        const ::YAML::Node node = this->root["settings"];
        this->cache.set_settings(Settings{
            .projectNameSeparator = node["project_name_separator"].as<std::string>(),
            .cppHeaderExtensions = CacheReader::read_settings_list(node["cpp_header_extensions"]),
            .cppSourceExtensions = CacheReader::read_settings_list(node["cpp_source_extensions"]),
            .executableFilenames = CacheReader::read_settings_list(node["executable_filenames"]),
            .ignoreDirectories = CacheReader::read_settings_list(node["ignore_directories"]),
            .skipDirectories = CacheReader::read_settings_list(node["skip_directories"]),
            .squashDirectories = CacheReader::read_settings_list(node["squash_directories"]),
            .testDirectories = CacheReader::read_settings_list(node["test_directories"])});
    }

    auto read_source_files() -> void
    {
        const ::YAML::Node node = this->root["sources"];

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            this->read_source_file(it->first.as<std::string>(), it->second);
        }
    }

    auto read_toolchain() -> void
    {
        const ::YAML::Node node = this->root["toolchain"];
        const ::YAML::Node abi = node["abi"];
        this->cache.set_toolchain(Toolchain{
            .name = node["name"].as<std::string>(),
            .frontend = CacheReader::toolchain_frontend(node["frontend"].as<std::string>()),
            .cCompiler = node["c_compiler"].as<std::string>(),
            .cppCompiler = node["cpp_compiler"].as<std::string>(),
            .linker = node["linker"].as<std::string>(),
            .archiver = node["archiver"].as<std::string>(),
            .abi = ABI{.architecture = CacheReader::abi_architecture(abi["architecture"].as<std::string>()),
                       .bitness = CacheReader::abi_bitness(abi["bitness"].as<std::string>()),
                       .platform = CacheReader::abi_platform(abi["platform"].as<std::string>())}
        });
    }

private:
    [[nodiscard]] static auto abi_architecture(const std::string &value) -> ABI::Architecture
    {
        if (value == "ARM")
        {
            return ABI::Architecture::ARM;
        }

        return ABI::Architecture::X86;
    }

    [[nodiscard]] static auto abi_bitness(const std::string &value) -> ABI::Bitness
    {
        if (value == "X32")
        {
            return ABI::Bitness::X32;
        }

        return ABI::Bitness::X64;
    }

    [[nodiscard]] static auto abi_platform(const std::string &value) -> ABI::Platform
    {
        if (value == "Unix")
        {
            return ABI::Platform::Unix;
        }

        if (value == "Windows")
        {
            return ABI::Platform::Windows;
        }

        return ABI::Platform::Linux;
    }

    [[nodiscard]] auto find_imported_header_unit_file(const std::filesystem::path &path) const -> HeaderFile *
    {
        HeaderFile *file = this->cache.exact_header_file(path);

        if (file == nullptr)
        {
            throw std::runtime_error{"Corrupted cache: missing imported header unit file '" + path.string() + '\''};
        }

        return file;
    }

    [[nodiscard]] static auto find_imported_module_partition(const std::string &name, const Module *mod) -> ModulePartition *
    {
        for (ModulePartition *partition : mod->partitions)
        {
            if (partition->name == name)
            {
                return partition;
            }
        }

        return nullptr;
    }

    [[nodiscard]] auto header_unit_file(const std::filesystem::path &path) -> HeaderFile *
    {
        HeaderFile *file = this->cache.exact_header_file(path);

        if (file == nullptr)
        {
            throw std::runtime_error{"Corrupted cache: missing header unit file '" + path.string() + '\''};
        }

        return file;
    }

    [[nodiscard]] auto imported_header_units(const ::YAML::Node &node) const -> std::vector<HeaderUnit *>
    {
        std::vector<HeaderUnit *> units;
        units.reserve(node.size());

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            units.push_back(this->read_imported_header_unit(it->as<std::string>()));
        }

        return units;
    }

    [[nodiscard]] auto imported_module_partition(const std::string &names) const -> ModulePartition *
    {
        const std::pair<std::string, std::string> modPartition = CacheReader::module_partition_names(names);
        Module *mod = this->imported_module_partition_module(modPartition.first);
        return CacheReader::imported_module_partition(modPartition.second, mod);
    }

    [[nodiscard]] static auto imported_module_partition(const std::string &name, const Module *mod) -> ModulePartition *
    {
        ModulePartition *partition = CacheReader::find_imported_module_partition(name, mod);

        if (partition == nullptr)
        {
            throw std::runtime_error{"Corrupted cache: missing imported module partition '" + mod->name + ':' + name + '\''};
        }

        return partition;
    }

    [[nodiscard]] auto imported_module_partitions(const ::YAML::Node &node) const -> std::vector<ModulePartition *>
    {
        std::vector<ModulePartition *> partitions;
        partitions.reserve(node.size());

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            partitions.push_back(this->imported_module_partition(it->as<std::string>()));
        }

        return partitions;
    }

    [[nodiscard]] auto imported_module_partition_module(const std::string &name) const -> Module *
    {
        Module *mod = this->cache.module_(name);

        if (mod == nullptr)
        {
            throw std::runtime_error{"Corrupted cache: missing imported module '" + name + '\''};
        }

        return mod;
    }

    [[nodiscard]] static auto module_partition_names(const std::string &value) -> std::pair<std::string, std::string>
    {
        const std::size_t split = value.find(':');
        return {value.substr(0, split), value.substr(split + 1)};
    }

    [[nodiscard]] auto imported_module(const std::string &name) const -> Module *
    {
        Module *mod = this->cache.module_(name);

        if (mod == nullptr)
        {
            throw std::runtime_error{"Corrupted cache: missing imported module '" + name + '\''};
        }

        return mod;
    }

    [[nodiscard]] auto imported_modules(const ::YAML::Node &node) const -> std::vector<Module *>
    {
        std::vector<Module *> modules;
        modules.reserve(node.size());

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            modules.push_back(this->imported_module(it->as<std::string>()));
        }

        return modules;
    }

    [[nodiscard]] auto include(const std::filesystem::path &path, const ::YAML::Node &node) const -> Header
    {
        return {.file = this->include_file(path),
                .includes = this->read_includes(node["includes"]),
                .importedHeaderUnits = this->read_imported_header_units(node["imported_header_units"]),
                .importedModules = this->read_imported_modules(node["imported_modules"]),
                .importedModulePartitions = this->read_imported_module_partitions(node["imported_module_partitions"])};
    }

    [[nodiscard]] auto include_file(const std::filesystem::path &path) const -> CppFile *
    {
        CppFile *file = this->include_header_or_source_file(path);

        if (file == nullptr)
        {
            throw std::runtime_error("Corrupted cache: missing included file '" + path.string() + '\'');
        }

        return file;
    }

    [[nodiscard]] auto include_header_or_source_file(const std::filesystem::path &path) const -> CppFile *
    {
        CppFile *file = this->cache.exact_header_file(path);

        if (file == nullptr)
        {
            file = this->cache.exact_source_file(path);
        }

        return file;
    }

    [[nodiscard]] auto includes(const ::YAML::Node &node) const -> std::vector<Header>
    {
        std::vector<Header> includes;
        includes.reserve(node.size());

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            includes.emplace_back(this->include(it->first.as<std::string>(), it->second));
        }

        return includes;
    }

    [[nodiscard]] auto module_partition(const std::string &name, const ::YAML::Node &node, Module *mod) -> ModulePartition *
    {
        ModulePartition *partition = this->cache.add_module_partition(name);
        partition->mod = mod;
        partition->sourceFile = this->cache.exact_source_file(node["source_file"].as<std::string>());
        partition->exported = node["exported"].as<bool>();
        CacheReader::read_file(partition->precompiledModuleInterface, node["precompiled_module_interface"]);
        return partition;
    }

    [[nodiscard]] auto module_partitions(const ::YAML::Node &node, Module *mod) -> std::vector<ModulePartition *>
    {
        std::vector<ModulePartition *> partitions;
        partitions.reserve(node.size());

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            partitions.push_back(this->module_partition(it->first.as<std::string>(), it->second, mod));
        }

        return partitions;
    }

    [[nodiscard]] auto project_header(const std::filesystem::path &path, Project *project) const -> HeaderFile *
    {
        HeaderFile *file = this->cache.exact_header_file(path);

        if (file == nullptr)
        {
            throw std::runtime_error{"Corrupted cache: missing project header file '" + path.string() + '\''};
        }

        file->project = project;
        return file;
    }

    [[nodiscard]] auto project_headers(const ::YAML::Node &node, Project *project) const -> std::vector<HeaderFile *>
    {
        std::vector<HeaderFile *> files;
        files.reserve(node.size());

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            files.emplace_back(this->project_header(it->as<std::string>(), project));
        }

        return files;
    }

    [[nodiscard]] auto project_source(const std::filesystem::path &path, Project *project) const -> SourceFile *
    {
        SourceFile *file = this->cache.exact_source_file(path);

        if (file == nullptr)
        {
            throw std::runtime_error{"Corrupted cache: missing project source file '" + path.string() + '\''};
        }

        file->project = project;
        return file;
    }

    [[nodiscard]] auto project_sources(const ::YAML::Node &node, Project *project) const -> std::vector<SourceFile *>
    {
        std::vector<SourceFile *> files;
        files.reserve(node.size());

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            files.push_back(this->project_source(it->as<std::string>(), project));
        }

        return files;
    }

    [[nodiscard]] static auto project_type(const std::string &value) -> Project::Type
    {
        if (value == "Executable")
        {
            return Project::Type::Executable;
        }

        if (value == "DynamicLibrary")
        {
            return Project::Type::DynamicLibrary;
        }

        return Project::Type::StaticLibrary;
    }

    auto read_bare_header_file(const std::filesystem::path &path, const ::YAML::Node &node) -> void
    {
        HeaderFile *file = this->cache.add_header_file(path);
        file->timestamp = node["timestamp"].as<std::size_t>();
        file->tokens = CacheReader::read_tokens(node["tokens"]);
    }

    auto read_bare_source_file(std::filesystem::path path, const ::YAML::Node &node) -> void
    {
        SourceFile *file = this->cache.add_source_file(std::move(path));
        file->timestamp = node["timestamp"].as<std::size_t>();
        file->tokens = CacheReader::read_tokens(node["tokens"]);
        CacheReader::read_file(file->objectFile, node["object_file"]);
    }

    auto read_cppfile(CppFile *file, const ::YAML::Node &node) -> void
    {
        file->includes = this->read_includes(node["includes"]);
        file->importedHeaderUnits = this->read_imported_header_units(node["imported_header_units"]);
        file->importedModules = this->read_imported_modules(node["imported_modules"]);
        file->importedModulePartitions = this->read_imported_module_partitions(node["imported_module_partitions"]);
    }

    [[nodiscard]] static auto read_defines(const ::YAML::Node &&node) -> std::vector<DefineToken>
    {
        std::vector<DefineToken> defines;
        defines.reserve(node.size());

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            defines.emplace_back(DefineToken{.name = it->first.as<std::string>(), .value = it->second.as<std::string>()});
        }

        return defines;
    }

    static auto read_file(File &file, const ::YAML::Node &&node) -> void
    {
        file.path = node["path"].as<std::string>();
        file.timestamp = node["timestamp"].as<std::size_t>();
    }

    [[nodiscard]] static auto read_flags(const ::YAML::Node &&node) -> std::vector<Flag>
    {
        std::vector<Flag> flags;
        flags.reserve(node.size());

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            flags.emplace_back(Flag{.name = it->first.as<std::string>(), .value = it->second.as<std::string>()});
        }

        return flags;
    }

    auto read_header_file(const std::filesystem::path &path, const ::YAML::Node &node) -> void
    {
        this->read_cppfile(this->cache.exact_header_file(path), node);
    }

    auto read_header_unit(const std::filesystem::path &path, const ::YAML::Node &node) -> void
    {
        HeaderFile *file = this->header_unit_file(path);
        HeaderUnit *unit = this->cache.add_header_unit(file);
        CacheReader::read_file(unit->precompiledHeaderUnit, node["precompiled_header_unit"]);
    }

    [[nodiscard]] auto read_imported_header_unit(const std::filesystem::path &path) const -> HeaderUnit *
    {
        HeaderUnit *unit = this->cache.header_unit(this->find_imported_header_unit_file(path));

        if (unit == nullptr)
        {
            throw std::runtime_error{"Corrupted cache: missing imported header unit '" + path.string() + '\''};
        }

        return unit;
    }

    [[nodiscard]] auto read_imported_header_units(const ::YAML::Node &&node) const -> std::vector<HeaderUnit *>
    {
        if (!node.IsDefined())
        {
            return {};
        }

        return this->imported_header_units(node);
    }

    [[nodiscard]] auto read_imported_module_partitions(const ::YAML::Node &&node) const -> std::vector<ModulePartition *>
    {
        if (!node.IsDefined())
        {
            return {};
        }

        return this->imported_module_partitions(node);
    }

    [[nodiscard]] auto read_imported_modules(const ::YAML::Node &&node) const -> std::vector<Module *>
    {
        if (!node.IsDefined())
        {
            return {};
        }

        return this->imported_modules(node);
    }

    [[nodiscard]] auto read_includes(const ::YAML::Node &&node) const -> std::vector<Header>
    {
        if (!node.IsDefined())
        {
            return {};
        }

        return this->includes(node);
    }

    auto read_module(const std::string &name, const ::YAML::Node &node) -> void
    {
        Module *mod = this->cache.add_module(name);
        mod->sourceFile = this->cache.exact_source_file(node["source_file"].as<std::string>());
        mod->exported = node["exported"].as<bool>();
        CacheReader::read_file(mod->precompiledModuleInterface, node["precompiled_module_interface"]);
        mod->partitions = this->read_module_partitions(node["partitions"], mod);
    }

    [[nodiscard]] auto read_module_partitions(const ::YAML::Node &&node, Module *mod) -> std::vector<ModulePartition *>
    {
        if (!node.IsDefined())
        {
            return {};
        }

        return this->module_partitions(node, mod);
    }

    auto read_project(std::string name, const ::YAML::Node &node) -> void
    {
        Project *project = this->cache.add_project(std::move(name));
        project->type = CacheReader::project_type(node["type"].as<std::string>());
        CacheReader::read_file(project->linkedFile, node["linked_file"]);
        project->headers = this->read_project_headers(node["headers"], project);
        project->sources = this->read_project_sources(node["sources"], project);
    }

    [[nodiscard]] auto read_project_headers(const ::YAML::Node &&node, Project *project) const -> std::vector<HeaderFile *>
    {
        if (!node.IsDefined())
        {
            return {};
        }

        return this->project_headers(node, project);
    }

    [[nodiscard]] auto read_project_sources(const ::YAML::Node &&node, Project *project) const -> std::vector<SourceFile *>
    {
        if (!node.IsDefined())
        {
            return {};
        }

        return this->project_sources(node, project);
    }

    [[nodiscard]] static auto read_settings_list(const ::YAML::Node &&node) -> std::unordered_set<std::string>
    {
        std::unordered_set<std::string> list;

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            list.insert(it->as<std::string>());
        }

        return list;
    }

    auto read_source_file(const std::filesystem::path &path, const ::YAML::Node &node) -> void
    {
        this->read_cppfile(this->cache.exact_source_file(path), node);
    }

    [[nodiscard]] static auto read_tokens(const ::YAML::Node &node) -> std::vector<Token>
    {
        std::stringstream stream;

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            stream << (*it).as<std::string>() << '\n';
        }

        return ::abuild::tokenize(stream.str());
    }

    [[nodiscard]] static auto toolchain_frontend(const std::string &value) -> Toolchain::Frontend
    {
        if (value == "Clang")
        {
            return Toolchain::Frontend::Clang;
        }

        if (value == "MSVC")
        {
            return Toolchain::Frontend::MSVC;
        }

        return Toolchain::Frontend::GCC;
    }

    CacheImpl &cache;
    ::YAML::Node root;
};

//! \private
auto read_cache(const std::filesystem::path &path, CacheImpl &cache) -> void
{
    CacheReader reader{path, cache};
    reader.read_configuration_name();
    reader.read_project_root();
    reader.read_settings();
    reader.read_toolchain();
    reader.read_compiler_flags();
    reader.read_linker_flags();
    reader.read_archiver_flags();
    reader.read_defines();
    reader.read_bare_header_files();
    reader.read_bare_source_files();
    reader.read_projects();
    reader.read_modules();
    reader.read_header_units();
    reader.read_header_files();
    reader.read_source_files();
}
}