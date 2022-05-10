#ifndef __clang__
export module abuild.cache:module_;
export import :file;
#endif

namespace abuild
{
export struct ModulePartition;
export struct SourceFile;

//! The `Module` is C++ module.
export struct Module
{
    //! Name of the module.
    std::string name;

    //! Underlying source file.
    SourceFile *sourceFile = nullptr;

    //! List of `ModulePartition`s of this module.
    std::vector<ModulePartition *> partitions;

    //! The precompiled interface file.
    File precompiledModuleInterface;

    //! Indicates if the module is exported.
    bool exported = false;
};
}
