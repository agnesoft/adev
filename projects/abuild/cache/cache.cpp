export module abuild.cache;

import yamlcpp;

#ifndef __clang__
export import : header_file;
export import : source_file;
export import : project;
#else
export import abuild.cpptokenizer;
// clang-format off
#include "file.cpp" //NOLINT(bugprone-suspicious-include)
#include "source_file.cpp" //NOLINT(bugprone-suspicious-include)
#include "header_file.cpp" //NOLINT(bugprone-suspicious-include)
#include "project.cpp" //NOLINT(bugprone-suspicious-include)
// clang-format on
#endif

namespace abuild
{
export class Cache
{
public:
    explicit Cache(std::filesystem::path path) :
        file{std::move(path)}
    {
    }

    auto add_source_file(std::filesystem::path path) -> SourceFile &
    {
        return this->sources.emplace_back(SourceFile{File{.path = std::move(path)}});
    }

    auto add_header_file(std::filesystem::path path) -> HeaderFile &
    {
        return this->headers.emplace_back(HeaderFile{File{.path = std::move(path)}});
    }

    auto add_project(std::string name) -> Project &
    {
        return this->projects.emplace_back(Project{.name = std::move(name)});
    }

    ~Cache()
    {
        try
        {
            this->save();
        }
        catch (...)
        {
        }
    }

private:
    [[nodiscard]] static auto ensure_node(::YAML::Node &&node) -> ::YAML::Node &
    {
        return node = {};
    }

    static auto add_file(const auto &file, ::YAML::Node &node) -> void
    {
        node.push_back(file->path.string());
    }

    static auto add_files(const auto &files, ::YAML::Node &node) -> void
    {
        for (const auto &file : files)
        {
            Cache::add_file(file, node);
        }
    }

    static auto save_token([[maybe_unused]] const Token &token, [[maybe_unused]] ::YAML::Node &node)
    {
        // TODO
    }

    static auto save_tokens(const std::vector<Token> &tokens, ::YAML::Node &&node) -> void
    {
        for (const Token &token : tokens)
        {
            Cache::save_token(token, node);
        }
    }

    static auto save_file(const auto &file, ::YAML::Node &node) -> void
    {
        Cache::save_tokens(file.tokens, Cache::ensure_node(node[file.path.string()])["tokens"]);
    }

    static auto save_files(const auto &files, ::YAML::Node &node)
    {
        for (const auto &file : files)
        {
            Cache::save_file(file, node);
        }
    }

    static auto save_project(const Project &project, ::YAML::Node &node) -> void
    {
        ::YAML::Node &projectNode = Cache::ensure_node(node[project.name]);
        Cache::add_files(project.sources, Cache::ensure_node(projectNode["sources"]));
        Cache::add_files(project.headers, Cache::ensure_node(projectNode["headers"]));
    }

    auto save_projects(::YAML::Node &node) -> void
    {
        ::YAML::Node &projectsNode = Cache::ensure_node(node["projects"]);

        for (const Project &project : this->projects)
        {
            Cache::save_project(project, projectsNode);
        }
    }

    auto save() -> void
    {
        ::YAML::Node root;
        this->save_projects(root);
        Cache::save_files(this->sources, Cache::ensure_node(root["sources"]));
        Cache::save_files(this->headers, Cache::ensure_node(root["headers"]));

        std::ofstream{this->file} << root;
    }

    std::filesystem::path file;
    std::vector<HeaderFile> headers;
    std::vector<Project> projects;
    std::vector<SourceFile> sources;
};
}
