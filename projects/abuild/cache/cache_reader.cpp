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
    }

private:
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

    Cache &cache;
};

//! \private
auto read_cache(const std::filesystem::path &path, Cache &cache) -> void
{
    CacheReader{cache}.read(path);
}
}