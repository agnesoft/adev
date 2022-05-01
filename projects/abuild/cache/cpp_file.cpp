#ifndef __clang__
export module abuild.cache:cpp_file;
export import :file;
export import :header;
export import abuild.cpptokenizer;
#endif

namespace abuild
{
export struct Project;

//! The `CppFile` is common base class for C++
//! source files: headers (.hpp) and sources
//! (.cpp).
export struct CppFile : File
{
    //! Project this source file belongs to.
    Project *project = nullptr;

    //! Relevant code tokens extracted during code
    //! analysis.
    std::vector<Token> tokens;

    //! List of `Header`s included by the source
    //! file.
    std::vector<Header> includes;

    //! List of `HeaderUnit`s imported by the
    //! source file.
    std::vector<HeaderUnit *> importedHeaderUnits;

    //! List of `Module`s imported by the file.
    std::vector<Module *> importedModules;

    //! List of `ModulePartition`s imported by the
    //! file.
    std::vector<ModulePartition *> importedModulePartitions;
};
}
