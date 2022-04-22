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

    [[nodiscard]] static auto to_string(const ABI::Architecture &architecture) -> std::string
    {
        if (architecture == ABI::Architecture::ARM)
        {
            return "ARM";
        }

        return "x86";
    }

    [[nodiscard]] static auto to_string(const ABI::Bitness &bitness) -> std::string
    {
        if (bitness == ABI::Bitness::x32)
        {
            return "x32";
        }

        return "x64";
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

    ::YAML::Node root;
};

//! \private
auto write_cache(const std::filesystem::path &path, const CacheData &data) -> void
{
    CacheWriter writer;
    writer.save_toolchains(data.toolchains);
    writer.save_sources(data.sources);
    writer.save_headers(data.headers);
    writer.save_to_file(path);
}
}
