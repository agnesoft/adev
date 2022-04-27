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
    std::vector<DefineToken> defines;
    std::vector<Flag> compilerFlags;
    std::vector<Flag> linkerFlags;
    std::vector<Flag> archiverFlags;
    std::vector<std::unique_ptr<Project>> projects;
    std::vector<std::unique_ptr<SourceFile>> sources;
    std::vector<std::unique_ptr<HeaderFile>> headers;
    std::vector<std::unique_ptr<HeaderUnit>> headerUnits;
    std::vector<std::unique_ptr<Module>> modules;
    std::vector<std::unique_ptr<ModulePartition>> modulePartitions;
    Settings settings;
    Toolchain toolchain;
    std::string configurationName;
    std::filesystem::path filePath;
};
}
