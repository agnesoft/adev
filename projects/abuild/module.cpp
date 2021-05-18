#ifdef _MSC_VER
export module abuild : cpp_module;
import<astl.hpp>;
#endif

namespace abuild
{
export class Source;

export struct ModulePartition
{
    std::string name;
    Source *source = nullptr;
};

export struct Module
{
    std::string name;
    Source *source = nullptr;
    std::vector<ModulePartition> partitions;
};
}
