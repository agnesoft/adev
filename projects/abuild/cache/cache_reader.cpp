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
    explicit CacheReader(Cache &cache) :
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

    Cache &cache;
};

//! \private
auto read_cache(const std::filesystem::path &path, Cache &cache) -> void
{
    CacheReader{cache}.read(path);
}
}