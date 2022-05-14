#ifndef __clang__
export module abuild.cache:module_partition;
export import :file;
#endif

namespace abuild
{
export struct Module;
export struct SourceFile;

//! The `ModulePartition` is C++ module partition.
export struct ModulePartition
{
    //! Name of the partition.
    std::string name;

    //! Underlying source file.
    SourceFile *sourceFile = nullptr;

    //! The module to which this partition belongs.
    Module *mod = nullptr;

    //! Precompiled interface file.
    File precompiledModuleInterface;

    //! Indicates if the partition is exported.
    bool exported = false;
};
}
