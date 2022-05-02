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
            HeaderFile *file = this->cache.add_header_file(it->first.as<std::string>());
            file->timestamp = it->second["timestamp"].as<std::size_t>();
            file->tokens = CacheReader::read_tokens(it->second["tokens"]);
        }
    }

    auto read_bare_source_files() -> void
    {
        const ::YAML::Node node = this->root["sources"];

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            SourceFile *file = this->cache.add_source_file(it->first.as<std::string>());
            file->timestamp = it->second["timestamp"].as<std::size_t>();
            file->tokens = CacheReader::read_tokens(it->second["tokens"]);

            const ::YAML::Node objectFileNode = it->second["object_file"];
            file->objectFile.path = objectFileNode["path"].as<std::string>();
            file->objectFile.timestamp = objectFileNode["timestamp"].as<std::size_t>();
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
            HeaderFile *file = this->cache.exact_header_file(it->first.as<std::string>());
            file->includes = this->read_includes(it->second["includes"]);
            file->importedHeaderUnits = this->read_imported_header_units(it->second["imported_header_units"]);
            file->importedModules = this->read_imported_modules(it->second["imported_modules"]);
            file->importedModulePartitions = this->read_imported_module_partitions(it->second["imported_module_partitions"]);
        }
    }

    auto read_header_units() -> void
    {
        const ::YAML::Node node = this->root["headers_units"];

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            HeaderFile *file = this->cache.exact_header_file(it->first.as<std::string>());

            if (file == nullptr)
            {
                throw std::runtime_error{"Corrupted cache: missing header unit file '" + it->first.as<std::string>() + '\''};
            }

            HeaderUnit *unit = this->cache.add_header_unit(file);

            const ::YAML::Node precompiledHeaderUnit = it->second["precompiled_header_unit"];
            unit->precompiledHeaderUnit.path = precompiledHeaderUnit["path"].as<std::string>();
            unit->precompiledHeaderUnit.timestamp = precompiledHeaderUnit["timestamp"].as<std::size_t>();
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
            Module *mod = this->cache.add_module(it->first.as<std::string>());
            mod->sourceFile = this->cache.exact_source_file(it->second["source_file"].as<std::string>());
            mod->exported = it->second["exported"].as<bool>();

            const ::YAML::Node precompiledInterfaceNode = it->second["precompiled_module_interface"];
            mod->precompiledModuleInterface.path = precompiledInterfaceNode["path"].as<std::string>();
            mod->precompiledModuleInterface.timestamp = precompiledInterfaceNode["timestamp"].as<std::size_t>();

            mod->partitions = this->read_module_partitions(it->second["partitions"], mod);
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
            Project *project = this->cache.add_project(it->first.as<std::string>());
            project->type = CacheReader::project_type(it->second["type"].as<std::string>());

            const ::YAML::Node linkedFile = it->second["linked_file"];
            project->linkedFile.path = linkedFile["path"].as<std::string>();
            project->linkedFile.timestamp = linkedFile["timestamp"].as<std::size_t>();

            project->headers = this->read_project_headers(it->second["headers"], project);
            project->sources = this->read_project_sources(it->second["sources"], project);
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
            SourceFile *file = this->cache.exact_source_file(it->first.as<std::string>());
            file->includes = this->read_includes(it->second["includes"]);
            file->importedHeaderUnits = this->read_imported_header_units(it->second["imported_header_units"]);
            file->importedModules = this->read_imported_modules(it->second["imported_modules"]);
            file->importedModulePartitions = this->read_imported_module_partitions(it->second["imported_module_partitions"]);
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

    [[nodiscard]] static auto module_partition_names(const std::string &value) -> std::pair<std::string, std::string>
    {
        const std::size_t split = value.find(':');
        return {value.substr(0, split), value.substr(split + 1)};
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

    [[nodiscard]] auto read_imported_header_units(const ::YAML::Node &&node) const -> std::vector<HeaderUnit *>
    {
        if (!node.IsDefined())
        {
            return {};
        }

        std::vector<HeaderUnit *> units;
        units.reserve(node.size());

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            HeaderFile *file = this->cache.exact_header_file(it->as<std::string>());

            if (file == nullptr)
            {
                throw std::runtime_error{"Corrupted cache: missing imported header unit file '" + it->as<std::string>() + '\''};
            }

            HeaderUnit *unit = this->cache.header_unit(file);

            if (unit == nullptr)
            {
                throw std::runtime_error{"Corrupted cache: missing imported header unit '" + it->as<std::string>() + '\''};
            }

            units.push_back(unit);
        }

        return units;
    }

    [[nodiscard]] auto read_imported_module_partitions(const ::YAML::Node &&node) const -> std::vector<ModulePartition *>
    {
        if (!node.IsDefined())
        {
            return {};
        }

        std::vector<ModulePartition *> partitions;
        partitions.reserve(node.size());

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            const std::pair<std::string, std::string> modPartition = CacheReader::module_partition_names(it->as<std::string>());
            Module *mod = this->cache.module_(modPartition.first);

            if (mod == nullptr)
            {
                throw std::runtime_error{"Corrupted cache: missing imported module '" + modPartition.first + '\''};
            }

            ModulePartition *partition = nullptr;

            for (ModulePartition *candidate : mod->partitions)
            {
                if (candidate->name == modPartition.second)
                {
                    partition = candidate;
                    break;
                }
            }

            if (partition == nullptr)
            {
                throw std::runtime_error{"Corrupted cache: missing imported module partition '" + it->as<std::string>() + '\''};
            }
            else
            {
                partitions.push_back(partition);
            }
        }

        return partitions;
    }

    [[nodiscard]] auto read_imported_modules(const ::YAML::Node &&node) const -> std::vector<Module *>
    {
        if (!node.IsDefined())
        {
            return {};
        }

        std::vector<Module *> modules;
        modules.reserve(node.size());

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            Module *mod = this->cache.module_(it->as<std::string>());

            if (mod == nullptr)
            {
                throw std::runtime_error{"Corrupted cache: missing imported module '" + it->as<std::string>() + '\''};
            }

            modules.push_back(mod);
        }

        return modules;
    }

    [[nodiscard]] auto read_includes(const ::YAML::Node &&node) const -> std::vector<Header>
    {
        if (!node.IsDefined())
        {
            return {};
        }

        std::vector<Header> includes;
        includes.reserve(node.size());

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            const std::filesystem::path path = it->first.as<std::string>();
            CppFile *file = this->cache.exact_header_file(path);

            if (file == nullptr)
            {
                file = this->cache.exact_source_file(path);

                if (file == nullptr)
                {
                    throw std::runtime_error("Corrupted cache: missing included file '" + path.string() + '\'');
                }
            }

            includes.emplace_back(Header{.file = file,
                                         .includes = this->read_includes(it->second["includes"]),
                                         .importedHeaderUnits = this->read_imported_header_units(it->second["imported_header_units"]),
                                         .importedModules = this->read_imported_modules(it->second["imported_modules"]),
                                         .importedModulePartitions = this->read_imported_module_partitions(it->second["imported_module_partitions"])});
        }

        return includes;
    }

    [[nodiscard]] auto read_module_partitions(const ::YAML::Node &&node, Module *mod) -> std::vector<ModulePartition *>
    {
        if (!node.IsDefined())
        {
            return {};
        }

        std::vector<ModulePartition *> partitions;
        partitions.reserve(node.size());

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            ModulePartition *partition = this->cache.add_module_partition(it->first.as<std::string>());
            partition->mod = mod;
            partition->sourceFile = this->cache.exact_source_file(it->second["source_file"].as<std::string>());
            partition->exported = it->second["exported"].as<bool>();

            const ::YAML::Node precompiledInterfaceNode = it->second["precompiled_module_interface"];
            partition->precompiledModuleInterface.path = precompiledInterfaceNode["path"].as<std::string>();
            partition->precompiledModuleInterface.timestamp = precompiledInterfaceNode["timestamp"].as<std::size_t>();

            partitions.push_back(partition);
        }

        return partitions;
    }

    [[nodiscard]] auto read_project_headers(const ::YAML::Node &&node, Project *project) const -> std::vector<HeaderFile *>
    {
        if (!node.IsDefined())
        {
            return {};
        }

        std::vector<HeaderFile *> files;
        files.reserve(node.size());

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            HeaderFile *file = this->cache.exact_header_file(it->as<std::string>());

            if (file == nullptr)
            {
                throw std::runtime_error("Corrupted cache: missing project header file '" + it->as<std::string>() + '\'');
            }

            file->project = project;
            files.push_back(file);
        }

        return files;
    }

    [[nodiscard]] auto read_project_sources(const ::YAML::Node &&node, Project *project) const -> std::vector<SourceFile *>
    {
        if (!node.IsDefined())
        {
            return {};
        }

        std::vector<SourceFile *> files;
        files.reserve(node.size());

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            SourceFile *file = this->cache.exact_source_file(it->as<std::string>());

            if (file == nullptr)
            {
                throw std::runtime_error("Corrupted cache: missing project source file '" + it->as<std::string>() + '\'');
            }

            file->project = project;
            files.push_back(file);
        }

        return files;
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