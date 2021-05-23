#ifdef _MSC_VER
export module abuild : cpp_module;
import<astl.hpp>;
#endif

namespace abuild
{
export class Source;

export enum class ModuleVisibility {
    Public,
    Private
};

export struct Module;

export struct ModulePartition
{
    std::string name;
    ModuleVisibility visibility = ModuleVisibility::Private;
    Source *source = nullptr;
    Module *mod = nullptr;
};

export struct Module
{
    std::string name;
    ModuleVisibility visibility = ModuleVisibility::Private;
    Source *source = nullptr;
    std::vector<std::unique_ptr<ModulePartition>> partitions;
};
}
