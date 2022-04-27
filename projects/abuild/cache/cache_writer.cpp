#ifndef __clang__
module abuild.cache:cache_writer;
import :cache_data;
import yamlcpp;
#endif

namespace abuild
{
//! \private
class CacheWriter
{
public:
    auto save_configurations(const std::vector<std::unique_ptr<Configuration>> &configurations)
    {
        ::YAML::Node node = CacheWriter::ensure_node(root["configurations"]);

        for (const std::unique_ptr<Configuration> &configuration : configurations)
        {
            CacheWriter::save_configuration(configuration.get(), node);
        }
    }

    auto save_headers(const std::vector<std::unique_ptr<HeaderFile>> &headers) -> void
    {
        CacheWriter::save_files(headers, CacheWriter::ensure_node(root["headers"]));
    }

    auto save_sources(const std::vector<std::unique_ptr<SourceFile>> &sources) -> void
    {
        CacheWriter::save_files(sources, CacheWriter::ensure_node(root["sources"]));
    }

    auto save_toolchains(const std::vector<std::unique_ptr<Toolchain>> &toolchains)
    {
        ::YAML::Node node = CacheWriter::ensure_node(root["toolchains"]);

        for (const std::unique_ptr<Toolchain> &toolchain : toolchains)
        {
            CacheWriter::save_toolchain(toolchain.get(), node);
        }
    }

    auto save_to_file(const std::filesystem::path &path) -> void
    {
        std::ofstream{path} << this->root;
    }

private:
    [[nodiscard]] static auto ensure_node(::YAML::Node &&node) -> ::YAML::Node &&
    {
        return std::move(node = {});
    }

    static auto save_abi(const ABI &abi, ::YAML::Node &&node) -> void
    {
        node["architecture"] = CacheWriter::to_string(abi.architecture);
        node["bitness"] = CacheWriter::to_string(abi.bitness);
        node["platform"] = CacheWriter::to_string(abi.platform);
    }

    static auto save_configuration(const Configuration *configuration, ::YAML::Node &node) -> void
    {
        ::YAML::Node configurationNode = CacheWriter::ensure_node(node[configuration->name()]);
        configurationNode["toolchain"] = configuration->toolchain()->name;
        CacheWriter::save_executables(configuration->executables(), configurationNode);
        CacheWriter::save_dynamic_libraries(configuration->dynamic_libraries(), configurationNode);
        CacheWriter::save_static_libraries(configuration->static_libraries(), configurationNode);
        CacheWriter::save_header_units(configuration->header_units(), configurationNode);
        CacheWriter::save_modules(configuration->modules(), configurationNode);
        CacheWriter::save_translation_units(configuration->translation_units(), configurationNode);
    }

    static auto save_executables(const std::vector<std::unique_ptr<Executable>> &executables, ::YAML::Node &node) -> void
    {
        ::YAML::Node executablesNode = CacheWriter::ensure_node(node["executables"]);

        for (const std::unique_ptr<Executable> &executable : executables)
        {
            executablesNode.push_back(executable->project->name);
        }
    }

    static auto save_file(const auto &file, ::YAML::Node &node) -> void
    {
        ::YAML::Node fileNode = CacheWriter::ensure_node(node[file->path.string()]);
        fileNode["timestamp"] = file->timestamp;
        fileNode["project"] = file->project->name;
        CacheWriter::save_tokens(file->tokens, fileNode["tokens"]);
    }

    static auto save_files(const auto &files, ::YAML::Node &&node) -> void
    {
        for (auto &file : files)
        {
            CacheWriter::save_file(file, node);
        }
    }

    static auto save_dynamic_libraries(const std::vector<std::unique_ptr<DynamicLibrary>> &libraries, ::YAML::Node &node) -> void
    {
        ::YAML::Node librariesNode = CacheWriter::ensure_node(node["dynamic_libraries"]);

        for (const std::unique_ptr<DynamicLibrary> &lib : libraries)
        {
            librariesNode.push_back(lib->project->name);
        }
    }

    static auto save_header_units(const std::vector<std::unique_ptr<HeaderUnit>> &units, ::YAML::Node &node) -> void
    {
        ::YAML::Node unitsNode = CacheWriter::ensure_node(node["header_units"]);

        for (const std::unique_ptr<HeaderUnit> &unit : units)
        {
            CacheWriter::save_header(*unit, unitsNode);
        }
    }

    static auto save_header(const Header &header, ::YAML::Node &node) -> void
    {
        ::YAML::Node unitNode = CacheWriter::ensure_node(node[header.file->path.string()]);
        CacheWriter::save_headers(header.includes, unitNode);
        CacheWriter::save_imported_header_units(header.importedHeaderUnits, unitNode);
        CacheWriter::save_imported_modules(header.importedModules, unitNode);
        CacheWriter::save_imported_module_partitions(header.importedModulePartitions, unitNode);
    }

    static auto save_headers(const std::vector<Header> &headers, ::YAML::Node &node) -> void
    {
        ::YAML::Node includesNode = CacheWriter::ensure_node(node["includes"]);

        for (const Header &header : headers)
        {
            CacheWriter::save_header(header, node);
        }
    }

    static auto save_imported_header_units(const std::vector<HeaderUnit *> &headerUnits, ::YAML::Node &node) -> void
    {
        ::YAML::Node headerUnitsNode = CacheWriter::ensure_node(node["importedHeaderUnits"]);

        for (const HeaderUnit *unit : headerUnits)
        {
            headerUnitsNode.push_back(unit->file->path.string());
        }
    }

    static auto save_imported_modules(const std::vector<Module *> &modules, ::YAML::Node &node) -> void
    {
        ::YAML::Node modulesNode = CacheWriter::ensure_node(node["importedModules"]);

        for (const Module *mod : modules)
        {
            modulesNode.push_back(mod->name);
        }
    }

