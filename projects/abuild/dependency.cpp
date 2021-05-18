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

struct IncludeExternalHeader
{
    std::string name;
    Header *header = nullptr;
};

struct IncludeLocalHeader
{
    std::string name;
    Header *header = nullptr;
};

struct IncludeLocalSource
{
    std::string name;
    Source *source = nullptr;
};

struct IncludeSTLHeader
{
    std::string name;
};

struct ImportExternalHeader
{
    std::string name;
    Header *header = nullptr;
};

struct ImportLocalHeader
{
    std::string name;
    Header *header = nullptr;
};

struct ImportModule
{
    std::string name;
    Module *mod = nullptr;
};

struct ImportModulePartition
{
    std::string name;
    ModulePartition *partition = nullptr;
};

struct ImportSTLHeader
{
    std::string name;
};

using Dependency = std::variant<
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
