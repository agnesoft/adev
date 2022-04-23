#ifndef __clang__
export module abuild.cache:translation_unit;
export import :source_file;
export import :header_unit;
export import :module_;
#endif

namespace abuild
{
//! The `TranslationUnit` is the base C++
//! compilation unit.
export struct TranslationUnit : File
{
    //! The C++ source file being compiled.
    SourceFile *sourceFile = nullptr;

    //! List of `Header`s included by the source
    //! file.
    std::vector<Header> includes;

    //! List of `HeaderUnit`s imported by the
    //! source file.
    std::vector<HeaderUnit *> importedHeaderUnits;

    //! List of `Module`s imported by the source file.
    std::vector<Module *> importedModules;

    //! List of `ModulePartition`s imported by the
    //! source file.
    std::vector<ModulePartition *> importedModulePartitions;
};
}
