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

export struct IncludeExternalHeader
{
    std::string name;
    Header *header = nullptr;
};

export struct IncludeLocalHeader
{
    std::string name;
    Header *header = nullptr;
};

export struct IncludeLocalSource
{
    std::string name;
    Source *source = nullptr;
};

export struct IncludeSTLHeader
{
    std::string name;
};

export struct ImportExternalHeader
{
    std::string name;
    Header *header = nullptr;
};

export struct ImportLocalHeader
{
    std::string name;
    Header *header = nullptr;
};

export struct ImportModule
{
    std::string name;
    Module *mod = nullptr;
};

export struct ImportModulePartition
{
    std::string name;
    ModulePartition *partition = nullptr;
};

export struct ImportSTLHeader
{
    std::string name;
};

export using Dependency = std::variant<
    IncludeExternalHeader,
    IncludeLocalHeader,
    IncludeLocalSource,
    IncludeSTLHeader,
    ImportExternalHeader,
    ImportLocalHeader,
    ImportModule,
    ImportModulePartition,
    ImportSTLHeader>;
}
