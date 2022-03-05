#ifndef __clang__
module abuild.cache : cache_writer;
import : cache_data;
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

    auto save_to_file(const std::filesystem::path &path) -> void
    {
        std::ofstream{path} << this->root;
    }

private:
    [[nodiscard]] static auto ensure_node(::YAML::Node &&node) -> ::YAML::Node &&
    {
        return std::move(node = {});
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
        fileNode["size"] = file->size;
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

    ::YAML::Node root;
};

//! \private
auto write_cache(const std::filesystem::path &path, const CacheData &data) -> void
{
    CacheWriter writer;
    writer.save_sources(data.sources);
    writer.save_headers(data.headers);
    writer.save_to_file(path);
}
}
