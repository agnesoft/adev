#ifndef __clang__
export module abuild.cache:cache_data;
export import :flag;
export import :header_file;
export import :header_unit;
export import :module_;
export import :module_partition;
export import :project;
export import :source_file;
export import :settings;
export import :toolchain;
#endif

namespace abuild
{
//! \private
struct CacheData
{
    std::vector<Flag> archiverFlags;
    std::vector<Flag> compilerFlags;
    std::vector<Flag> linkerFlags;
    std::vector<DefineToken> defines;
    std::vector<std::unique_ptr<HeaderFile>> headers;
    std::vector<std::unique_ptr<HeaderUnit>> headerUnits;
    std::vector<std::unique_ptr<Module>> modules;
    std::vector<std::unique_ptr<ModulePartition>> modulePartitions;
    std::vector<std::unique_ptr<Project>> projects;
    std::vector<std::unique_ptr<SourceFile>> sources;
    Settings settings;
    Toolchain toolchain;
    std::string configurationName;
    std::filesystem::path filePath;
    std::filesystem::path projectRoot;
};
}
