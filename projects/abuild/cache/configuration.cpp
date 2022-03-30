#ifndef __clang__
export module abuild.cache : configuration;
export import : flag;
export import : toolchain;
export import : executable;
export import : dynamic_library;
export import : static_library;
#endif

namespace abuild
{
//! The `Configuration` represents the build
//! configuration. That is the combination of
//! `Toolchain`, custom `defines` and compiler and
//! linker flags.
export struct Configuration
{
    //! Pointer to the `Toolchain` that defines
    //! the base ABI.
    const Toolchain *toolchain = nullptr;

    //! List of `defines` for this configuration
    //! that is the combination of the compiler
    //! in-built defines and user specified
    //! defines.
    std::unordered_set<std::string> defines;

    //! List of flags for the compiler.
    std::vector<Flag> compilerFlags;

    //! List of flags for the linker.
    std::vector<Flag> linkerFlags;

    //! List of flags for the archiver.
    std::vector<Flag> archiverFlags;

    std::vector<DynamicLibrary> dynamicLibraries;
    std::vector<StaticLibrary> staticLibraries;
    std::vector<Executable> executables;
    std::vector<TranslationUnit> translationUnits;
    std::vector<HeaderUnit> headerUnits;
    std::vector<Module> modules;
};
}
