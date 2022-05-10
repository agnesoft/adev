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
    auto save_archiver_flags(const std::vector<Flag> &flags) -> void
    {
        this->save_flags("archiver_flags", flags);
    }

    auto save_compiler_flags(const std::vector<Flag> &flags) -> void
    {
        this->save_flags("compiler_flags", flags);
    }

    auto save_configuration_name(const std::string &name) -> void
    {
        this->root["name"] = name;
    }

    auto save_defines(const std::vector<DefineToken> &defines) -> void
    {
        ::YAML::Node node = this->root["defines"];

        for (const DefineToken &define : defines)
        {
            node[define.name] = define.value;
        }
    }

    auto save_linker_flags(const std::vector<Flag> &flags) -> void
    {
        this->save_flags("linker_flags", flags);
    }

    auto save_header_files(const std::vector<std::unique_ptr<HeaderFile>> &files) -> void
    {
        ::YAML::Node node = this->root["headers"];

        for (const std::unique_ptr<HeaderFile> &file : files)
        {
            CacheWriter::save_header_file(node, file.get());
        }
    }

    auto save_header_units(const std::vector<std::unique_ptr<HeaderUnit>> &headerUnits) -> void
    {
        ::YAML::Node node = this->root["headers_units"];

        for (const std::unique_ptr<HeaderUnit> &unit : headerUnits)
        {
            CacheWriter::save_header_unit(node, unit.get());
        }
    }

    auto save_modules(const std::vector<std::unique_ptr<Module>> &modules) -> void
    {
        ::YAML::Node node = this->root["modules"];

        for (const std::unique_ptr<Module> &mod : modules)
        {
            CacheWriter::save_module(node, mod.get());
        }
    }

    auto save_source_files(const std::vector<std::unique_ptr<SourceFile>> &files) -> void
    {
        ::YAML::Node node = this->root["sources"];

        for (const std::unique_ptr<SourceFile> &file : files)
        {
            CacheWriter::save_source_file(node, file.get());
        }
    }

    auto save_project_root(const std::filesystem::path &path) -> void
    {
        this->root["project_root"] = path.string();
    }

    auto save_projects(const std::vector<std::unique_ptr<Project>> &projects) -> void
    {
        ::YAML::Node node = this->root["projects"];

        for (const std::unique_ptr<Project> &project : projects)
        {
            CacheWriter::save_project(node, project.get());
        }
    }

    auto save_settings(const Settings &settings) -> void
    {
        ::YAML::Node node = this->root["settings"];
        node["project_name_separator"] = settings.projectNameSeparator;
        CacheWriter::save_settings_list(node["cpp_header_extensions"], settings.cppHeaderExtensions);
        CacheWriter::save_settings_list(node["cpp_source_extensions"], settings.cppSourceExtensions);
        CacheWriter::save_settings_list(node["executable_filenames"], settings.executableFilenames);
        CacheWriter::save_settings_list(node["ignore_directories"], settings.ignoreDirectories);
        CacheWriter::save_settings_list(node["skip_directories"], settings.skipDirectories);
        CacheWriter::save_settings_list(node["squash_directories"], settings.squashDirectories);
        CacheWriter::save_settings_list(node["test_directories"], settings.testDirectories);
    }

    auto save_toolchain(const Toolchain &toolchain) -> void
    {
        ::YAML::Node node = this->root["toolchain"];
        node["name"] = toolchain.name;
        node["frontend"] = CacheWriter::to_string(toolchain.frontend);
        node["c_compiler"] = toolchain.cCompiler.string();
        node["cpp_compiler"] = toolchain.cppCompiler.string();
        node["linker"] = toolchain.linker.string();
        node["archiver"] = toolchain.archiver.string();

        CacheWriter::save_abi(node["abi"], toolchain.abi);
    }

    auto save_to_file(const std::filesystem::path &path) -> void
    {
        std::ofstream{path} << this->root;
    }

