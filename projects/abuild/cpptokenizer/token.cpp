#ifndef __clang__
export module abuild.cpptokenizer : token;
export import : condition;
#endif

namespace abuild
{
export struct DefineToken
{
    std::string name;
    std::string value = "1";
};

export struct ImportIncludeExternalToken
{
    std::string name;
    bool exported = false;
};

export struct ImportIncludeLocalToken
{
    std::string name;
    bool exported = false;
};

export struct ImportModulePartitionToken
{
    std::string name;
    bool exported = false;
};

export struct ImportModuleToken
{
    std::string name;
    bool exported = false;
};

export struct IncludeExternalToken
{
    std::string name;
};

export struct IncludeLocalToken
{
    std::string name;
};

export struct ModulePartitionToken
{
    std::string mod;
    std::string name;
    bool exported = false;
};

export struct ModuleToken
{
    std::string name;
    bool exported = false;
};

export using Token = std::variant<
    ConditionToken,
    DefineToken,
    ImportIncludeExternalToken,
    ImportIncludeLocalToken,
    ImportModulePartitionToken,
    ImportModuleToken,
    IncludeExternalToken,
    IncludeLocalToken,
    ModulePartitionToken,
    ModuleToken>;
}
