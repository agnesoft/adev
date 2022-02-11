#ifndef __clang__
module abuild.cache : cache_reader;
import : cache_impl;
import yamlcpp;
#endif

namespace abuild
{
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
        this->read_projects(root["projects"]);
    }

private:
    [[nodiscard]] static auto read_tokens(const ::YAML::Node &node) -> std::vector<Token>
    {
        std::stringstream stream;

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            stream << (*it).as<std::string>() << '\n';
        }

        return ::abuild::tokenize(stream.view());
    }

    auto read_headers(const ::YAML::Node &node) -> void
    {
        for (auto it = node.begin(); it != node.end(); ++it)
        {
            HeaderFile *header = this->cache.add_header_file(it->first.as<std::string>());
            header->tokens = CacheReader::read_tokens(it->second["tokens"]);
        }
    }

    auto read_sources(const ::YAML::Node &node) -> void
    {
        for (auto it = node.begin(); it != node.end(); ++it)
        {
            SourceFile *source = this->cache.add_source_file(it->first.as<std::string>());
            source->tokens = CacheReader::read_tokens(it->second["tokens"]);
        }
    }

    auto read_project_headers(const ::YAML::Node &node) -> std::vector<HeaderFile *>
    {
        std::vector<HeaderFile *> files;

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            files.push_back(this->cache.header_file((*it).as<std::string>()));
        }

        return files;
    }

    auto read_project_sources(const ::YAML::Node &node) -> std::vector<SourceFile *>
    {
        std::vector<SourceFile *> files;

        for (auto it = node.begin(); it != node.end(); ++it)
        {
            files.push_back(this->cache.source_file((*it).as<std::string>()));
        }

        return files;
    }

    auto read_project(std::string name, const ::YAML::Node &node) -> void
    {
        Project *proj = this->cache.add_project(std::move(name));
        proj->sources = this->read_project_sources(node["sources"]);
        proj->headers = this->read_project_headers(node["headers"]);
    }

    auto read_projects(const ::YAML::Node &node) -> void
    {
        for (auto it = node.begin(); it != node.end(); ++it)
        {
            this->read_project(it->first.as<std::string>(), it->second);
        }
    }

private:
    Cache &cache;
};

auto read_cache(const std::filesystem::path &path, Cache &cache) -> void
{
    CacheReader{cache}.read(path);
}
}