private:
    static auto save_references(::YAML::Node &node, const auto &entity) -> void
    {
        CacheWriter::save_includes(node["includes"], entity.includes);
        CacheWriter::save_imported_header_units(node["imported_header_units"], entity.importedHeaderUnits);
        CacheWriter::save_imported_modules(node["imported_modules"], entity.importedModules);
        CacheWriter::save_imported_module_partitions(node["imported_module_partitions"], entity.importedModulePartitions);
    }

    static auto save_abi(::YAML::Node &&node, const ABI &abi) -> void
    {
        node["architecture"] = CacheWriter::to_string(abi.architecture);
        node["bitness"] = CacheWriter::to_string(abi.bitness);
        node["platform"] = CacheWriter::to_string(abi.platform);
    }

    auto save_flags(const std::string &name, const std::vector<Flag> &flags) -> void
    {
        ::YAML::Node node = this->root[name];

        for (const Flag &flag : flags)
        {
            node[flag.name] = flag.value;
        }
    }

    static auto save_imported_header_units(::YAML::Node &&node, const std::vector<HeaderUnit *> &headerUnits) -> void
    {
        for (const HeaderUnit *unit : headerUnits)
        {
            node.push_back(unit->headerFile->path.string());
        }
    }

    static auto save_imported_modules(::YAML::Node &&node, const std::vector<Module *> &modules) -> void
    {
        for (const Module *mod : modules)
        {
            node.push_back(mod->name);
        }
    }

    static auto save_imported_module_partitions(::YAML::Node &&node, const std::vector<ModulePartition *> &partitions) -> void
    {
        for (const ModulePartition *partition : partitions)
        {
            node.push_back(partition->mod->name + ':' + partition->name);
        }
    }

    static auto save_include(::YAML::Node &includes, const Header &include) -> void
    {
        ::YAML::Node node = includes[include.file->path.string()];
        node = {};
        CacheWriter::save_references(node, include);
    }

    static auto save_includes(::YAML::Node &&node, const std::vector<Header> &includes) -> void
    {
        for (const Header &include : includes)
        {
            CacheWriter::save_include(node, include);
        }
    }

    static auto save_header_file(::YAML::Node &files, const HeaderFile *file) -> void
    {
        ::YAML::Node node = files[file->path.string()];
        node["timestamp"] = file->timestamp;
        CacheWriter::save_tokens(node["tokens"], file->tokens);
        CacheWriter::save_references(node, *file);
    }

    static auto save_header_unit(::YAML::Node &units, const HeaderUnit *unit) -> void
    {
        ::YAML::Node node = units[unit->headerFile->path.string()];
        CacheWriter::save_file(node["precompiled_header_unit"], unit->precompiledHeaderUnit);
    }

    static auto save_module(::YAML::Node &modules, const Module *mod) -> void
    {
        ::YAML::Node node = modules[mod->name];
        node["exported"] = mod->exported;
        CacheWriter::save_referenced_source_file(node["source_file"], mod->sourceFile);
        CacheWriter::save_file(node["precompiled_module_interface"], mod->precompiledModuleInterface);
        CacheWriter::save_module_partitions(node["partitions"], mod->partitions);
    }

    static auto save_module_partition(::YAML::Node &partitions, const ModulePartition *partition) -> void
    {
        ::YAML::Node node = partitions[partition->name];
        node["source_file"] = partition->sourceFile->path.string();
        node["exported"] = partition->exported;
        CacheWriter::save_file(node["precompiled_module_interface"], partition->precompiledModuleInterface);
    }

    static auto save_module_partitions(::YAML::Node &&node, const std::vector<ModulePartition *> &partitions) -> void
    {
        for (const ModulePartition *partition : partitions)
        {
            CacheWriter::save_module_partition(node, partition);
        }
    }

    static auto save_file(::YAML::Node &&node, const File &file) -> void
    {
        node["path"] = file.path.string();
        node["timestamp"] = file.timestamp;
    }

    static auto save_project(::YAML::Node &projects, const Project *project) -> void
    {
        ::YAML::Node node = projects[project->name];
        node["type"] = CacheWriter::to_string(project->type);
        CacheWriter::save_file(node["linked_file"], project->linkedFile);
        CacheWriter::save_project_headers(node["headers"], project->headers);
        CacheWriter::save_project_sources(node["sources"], project->sources);
    }

    static auto save_project_headers(::YAML::Node &&node, const std::vector<HeaderFile *> &files) -> void
    {
        for (const HeaderFile *file : files)
        {
            node.push_back(file->path.string());
        }
    }

    static auto save_referenced_source_file(::YAML::Node &&node, SourceFile *file) -> void
    {
        if (file != nullptr)
        {
            node = file->path.string();
        }
        else
        {
            node = "";
        }
    }

    static auto save_source_file(::YAML::Node &files, const SourceFile *file) -> void
    {
        ::YAML::Node node = files[file->path.string()];
        node["timestamp"] = file->timestamp;
        CacheWriter::save_file(node["object_file"], file->objectFile);
        CacheWriter::save_tokens(node["tokens"], file->tokens);
        CacheWriter::save_references(node, *file);
    }

    static auto save_project_sources(::YAML::Node &&node, const std::vector<SourceFile *> &files) -> void
    {
        for (const SourceFile *file : files)
        {
            node.push_back(file->path.string());
        }
    }

    static auto save_settings_list(::YAML::Node &&node, const std::unordered_set<std::string> &list) -> void
    {
        for (const std::string &value : list)
        {
            node.push_back(value);
        }
    }

    static auto save_token(::YAML::Node &node, const Token &token)
    {
        std::stringstream stream;
        stream << token;
        node.push_back(stream.str());
    }

    static auto save_tokens(::YAML::Node &&node, const std::vector<Token> &tokens) -> void
    {
        for (const Token &token : tokens)
        {
            CacheWriter::save_token(node, token);
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

    [[nodiscard]] static auto to_string(const Project::Type &type) -> std::string
    {
        if (type == Project::Type::Executable)
        {
            return "Executable";
        }

        if (type == Project::Type::DynamicLibrary)
        {
            return "DynamicLibrary";
        }

        return "StaticLibrary";
    }

    ::YAML::Node root;
};

//! \private
auto write_cache(const CacheData &data) -> void
{
    CacheWriter writer;
    writer.save_configuration_name(data.configurationName);
    writer.save_project_root(data.projectRoot);
    writer.save_settings(data.settings);
    writer.save_toolchain(data.toolchain);
    writer.save_compiler_flags(data.compilerFlags);
    writer.save_linker_flags(data.linkerFlags);
    writer.save_archiver_flags(data.archiverFlags);
    writer.save_defines(data.defines);
    writer.save_projects(data.projects);
    writer.save_header_files(data.headers);
    writer.save_source_files(data.sources);
    writer.save_modules(data.modules);
    writer.save_header_units(data.headerUnits);

    writer.save_to_file(data.filePath);
}
}
