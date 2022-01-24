#ifndef __clang__
export module abuild.cpptokenizer : token;
export import : if_token;
#endif

namespace abuild
{
//! Represents a preprocessor token `#define`.
//! E.g. `#define MY_MACRO 1`.
export struct DefineToken
{
    //! Define name
    std::string name;

    //! Define value
    std::string value;

    friend auto operator==(const DefineToken &, const DefineToken &) -> bool = default;
};

//! Represents an import statement of an external
//! `<header>`. E.g. `import <vector>;`.
export struct ImportIncludeExternalToken
{
    //! Header name or path.
    std::string name;

    //! Flag representing whether the token was
    //! `export`ed.
    bool exported = false;

    friend auto operator==(const ImportIncludeExternalToken &, const ImportIncludeExternalToken &) -> bool = default;
};

//! Represents an import statement of a local
//! `"header"`. E.g. `import "myheader.h";`.
export struct ImportIncludeLocalToken
{
    //! Header name or path.
    std::string name;

    //! Flag representing whether the token was
    //! `export`ed.
    bool exported = false;

    friend auto operator==(const ImportIncludeLocalToken &, const ImportIncludeLocalToken &) -> bool = default;
};

//! Represents an import statement of a module
//! partition. E.q. `import : mypartition;`.
export struct ImportModulePartitionToken
{
    //! Partition name.
    std::string name;

    //! Flag representing whether the token was
    //! `export`ed.
    bool exported = false;

    friend auto operator==(const ImportModulePartitionToken &, const ImportModulePartitionToken &) -> bool = default;
};

//! Represents na import statement of a module.
//! E.q. `import mymodule;`.
export struct ImportModuleToken
{
    //! Module name.
    std::string name;

    //! Flag representing whether the token was
    //! `export`ed.
    bool exported = false;

    friend auto operator==(const ImportModuleToken &, const ImportModuleToken &) -> bool = default;
};

//! Represents a preprocessor token `#include` of
//! an external header. E.g. `#include <vector>`.
export struct IncludeExternalToken
{
    //! Header name or path.
    std::string name;

    friend auto operator==(const IncludeExternalToken &, const IncludeExternalToken &) -> bool = default;
};

//! Represents a preprocessor token `#include` of
//! a local header. E.g. `#include "myheader.h"`.
export struct IncludeLocalToken
{
    //! Header name or path.
    std::string name;

    friend auto operator==(const IncludeLocalToken &, const IncludeLocalToken &) -> bool = default;
};

//! Represents a declaration of a module
//! partition. E.g. `module mymodule :
//! mypartition;`.
export struct ModulePartitionToken
{
    //! Module name.
    std::string mod;

    //! Module partition name.
    std::string name;

    //! Flag representing whether the token was
    //! `export`ed.
    bool exported = false;

    friend auto operator==(const ModulePartitionToken &, const ModulePartitionToken &) -> bool = default;
};

//! Represents a declaration of a module. E.g.
//! `module mymodule;`.
export struct ModuleToken
{
    //! Module name.
    std::string name;

    //! Flag representing whether the token was
    //! `export`ed.
    bool exported = false;

    friend auto operator==(const ModuleToken &, const ModuleToken &) -> bool = default;
};

//! Represents a preprocessor token `#undefine`.
//! E.g. `#undefine MY_MACRO`.
export struct UndefToken
{
    //! Define name
    std::string name;

    friend auto operator==(const UndefToken &, const UndefToken &) -> bool = default;
};

//! Token is a variant type combining all possible
//! token types.
export using Token = std::variant<
    IfToken,
    ElseToken,
    EndIfToken,
    DefineToken,
    ImportIncludeExternalToken,
    ImportIncludeLocalToken,
    ImportModulePartitionToken,
    ImportModuleToken,
    IncludeExternalToken,
    IncludeLocalToken,
    ModulePartitionToken,
    ModuleToken,
    UndefToken>;
}
