#ifdef _MSC_VER
export module abuild : dependency;
export import<astl.hpp>;
#endif

namespace abuild
{
export class Header;
export struct Module;
export struct ModulePartition;
export class Source;

export struct IncludeExternalHeaderDependency
{
    std::string name;
    Header *header = nullptr;
};

export struct IncludeLocalHeaderDependency
{
    std::string name;
    Header *header = nullptr;
};

export struct IncludeLocalSourceDependency
{
    std::string name;
    Source *source = nullptr;
};

export struct IncludeSTLHeaderDependency
{
    std::string name;
};

export struct ImportExternalHeaderDependency
{
    std::string name;
    Header *header = nullptr;
};

export struct ImportLocalHeaderDependency
{
    std::string name;
    Header *header = nullptr;
};

export struct ImportModuleDependency
{
    std::string name;
    Module *mod = nullptr;
};

export struct ImportModulePartitionDependency
{
    std::string name;
    ModulePartition *partition = nullptr;
};

export struct ImportSTLHeaderDependency
{
    std::string name;
};

export using Dependency = std::variant<
    IncludeExternalHeaderDependency,
    IncludeLocalHeaderDependency,
    IncludeLocalSourceDependency,
    IncludeSTLHeaderDependency,
    ImportExternalHeaderDependency,
    ImportLocalHeaderDependency,
    ImportModuleDependency,
    ImportModulePartitionDependency,
    ImportSTLHeaderDependency>;
}
