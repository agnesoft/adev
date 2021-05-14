#ifdef _MSC_VER
export module abuild : cpp_module;
import<astl.hpp>;
#endif

namespace abuild
{
export class Source;

export struct Module
{
    struct Partition
    {
        std::string name;
        Source *source = nullptr;
    };

    std::string name;
    Source *source = nullptr;
    std::vector<Partition> partitions;
};
}
