#ifndef __clang__
export module abuild.cache:module_partition;
export import :file;
export import :visibility;
#endif

namespace abuild
{
export struct Module;
export struct TranslationUnit;

//! The `ModulePartition` is C++ module partition.
export struct ModulePartition
{
    //! Name of the partition.
    std::string name;

    SourceFile *sourceFile = nullptr;

    //! The module to which this partition belongs.
    Module *mod = nullptr;

    //! Precompiled interface file.
    File precompiledModuleInterface;

    bool exported = false;
};
}
