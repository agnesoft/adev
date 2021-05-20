#ifdef _MSC_VER
export module abuild : token;
import<astl.hpp>;
#endif

namespace abuild
{
export enum class TokenVisibility {
    Exported,
    Private
};

export struct IncludeLocalToken
{
    std::string name;
};

export struct IncludeExternalToken
{
    std::string name;
};

export struct ModuleToken
{
    std::string name;
    TokenVisibility visibility = TokenVisibility::Private;
};

export struct ModulePartitionToken
{
    std::string name;
    std::string mod;
    TokenVisibility visibility = TokenVisibility::Private;
};

export struct ImportModuleToken
{
    std::string name;
    TokenVisibility visibility = TokenVisibility::Private;
};

export struct ImportIncludeLocalToken
{
    std::string name;
    TokenVisibility visibility = TokenVisibility::Private;
};

export struct ImportIncludeExternalToken
{
    std::string name;
    TokenVisibility visibility = TokenVisibility::Private;
};

export struct ImportModulePartitionToken
{
    std::string name;
    TokenVisibility visibility = TokenVisibility::Private;
};

export using Token = std::variant<
    std::monostate,
    IncludeLocalToken,
    IncludeExternalToken,
    ModuleToken,
    ModulePartitionToken,
    ImportModuleToken,
    ImportIncludeLocalToken,
    ImportIncludeExternalToken,
    ImportModulePartitionToken>;
}
