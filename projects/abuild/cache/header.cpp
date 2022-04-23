#ifndef __clang__
export module abuild.cache:header;
export import astl;
#endif

namespace abuild
{
export struct CppFile;
export struct HeaderUnit;
export struct Module;
export struct ModulePartition;

//! The `Header` is a file included by the
//! preprocessor directive `#include`.
export struct Header
{
    //! The C++ file being included.
    CppFile *file = nullptr;

    //! List of `Header`s included by this header.
    std::vector<Header> includes;

    //! List of `HeaderUnit`s imported by this
    //! header.
    std::vector<HeaderUnit *> headerUnit;

    //! List of `Module`s imported by this header.
    std::vector<Module *> modules;

    //! List of `ModulePartition`s imported by
    //! this header.
    std::vector<ModulePartition *> modulePartitions;
};
}
