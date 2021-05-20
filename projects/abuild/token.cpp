#ifdef _MSC_VER
export module abuild : tokenizer;
import<astl.hpp>;
#endif

namespace abuild
{
enum class TokenVisibility
{
    Exported,
    Private
};

struct IncludeLocalToken
{
    std::string name;
};

struct IncludeExternalToken
{
    std::string name;
};

struct ModuleToken
{
    std::string name;
    TokenVisibility visibility = TokenVisibility::Private;
};

struct ModulePartitionToken
{
    std::string mod;
    std::string name;
    TokenVisibility visibility = TokenVisibility::Private;
};

struct ImportModuleToken
{
    std::string name;
    TokenVisibility visibility = TokenVisibility::Private;
};

struct ImportIncludeLocalToken
{
    std::string name;
    TokenVisibility visibility = TokenVisibility::Private;
};

struct ImportIncludeExternalToken
{
    std::string name;
    TokenVisibility visibility = TokenVisibility::Private;
};

struct ImportModulePartitionToken
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
