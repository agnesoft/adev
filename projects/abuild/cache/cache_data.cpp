#ifndef __clang__
export module abuild.cache:cache_data;
export import :configuration;
export import :header_file;
export import :project;
export import :source_file;
export import :settings;
#endif

namespace abuild
{
//! \private
struct CacheData
{
    std::vector<std::unique_ptr<SourceFile>> sources;
    std::vector<std::unique_ptr<HeaderFile>> headers;
    std::vector<std::unique_ptr<Project>> projects;
    std::vector<std::unique_ptr<Toolchain>> toolchains;
    std::vector<std::unique_ptr<Configuration>> configurations;
    Settings settings;
};
}
