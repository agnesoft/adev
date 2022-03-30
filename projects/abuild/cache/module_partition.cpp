#ifndef __clang__
export module abuild.cache : module_partition;
export import : file;
export improt : visibility;
#endif

namespace abuild
{
export struct Module;
export class TranslationUnit;

export struct ModulePartition
{
    enum class Visibility
    {
        Public,
        Private
    };

    std::string name;
    File precompiledModuleInterface;
    Module *mod = nullptr;
    TranslationUnit *translationUnit = nullptr;
    Visibility visibility = Visibility::Private;
};
}
