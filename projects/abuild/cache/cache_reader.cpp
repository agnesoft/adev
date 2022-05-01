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
    /*
public:
    explicit CacheReader(CacheImpl &cache) :
        cache{cache}
    {
    }

    auto read(const std::filesystem::path &path) -> void
    {
        ::YAML::Node root = ::YAML::LoadFile(path.string());
        this->read_sources(root["sources"]);
        this->read_headers(root["headers"]);
        this->read_toolchains(root["toolchains"]);
    }

    auto read_configurations(const std::filesystem::path &path) -> void
    {
        ::YAML::Node root = ::YAML::LoadFile(path.string());
        this->load_configurations(root["configurations"]);
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

    [[nodiscard]] auto load_configuration(const std::string &name, const ::YAML::Node &node) -> void
    {
        Toolchain *toolchain = this->cache.toolchain(node["toolchain"].as<std::string>());

        if (toolchain == toolchain)
        {
            return;
        }

        Configuration *config = this->cache.add_configuration(toolchain, name);
        static_cast<void>(config);
        // this->read_translation_units(config, node);
        // this->read_header_units(config, node);
        // this->read_modules(config, node);
        // this->read_executables(config, node);
        // this->read_dynamic_libraries(config, node);
        // this->read_static_libraries(config, node);
    }

    [[nodiscard]] auto load_configurations(const ::YAML::Node &node) -> void
    {
        for (auto it = node.begin(); it != node.end(); ++it)
        {
            this->load_configuration(it->first.as<std::string>(), it->second);
        }
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

    auto read_headers(const ::YAML::Node &node) -> void
    {
        for (auto it = node.begin(); it != node.end(); ++it)
        {
            std::filesystem::path path = it->first.as<std::string>();

            if (std::filesystem::exists(path))
            {
                HeaderFile *header = this->cache.add_header_file(path, it->second["project"].as<std::string>());
                header->path = std::move(path);
                header->timestamp = it->second["timestamp"].template as<std::size_t>();
                header->outdated = false;
                header->tokens = CacheReader::read_tokens(it->second["tokens"]);
            }
        }
    }

    auto read_sources(const ::YAML::Node &node) -> void
    {
        for (auto it = node.begin(); it != node.end(); ++it)
        {
            std::filesystem::path path = it->first.as<std::string>();

            if (std::filesystem::exists(path))
            {
                SourceFile *source = this->cache.add_source_file(path, it->second["project"].as<std::string>());
                source->path = std::move(path);
                source->timestamp = it->second["timestamp"].template as<std::size_t>();
                source->outdated = false;
                source->tokens = CacheReader::read_tokens(it->second["tokens"]);
            }
        }
    }

    auto read_translation_units(Configuration *config, const ::YAML::Node &node) -> void
    {
        const ::YAML::Node &units = node["translationUnits"];

        for (auto it = units.begin(); it != units.end(); ++it)
        {
            std::filesystem::path path = it->first.as<std::string>();
            SourceFile *src = this->cache.exact_source_file(path);

            if (src != nullptr)
            {
                config->add_translation_unit(src);
            }
        }
    }

    auto read_toolchains(const ::YAML::Node &node) -> void
    {
        for (auto it = node.begin(); it != node.end(); ++it)
        {
            auto name = it->first.as<std::string>();
            std::filesystem::path cCompiler = it->second["c_compiler"].as<std::string>();
            std::filesystem::path cppCompiler = it->second["cpp_compiler"].as<std::string>();
            std::filesystem::path linker = it->second["linker"].as<std::string>();

            if (std::filesystem::exists(cCompiler)
                && std::filesystem::exists(cppCompiler)
                && std::filesystem::exists(linker))
            {
                Toolchain *toolchain = cache.add_toolchain(name);
                toolchain->frontend = CacheReader::toolchain_frontend(it->second["frontend"].as<std::string>());
                toolchain->cCompiler = std::move(cCompiler);
                toolchain->cppCompiler = std::move(cppCompiler);
                toolchain->linker = std::move(linker);
                toolchain->archiver = it->second["archiver"].as<std::string>();
                toolchain->abi.architecture = CacheReader::abi_architecture(it->second["abi"]["architecture"].as<std::string>());
                toolchain->abi.bitness = CacheReader::abi_bitness(it->second["abi"]["bitness"].as<std::string>());
                toolchain->abi.platform = CacheReader::abi_platform(it->second["abi"]["platform"].as<std::string>());
            }
        }
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

    CacheImpl &cache;*/
};

//! \private
auto read_cache([[maybe_unused]] const std::filesystem::path &path, [[maybe_unused]] CacheImpl &cache) -> void
{
    // CacheReader{cache}.read(path);
}
}