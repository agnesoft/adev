#ifndef __clang__
export module abuild.cache:configuration_data;
export import :flag;
export import :toolchain;
export import :executable;
export import :dynamic_library;
export import :static_library;
export import :translation_unit;
export import abuild.cpptokenizer;
#endif

namespace abuild
{
//! \private
export struct ConfigurationData
{
    std::string name;
    const Toolchain *toolchain = nullptr;
    std::vector<DefineToken> defines;
    std::vector<Flag> compilerFlags;
    std::vector<Flag> linkerFlags;
    std::vector<Flag> archiverFlags;
    std::vector<std::unique_ptr<DynamicLibrary>> dynamicLibraries;
    std::vector<std::unique_ptr<StaticLibrary>> staticLibraries;
    std::vector<std::unique_ptr<Executable>> executables;
    std::vector<std::unique_ptr<TranslationUnit>> translationUnits;
    std::vector<std::unique_ptr<HeaderUnit>> headerUnits;
    std::vector<std::unique_ptr<Module>> modules;
    std::vector<std::unique_ptr<ModulePartition>> modulePartitions;
};
}
