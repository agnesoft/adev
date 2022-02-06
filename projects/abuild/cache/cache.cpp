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
    [[nodiscard]] static auto find_file(const std::filesystem::path &path, const auto &index)
    {
        using T = std::remove_cvref_t<decltype(index)>::mapped_type;
        auto range = index.equal_range(path.filename().string());

        for (auto it = range.first; it != range.second; ++it)
        {
            if (Cache::is_same(it->second->path, path))
            {
                return it->second;
            }
        }

        return T{nullptr};
    }

    [[nodiscard]] static auto insert_file(std::filesystem::path path, auto &files, auto &index)
    {
        using T = std::remove_cvref_t<decltype(files)>::value_type::element_type;
        T *file = files.emplace_back(new T{File{.path = std::move(path)}}).get();
        index.insert({file->path.filename().string(), file});
        return file;
    }

public:
    explicit Cache(std::filesystem::path path) :
        filePath{std::move(path)}
    {
        if (std::filesystem::exists(this->filePath))
        {
            this->load();
        }
    }

    auto add_header_file(std::filesystem::path path) -> HeaderFile *
    {
        return Cache::insert_file(std::move(path), this->data.headers, this->data.headerIndex);
    }

    auto add_project(std::string name) -> Project *
    {
        Project *project = this->data.projects.emplace_back(new Project{.name = std::move(name)}).get();
        this->data.projectIndex[project->name] = project;
        return project;
    }

    auto add_source_file(std::filesystem::path path) -> SourceFile *
    {
        return Cache::insert_file(std::move(path), this->data.sources, this->data.sourceIndex);
    }

    [[nodiscard]] auto header_file(const std::filesystem::path &path) -> HeaderFile *
    {
        return Cache::find_file(path, this->data.headerIndex);
    }

    [[nodiscard]] auto project(const std::string &name) -> Project *
    {
        return this->data.projectIndex.at(name);
    }

    [[nodiscard]] auto source_file(const std::filesystem::path &path) -> SourceFile *
    {
        return Cache::find_file(path, this->data.sourceIndex);
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
    struct Data
    {
        std::vector<std::unique_ptr<SourceFile>> sources;
        std::vector<std::unique_ptr<HeaderFile>> headers;
        std::vector<std::unique_ptr<Project>> projects;

        std::unordered_multimap<std::string, SourceFile *> sourceIndex;
        std::unordered_multimap<std::string, HeaderFile *> headerIndex;
        std::unordered_map<std::string, Project *> projectIndex;
    };

    [[nodiscard]] static auto is_same(std::filesystem::path left, std::filesystem::path right) -> bool
    {
        while (right.has_parent_path())
        {
            left = left.parent_path();
            right = right.parent_path();

            if (left.filename() != right.filename())
            {
                return false;
            }
        }

        return true;
    }

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

    static auto load_tokens([[maybe_unused]] const ::YAML::Node &node) -> std::vector<Token>
    {
        // TODO
        return {};
    }

    auto load_headers(const ::YAML::Node &node) -> void
    {
        for (auto it = node.begin(); it != node.end(); ++it)
        {
            HeaderFile *header = this->add_header_file(it->first.as<std::string>());
            header->tokens = Cache::load_tokens(it->second["tokens"]);
        }
    }

    auto load_sources(const ::YAML::Node &node) -> void
    {
        for (auto it = node.begin(); it != node.end(); ++it)
        {
            SourceFile *source = this->add_source_file(it->first.as<std::string>());
            source->tokens = Cache::load_tokens(it->second["tokens"]);
        }
    }

    auto load_project_headers(const ::YAML::Node &node) -> std::vector<HeaderFile *>
    {
        std::vector<HeaderFile *> files;

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            files.push_back(this->header_file((*it).as<std::string>()));
        }

        return files;
    }

    auto load_project_sources(const ::YAML::Node &node) -> std::vector<SourceFile *>
    {
        std::vector<SourceFile *> files;

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            files.push_back(this->source_file((*it).as<std::string>()));
        }

        return files;
    }

    auto load_project(std::string name, const ::YAML::Node &node) -> void
    {
        Project *proj = this->add_project(std::move(name));
        proj->sources = this->load_project_sources(node["sources"]);
        proj->headers = this->load_project_headers(node["headers"]);
    }

    auto load_projects(const ::YAML::Node &node) -> void
    {
        for (auto it = node.begin(); it != node.end(); ++it)
        {
            this->load_project(it->first.as<std::string>(), it->second);
        }
    }

    auto load() -> void
    {
        const ::YAML::Node root = ::YAML::LoadFile(this->filePath.string());

        this->load_sources(root["sources"]);
        this->load_headers(root["headers"]);
        this->load_projects(root["projects"]);
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
        Cache::save_tokens(file->tokens, Cache::ensure_node(node[file->path.string()])["tokens"]);
    }

    static auto save_files(const auto &files, ::YAML::Node &node)
    {
        for (const auto &file : files)
        {
            Cache::save_file(file, node);
        }
    }

    static auto save_project(const Project *project, ::YAML::Node &node) -> void
    {
        ::YAML::Node &projectNode = Cache::ensure_node(node[project->name]);
        Cache::add_files(project->sources, Cache::ensure_node(projectNode["sources"]));
        Cache::add_files(project->headers, Cache::ensure_node(projectNode["headers"]));
    }

    auto save_projects(::YAML::Node &node) -> void
    {
        ::YAML::Node &projectsNode = Cache::ensure_node(node["projects"]);

        for (const std::unique_ptr<Project> &project : this->data.projects)
        {
            Cache::save_project(project.get(), projectsNode);
        }
    }

    auto save() -> void
    {
        ::YAML::Node root;
        this->save_projects(root);
        Cache::save_files(this->data.sources, Cache::ensure_node(root["sources"]));
        Cache::save_files(this->data.headers, Cache::ensure_node(root["headers"]));

        std::ofstream{this->filePath} << root;
    }

    std::filesystem::path filePath;
    Data data;
};
}
