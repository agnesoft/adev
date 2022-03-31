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

export struct Header
{
    CppFile *file = nullptr;
    std::vector<Header> includes;
    std::vector<HeaderUnit *> headerUnit;
    std::vector<Module *> modules;
    std::vector<ModulePartition *> modulePartitions;
};
}
