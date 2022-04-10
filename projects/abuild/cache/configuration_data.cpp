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

export struct ConfigurationData
{
    //! Configuration name
    std::string name;

    //! Pointer to the `Toolchain` that defines
    //! the base ABI.
    const Toolchain *toolchain = nullptr;

    //! List of `defines` for this configuration
    //! that is the combination of the compiler
    //! in-built defines and user specified
    //! defines.
    std::vector<DefineToken> defines;

    //! List of flags for the compiler.
    std::vector<Flag> compilerFlags;

    //! List of flags for the linker.
    std::vector<Flag> linkerFlags;

    //! List of flags for the archiver.
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