    static auto save_imported_module_partitions(const std::vector<ModulePartition *> &partitions, ::YAML::Node &node) -> void
    {
        ::YAML::Node partitionsNode = CacheWriter::ensure_node(node["importedModules"]);

        for (const ModulePartition *partition : partitions)
        {
            partitionsNode.push_back(partition->name);
        }
    }

    static auto save_module(const Module *mod, ::YAML::Node &node) -> void
    {
        ::YAML::Node modNode = node[mod->name];
        modNode["translationUnit"] = mod->translationUnit->sourceFile->path.string();

        if (mod->visibility == Visibility::Public)
        {
            modNode["export"] = "true";
        }

        CacheWriter::save_module_partitions(mod->partitions, node);
    }

    static auto save_module_partition(const ModulePartition *partition, ::YAML::Node &node) -> void
    {
        ::YAML::Node partitionNode = CacheWriter::ensure_node(node[partition->name]);
        partitionNode["translationUnit"] = partition->translationUnit->sourceFile->path.string();

        if (partition->visibility == Visibility::Public)
        {
            partitionNode["export"] = "true";
        }
    }

    static auto save_module_partitions(const std::vector<ModulePartition *> &partitions, ::YAML::Node &node) -> void
    {
        ::YAML::Node partitionsNode = CacheWriter::ensure_node(node["partitions"]);

        for (const ModulePartition *partition : partitions)
        {
            CacheWriter::save_module_partition(partition, partitionsNode);
        }
    }

    static auto save_modules(const std::vector<std::unique_ptr<Module>> &modules, ::YAML::Node &node) -> void
    {
        ::YAML::Node modulesNode = CacheWriter::ensure_node(node["modules"]);

        for (const std::unique_ptr<Module> &mod : modules)
        {
            CacheWriter::save_module(mod.get(), modulesNode);
        }
    }

    static auto save_static_libraries(const std::vector<std::unique_ptr<StaticLibrary>> &libraries, ::YAML::Node &node) -> void
    {
        ::YAML::Node librariesNode = CacheWriter::ensure_node(node["static_libraries"]);

        for (const std::unique_ptr<StaticLibrary> &lib : libraries)
        {
            librariesNode.push_back(lib->project->name);
        }
    }

    static auto save_token(const Token &token, ::YAML::Node &node)
    {
        std::stringstream stream;
        stream << token;
        node.push_back(stream.str());
    }

    static auto save_tokens(const std::vector<Token> &tokens, ::YAML::Node &&node) -> void
    {
        for (const Token &token : tokens)
        {
            CacheWriter::save_token(token, node);
        }
    }

    static auto save_toolchain(const Toolchain *toolchain, ::YAML::Node &node) -> void
    {
        ::YAML::Node toolchainNode = CacheWriter::ensure_node(node[toolchain->name]);
        toolchainNode["frontend"] = CacheWriter::to_string(toolchain->frontend);
        toolchainNode["c_compiler"] = toolchain->cCompiler.string();
        toolchainNode["cpp_compiler"] = toolchain->cppCompiler.string();
        toolchainNode["linker"] = toolchain->linker.string();
        toolchainNode["archiver"] = toolchain->archiver.string();

        CacheWriter::save_abi(toolchain->abi, CacheWriter::ensure_node(toolchainNode["abi"]));
    }

    static auto save_translation_unit(const TranslationUnit *unit, ::YAML::Node &node) -> void
    {
        ::YAML::Node unitNode = CacheWriter::ensure_node(node[unit->sourceFile->path.string()]);
        CacheWriter::save_headers(unit->includes, unitNode);
        CacheWriter::save_imported_header_units(unit->importedHeaderUnits, unitNode);
        CacheWriter::save_imported_modules(unit->importedModules, unitNode);
        CacheWriter::save_imported_module_partitions(unit->importedModulePartitions, unitNode);
    }

    static auto save_translation_units(const std::vector<std::unique_ptr<TranslationUnit>> &units, ::YAML::Node &node) -> void
    {
        ::YAML::Node unitsNode = CacheWriter::ensure_node(node["translationUnis"]);

        for (const std::unique_ptr<TranslationUnit> &unit : units)
        {
            CacheWriter::save_translation_unit(unit.get(), unitsNode);
        }
    }

    [[nodiscard]] static auto to_string(const ABI::Architecture &architecture) -> std::string
    {
        if (architecture == ABI::Architecture::ARM)
        {
            return "ARM";
        }

        return "X86";
    }

    [[nodiscard]] static auto to_string(const ABI::Bitness &bitness) -> std::string
    {
        if (bitness == ABI::Bitness::X32)
        {
            return "X32";
        }

        return "X64";
    }

    [[nodiscard]] static auto to_string(const ABI::Platform &platform) -> std::string
    {
        if (platform == ABI::Platform::Unix)
        {
            return "Unix";
        }

        if (platform == ABI::Platform::Windows)
        {
            return "Windows";
        }

        return "Linux";
    }

    [[nodiscard]] static auto to_string(const Toolchain::Frontend &frontend) -> std::string
    {
        if (frontend == Toolchain::Frontend::Clang)
        {
            return "Clang";
        }

        if (frontend == Toolchain::Frontend::MSVC)
        {
            return "MSVC";
        }

        return "GCC";
    }

    ::YAML::Node root;
};

//! \private
auto write_cache(const std::filesystem::path &path, const CacheData &data) -> void
{
    CacheWriter writer;
    writer.save_toolchains(data.toolchains);
    writer.save_sources(data.sources);
    writer.save_headers(data.headers);
    writer.save_configurations(data.configurations);
    writer.save_to_file(path);
}
}
