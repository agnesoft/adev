#ifndef __clang__
export module abuild.cache:module_;
export import :module_partition;
#endif

namespace abuild
{
export struct Module
{
    std::string name;
    TranslationUnit *translationUnit = nullptr;
    std::vector<ModulePartition *> partitions;
    File precompiledModuleInterface;
    Visibility visibility = Visibility::Private;
};
}
