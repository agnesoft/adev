#ifndef __clang__
export module abuild.cache:module_;
export import :module_partition;
#endif

namespace abuild
{
//! The `Module` is C++ module.
export struct Module
{
    //! Name of the module.
    std::string name;

    //! `TranslationUnit` of this module.
    TranslationUnit *translationUnit = nullptr;

    //! List of `ModulePartition`s of this module.
    std::vector<ModulePartition *> partitions;

    //! The precompiled interface file.
    File precompiledModuleInterface;

    //! Declared visibility of this partition.
    Visibility visibility = Visibility::Private;
};
}
