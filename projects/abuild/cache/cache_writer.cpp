#ifndef __clang__
module abuild.cache : cache_writer;
import : cache_data;
import yamlcpp;
#endif

namespace abuild
{
class CacheWriter
{
public:
    auto save_headers(const std::vector<std::unique_ptr<HeaderFile>> &headers) -> void
    {
        CacheWriter::save_files(headers, CacheWriter::ensure_node(root["headers"]));
    }

    auto save_projects(const std::vector<std::unique_ptr<Project>> &projects) -> void
    {
        ::YAML::Node projectsNode = CacheWriter::ensure_node(this->root["projects"]);

        for (const std::unique_ptr<Project> &project : projects)
        {
            CacheWriter::save_project(project.get(), projectsNode);
        }
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

    static auto add_file(const auto &file, ::YAML::Node &node) -> void
    {
        node.push_back(file->path.string());
    }

    static auto add_files(const auto &files, ::YAML::Node &&node) -> void
    {
        for (const auto &file : files)
        {
            CacheWriter::add_file(file, node);
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

    static auto save_file(const auto &file, ::YAML::Node &node) -> void
    {
        CacheWriter::save_tokens(file->tokens, CacheWriter::ensure_node(node[file->path.string()])["tokens"]);
    }

    static auto save_files(const auto &files, ::YAML::Node &&node) -> void
    {
        for (auto &file : files)
        {
            CacheWriter::save_file(file, node);
        }
    }

    static auto save_project(const Project *project, ::YAML::Node &node) -> void
    {
        ::YAML::Node projectNode = CacheWriter::ensure_node(node[project->name]);
        CacheWriter::add_files(project->sources, CacheWriter::ensure_node(projectNode["sources"]));
        CacheWriter::add_files(project->headers, CacheWriter::ensure_node(projectNode["headers"]));
    }

    ::YAML::Node root;
};

auto write_cache(const std::filesystem::path &path, const CacheData &data) -> void
{
    CacheWriter writer;
    writer.save_sources(data.sources);
    writer.save_headers(data.headers);
    writer.save_projects(data.projects);
    writer.save_to_file(path);
}
}
