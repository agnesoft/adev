#ifndef __clang__
export module abuild.cache:translation_unit;
export import :source_file;
export import :header_unit;
export import :module_;
#endif

namespace abuild
{
export struct TranslationUnit : File
{
    SourceFile *sourceFile = nullptr;
    std::vector<Header> includes;
    std::vector<HeaderUnit *> importedHeaderUnits;
    std::vector<Module *> importedModules;
    std::vector<ModulePartition *> importedModulePartitions;
};
}
