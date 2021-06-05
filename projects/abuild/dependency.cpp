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

export enum class DependencyVisibility {
    Public,
    Private
};

export struct IncludeExternalHeaderDependency
{
    std::string name;
    Header *header = nullptr;
    DependencyVisibility visibility = DependencyVisibility::Public;
};

export struct IncludeExternalSourceDependency
{
    std::string name;
    Source *source = nullptr;
    DependencyVisibility visibility = DependencyVisibility::Public;
};

export struct IncludeLocalHeaderDependency
{
    std::string name;
    Header *header = nullptr;
    DependencyVisibility visibility = DependencyVisibility::Public;
};

export struct IncludeLocalSourceDependency
{
    std::string name;
    Source *source = nullptr;
    DependencyVisibility visibility = DependencyVisibility::Public;
};

export struct IncludeSTLHeaderDependency
{
    std::string name;
    DependencyVisibility visibility = DependencyVisibility::Public;
};

export struct ImportExternalHeaderDependency
{
    std::string name;
    Header *header = nullptr;
    DependencyVisibility visibility = DependencyVisibility::Private;
};

export struct ImportLocalHeaderDependency
{
    std::string name;
    Header *header = nullptr;
    DependencyVisibility visibility = DependencyVisibility::Private;
};

export struct ImportModuleDependency
{
    std::string name;
    Module *mod = nullptr;
    DependencyVisibility visibility = DependencyVisibility::Private;
};

export struct ImportModulePartitionDependency
{
    std::string name;
    ModulePartition *partition = nullptr;
    DependencyVisibility visibility = DependencyVisibility::Private;
};

export struct ImportSTLHeaderDependency
{
    std::string name;
    DependencyVisibility visibility = DependencyVisibility::Private;
};

export using Dependency = std::variant<
    IncludeExternalHeaderDependency,
    IncludeExternalSourceDependency,
    IncludeLocalHeaderDependency,
    IncludeLocalSourceDependency,
    IncludeSTLHeaderDependency,
    ImportExternalHeaderDependency,
    ImportLocalHeaderDependency,
    ImportModuleDependency,
    ImportModulePartitionDependency,
    ImportSTLHeaderDependency>;
}
