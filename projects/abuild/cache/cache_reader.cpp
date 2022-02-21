#ifndef __clang__
module abuild.cache : cache_reader;
import : cache_impl;
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
            HeaderFile *header = this->cache.add_header_file(it->first.as<std::string>(), it->second["project"].as<std::string>());
            header->tokens = CacheReader::read_tokens(it->second["tokens"]);
        }
    }

    auto read_sources(const ::YAML::Node &node) -> void
    {
        for (auto it = node.begin(); it != node.end(); ++it)
        {
            SourceFile *source = this->cache.add_source_file(it->first.as<std::string>(), it->second["project"].as<std::string>());
            source->tokens = CacheReader::read_tokens(it->second["tokens"]);
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