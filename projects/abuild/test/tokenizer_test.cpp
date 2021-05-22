import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

namespace std
{
auto operator<<(std::ostream &stream, const abuild::TokenVisibility &visibility) -> std::ostream &
{
    switch (visibility)
    {
    case abuild::TokenVisibility::Exported:
        return stream << "Exported";
    case abuild::TokenVisibility::Private:
        return stream << "Private";
    }

    return stream;
}

auto operator<<(std::ostream &stream, const abuild::Token &token) -> std::ostream &
{
    if (auto *value = std::get_if<std::monostate>(&token))
    {
        return stream << "{}";
    }

    if (auto *value = std::get_if<abuild::IncludeLocalToken>(&token))
    {
        return stream << value->name;
    }

    if (auto *value = std::get_if<abuild::IncludeExternalToken>(&token))
    {
        return stream << value->name;
    }

    if (auto *value = std::get_if<abuild::ModuleToken>(&token))
    {
        return stream << value->name << " [" << value->visibility << ']';
    }

    if (auto *value = std::get_if<abuild::ModulePartitionToken>(&token))
    {
        return stream << value->mod << ':' << value->name << " [" << value->visibility << ']';
    }

    if (auto *value = std::get_if<abuild::ImportModuleToken>(&token))
    {
        return stream << value->name << " [" << value->visibility << ']';
    }

    if (auto *value = std::get_if<abuild::ImportIncludeLocalToken>(&token))
    {
        return stream << value->name << " [" << value->visibility << ']';
    }

    if (auto *value = std::get_if<abuild::ImportIncludeExternalToken>(&token))
    {
        return stream << value->name << " [" << value->visibility << ']';
    }

    if (auto *value = std::get_if<abuild::ImportModulePartitionToken>(&token))
    {
        return stream << value->name << " [" << value->visibility << ']';
    }

    return stream;
}
}

namespace abuild
{
[[nodiscard]] auto operator==(const abuild::IncludeLocalToken &left, const abuild::IncludeLocalToken &right) -> bool
{
    return left.name == right.name;
}

[[nodiscard]] auto operator==(const abuild::IncludeExternalToken &left, const abuild::IncludeExternalToken &right) -> bool
{
    return left.name == right.name;
}

[[nodiscard]] auto operator==(const abuild::ModuleToken &left, const abuild::ModuleToken &right) -> bool
{
    return left.name == right.name && left.visibility == right.visibility;
}

[[nodiscard]] auto operator==(const abuild::ModulePartitionToken &left, const abuild::ModulePartitionToken &right) -> bool
{
    return left.mod == right.mod && left.name == right.name && left.visibility == right.visibility;
}

[[nodiscard]] auto operator==(const abuild::ImportModuleToken &left, const abuild::ImportModuleToken &right) -> bool
{
    return left.name == right.name && left.visibility == right.visibility;
}

[[nodiscard]] auto operator==(const abuild::ImportIncludeLocalToken &left, const abuild::ImportIncludeLocalToken &right) -> bool
{
    return left.name == right.name && left.visibility == right.visibility;
}

[[nodiscard]] auto operator==(const abuild::ImportIncludeExternalToken &left, const abuild::ImportIncludeExternalToken &right) -> bool
{
    return left.name == right.name && left.visibility == right.visibility;
}

[[nodiscard]] auto operator==(const abuild::ImportModulePartitionToken &left, const abuild::ImportModulePartitionToken &right) -> bool
{
    return left.name == right.name && left.visibility == right.visibility;
}
}

static const auto testSuite = suite("abuild::Tokenizer", [] {
    test("empty content", [] {
        expect(abuild::Tokenizer{""}.next()).toBe(abuild::Token{});
    });

    test("local include", [] {
        expect(abuild::Tokenizer{"#include \"header.hpp\""}.next()).toBe(abuild::Token{abuild::IncludeLocalToken{.name = "header.hpp"}});
        expect(abuild::Tokenizer{"#include\"header.hpp\""}.next()).toBe(abuild::Token{abuild::IncludeLocalToken{.name = "header.hpp"}});
        expect(abuild::Tokenizer{"    #   include    \"header.hpp\""}.next()).toBe(abuild::Token{abuild::IncludeLocalToken{.name = "header.hpp"}});
        expect(abuild::Tokenizer{"/*\n\n*/#include/*\n\n*/\"header.hpp\""}.next()).toBe(abuild::Token{abuild::IncludeLocalToken{.name = "header.hpp"}});
        expect(abuild::Tokenizer{"#include \"hea der.hpp\""}.next()).toBe(abuild::Token{abuild::IncludeLocalToken{.name = "hea der.hpp"}});
        expect(abuild::Tokenizer{"#include \"  header.hpp  \""}.next()).toBe(abuild::Token{abuild::IncludeLocalToken{.name = "  header.hpp  "}});
        expect(abuild::Tokenizer{"#include \"header.hpp\" /* some comment */"}.next()).toBe(abuild::Token{abuild::IncludeLocalToken{.name = "header.hpp"}});
    });

    test("external include", [] {
        expect(abuild::Tokenizer{"#include <header.hpp>"}.next()).toBe(abuild::Token{abuild::IncludeExternalToken{.name = "header.hpp"}});
        expect(abuild::Tokenizer{"#include<header.hpp>"}.next()).toBe(abuild::Token{abuild::IncludeExternalToken{.name = "header.hpp"}});
        expect(abuild::Tokenizer{"    #   include    <header.hpp>"}.next()).toBe(abuild::Token{abuild::IncludeExternalToken{.name = "header.hpp"}});
        expect(abuild::Tokenizer{"/*\n\n*/#include/*\n\n*/<header.hpp>"}.next()).toBe(abuild::Token{abuild::IncludeExternalToken{.name = "header.hpp"}});
        expect(abuild::Tokenizer{"#include <hea der.hpp>"}.next()).toBe(abuild::Token{abuild::IncludeExternalToken{.name = "hea der.hpp"}});
        expect(abuild::Tokenizer{"#include <  header.hpp  >"}.next()).toBe(abuild::Token{abuild::IncludeExternalToken{.name = "  header.hpp  "}});
        expect(abuild::Tokenizer{"#include <header.hpp> /* some comment */"}.next()).toBe(abuild::Token{abuild::IncludeExternalToken{.name = "header.hpp"}});
    });

    test("bad include", [] {
        expect(abuild::Tokenizer{"#include \"aaa"}.next()).toBe(abuild::Token{});
        expect(abuild::Tokenizer{"#include aaa\""}.next()).toBe(abuild::Token{});
        expect(abuild::Tokenizer{"#include <header.hpp\n#include \"otherheader.hpp\""}.next()).toBe(abuild::Token{abuild::IncludeLocalToken{.name = "otherheader.hpp"}});
        expect(abuild::Tokenizer{"#include \"header.hpp\n#include \"otherheader.hpp\""}.next()).toBe(abuild::Token{abuild::IncludeLocalToken{.name = "otherheader.hpp"}});
    });

    test("module", [] {
        expect(abuild::Tokenizer{"module mymodule;"}.next()).toBe(abuild::Token{abuild::ModuleToken{.name = "mymodule", .visibility = abuild::TokenVisibility::Private}});
        expect(abuild::Tokenizer{"   module     mymodule   ;"}.next()).toBe(abuild::Token{abuild::ModuleToken{.name = "mymodule", .visibility = abuild::TokenVisibility::Private}});
        expect(abuild::Tokenizer{"module\nmymodule\n;"}.next()).toBe(abuild::Token{abuild::ModuleToken{.name = "mymodule", .visibility = abuild::TokenVisibility::Private}});
        expect(abuild::Tokenizer{"/*\n\n*/module/*\n\n*/mymodule/*\n\n*/;"}.next()).toBe(abuild::Token{abuild::ModuleToken{.name = "mymodule", .visibility = abuild::TokenVisibility::Private}});
    });

    test("bad module", [] {
        expect(abuild::Tokenizer{"module mymodule"}.next()).toBe(abuild::Token{});
        expect(abuild::Tokenizer{"module my module;"}.next()).toBe(abuild::Token{});
        expect(abuild::Tokenizer{"module mymodule\n\nmodule othermodule;"}.next()).toBe(abuild::Token{abuild::ModuleToken{.name = "othermodule", .visibility = abuild::TokenVisibility::Private}});
    });

    test("module partition", [] {
        expect(abuild::Tokenizer{"module mymodule : mypartition;"}.next()).toBe(abuild::Token{abuild::ModulePartitionToken{.name = "mypartition", .mod = "mymodule", .visibility = abuild::TokenVisibility::Private}});
        expect(abuild::Tokenizer{"module mymodule:mypartition;"}.next()).toBe(abuild::Token{abuild::ModulePartitionToken{.name = "mypartition", .mod = "mymodule", .visibility = abuild::TokenVisibility::Private}});
        expect(abuild::Tokenizer{"   module    mymodule   :   mypartition  ;"}.next()).toBe(abuild::Token{abuild::ModulePartitionToken{.name = "mypartition", .mod = "mymodule", .visibility = abuild::TokenVisibility::Private}});
        expect(abuild::Tokenizer{"module\nmymodule\n:\nmypartition\n;"}.next()).toBe(abuild::Token{abuild::ModulePartitionToken{.name = "mypartition", .mod = "mymodule", .visibility = abuild::TokenVisibility::Private}});
        expect(abuild::Tokenizer{"/*\n\n*/module/*\n\n*/mymodule/*\n\n*/:/*\n\n*/mypartition/*\n\n*/;"}.next()).toBe(abuild::Token{abuild::ModulePartitionToken{.name = "mypartition", .mod = "mymodule", .visibility = abuild::TokenVisibility::Private}});
    });

    test("bad module partitions", [] {
        expect(abuild::Tokenizer{"module mymodule : mypartition"}.next()).toBe(abuild::Token{});
        expect(abuild::Tokenizer{"module mymodule : my partition;"}.next()).toBe(abuild::Token{});
        expect(abuild::Tokenizer{"module mymodule : mypartition\n\nmodule mymodule : otherpartition;"}.next()).toBe(abuild::Token{abuild::ModulePartitionToken{.name = "otherpartition", .mod = "mymodule", .visibility = abuild::TokenVisibility::Private}});
    });

    test("import module", [] {
        expect(abuild::Tokenizer{"import mymodule;"}.next()).toBe(abuild::Token{abuild::ImportModuleToken{.name = "mymodule", .visibility = abuild::TokenVisibility::Private}});
        expect(abuild::Tokenizer{"   import    mymodule   ;"}.next()).toBe(abuild::Token{abuild::ImportModuleToken{.name = "mymodule", .visibility = abuild::TokenVisibility::Private}});
        expect(abuild::Tokenizer{"import\n  mymodule  \n;"}.next()).toBe(abuild::Token{abuild::ImportModuleToken{.name = "mymodule", .visibility = abuild::TokenVisibility::Private}});
        expect(abuild::Tokenizer{"/*\n\n*/import/*\n\n*/mymodule/*\n*/;"}.next()).toBe(abuild::Token{abuild::ImportModuleToken{.name = "mymodule", .visibility = abuild::TokenVisibility::Private}});
    });

    test("import module partition", [] {
        expect(abuild::Tokenizer{"import : mypartition;"}.next()).toBe(abuild::Token{abuild::ImportModulePartitionToken{.name = "mypartition", .visibility = abuild::TokenVisibility::Private}});
        expect(abuild::Tokenizer{"  import:   mypartition;"}.next()).toBe(abuild::Token{abuild::ImportModulePartitionToken{.name = "mypartition", .visibility = abuild::TokenVisibility::Private}});
        expect(abuild::Tokenizer{"import:mypartition;"}.next()).toBe(abuild::Token{abuild::ImportModulePartitionToken{.name = "mypartition", .visibility = abuild::TokenVisibility::Private}});
        expect(abuild::Tokenizer{"import\n:\n\nmypartition\n\n\n;"}.next()).toBe(abuild::Token{abuild::ImportModulePartitionToken{.name = "mypartition", .visibility = abuild::TokenVisibility::Private}});
        expect(abuild::Tokenizer{"/*\n\n*/import/*\n*/:/*\n*/mypartition/*\n\n*/;"}.next()).toBe(abuild::Token{abuild::ImportModulePartitionToken{.name = "mypartition", .visibility = abuild::TokenVisibility::Private}});
    });

    test("import local include", [] {
        expect(abuild::Tokenizer{"import \"header.hpp\";"}.next()).toBe(abuild::Token{abuild::ImportIncludeLocalToken{.name = "header.hpp", .visibility = abuild::TokenVisibility::Private}});
        expect(abuild::Tokenizer{"import\"header.hpp\";"}.next()).toBe(abuild::Token{abuild::ImportIncludeLocalToken{.name = "header.hpp", .visibility = abuild::TokenVisibility::Private}});
        expect(abuild::Tokenizer{"   import   \"header.hpp\"   ;"}.next()).toBe(abuild::Token{abuild::ImportIncludeLocalToken{.name = "header.hpp", .visibility = abuild::TokenVisibility::Private}});
        expect(abuild::Tokenizer{"import \"  header.hpp  \";"}.next()).toBe(abuild::Token{abuild::ImportIncludeLocalToken{.name = "  header.hpp  ", .visibility = abuild::TokenVisibility::Private}});
        expect(abuild::Tokenizer{"import\n\n\"header.hpp\"\n\n;"}.next()).toBe(abuild::Token{abuild::ImportIncludeLocalToken{.name = "header.hpp", .visibility = abuild::TokenVisibility::Private}});
        expect(abuild::Tokenizer{"/*\n\n*/import/*\n\n*/\"header.hpp\"/*\n\n*/;"}.next()).toBe(abuild::Token{abuild::ImportIncludeLocalToken{.name = "header.hpp", .visibility = abuild::TokenVisibility::Private}});
    });

    test("import external include", [] {
        expect(abuild::Tokenizer{"import <header.hpp>;"}.next()).toBe(abuild::Token{abuild::ImportIncludeExternalToken{.name = "header.hpp", .visibility = abuild::TokenVisibility::Private}});
        expect(abuild::Tokenizer{"import<header.hpp>;"}.next()).toBe(abuild::Token{abuild::ImportIncludeExternalToken{.name = "header.hpp", .visibility = abuild::TokenVisibility::Private}});
        expect(abuild::Tokenizer{"   import   <header.hpp>   ;"}.next()).toBe(abuild::Token{abuild::ImportIncludeExternalToken{.name = "header.hpp", .visibility = abuild::TokenVisibility::Private}});
        expect(abuild::Tokenizer{"import <  header.hpp  >;"}.next()).toBe(abuild::Token{abuild::ImportIncludeExternalToken{.name = "  header.hpp  ", .visibility = abuild::TokenVisibility::Private}});
        expect(abuild::Tokenizer{"import\n\n<header.hpp>\n\n;"}.next()).toBe(abuild::Token{abuild::ImportIncludeExternalToken{.name = "header.hpp", .visibility = abuild::TokenVisibility::Private}});
        expect(abuild::Tokenizer{"/*\n\n*/import/*\n\n*/<header.hpp>/*\n\n*/;"}.next()).toBe(abuild::Token{abuild::ImportIncludeExternalToken{.name = "header.hpp", .visibility = abuild::TokenVisibility::Private}});
    });

    test("export module", [] {
        expect(abuild::Tokenizer{"export module mymodule;"}.next()).toBe(abuild::Token{abuild::ModuleToken{.name = "mymodule", .visibility = abuild::TokenVisibility::Exported}});
        expect(abuild::Tokenizer{"   export   module    mymodule   ;"}.next()).toBe(abuild::Token{abuild::ModuleToken{.name = "mymodule", .visibility = abuild::TokenVisibility::Exported}});
        expect(abuild::Tokenizer{"export\nmodule\nmymodule\n;"}.next()).toBe(abuild::Token{abuild::ModuleToken{.name = "mymodule", .visibility = abuild::TokenVisibility::Exported}});
        expect(abuild::Tokenizer{"/*\n*/export/*\n\n*/module/*\n\n*/mymodule/*\n*/;"}.next()).toBe(abuild::Token{abuild::ModuleToken{.name = "mymodule", .visibility = abuild::TokenVisibility::Exported}});
    });

    test("export import module", [] {
        expect(abuild::Tokenizer{"export import mymodule;"}.next()).toBe(abuild::Token{abuild::ImportModuleToken{.name = "mymodule", .visibility = abuild::TokenVisibility::Exported}});
        expect(abuild::Tokenizer{"   export   import    mymodule   ;"}.next()).toBe(abuild::Token{abuild::ImportModuleToken{.name = "mymodule", .visibility = abuild::TokenVisibility::Exported}});
        expect(abuild::Tokenizer{"export\nimport\nmymodule\n;"}.next()).toBe(abuild::Token{abuild::ImportModuleToken{.name = "mymodule", .visibility = abuild::TokenVisibility::Exported}});
        expect(abuild::Tokenizer{"/*\n*/export/*\n\n*/import/*\n\n*/mymodule/*\n*/;"}.next()).toBe(abuild::Token{abuild::ImportModuleToken{.name = "mymodule", .visibility = abuild::TokenVisibility::Exported}});
    });

    test("export import module partition", [] {
        expect(abuild::Tokenizer{"export import : mypartition;"}.next()).toBe(abuild::Token{abuild::ImportModulePartitionToken{.name = "mypartition", .visibility = abuild::TokenVisibility::Exported}});
        expect(abuild::Tokenizer{" export import:   mypartition;"}.next()).toBe(abuild::Token{abuild::ImportModulePartitionToken{.name = "mypartition", .visibility = abuild::TokenVisibility::Exported}});
        expect(abuild::Tokenizer{"export import:mypartition;"}.next()).toBe(abuild::Token{abuild::ImportModulePartitionToken{.name = "mypartition", .visibility = abuild::TokenVisibility::Exported}});
        expect(abuild::Tokenizer{"export\nimport\n:\n\nmypartition\n\n\n;"}.next()).toBe(abuild::Token{abuild::ImportModulePartitionToken{.name = "mypartition", .visibility = abuild::TokenVisibility::Exported}});
        expect(abuild::Tokenizer{"/*\n*/export/*\n\n*/import/*\n*/:/*\n*/mypartition/*\n\n*/;"}.next()).toBe(abuild::Token{abuild::ImportModulePartitionToken{.name = "mypartition", .visibility = abuild::TokenVisibility::Exported}});
    });

    test("export import local include", [] {
        expect(abuild::Tokenizer{"export import \"header.hpp\";"}.next()).toBe(abuild::Token{abuild::ImportIncludeLocalToken{.name = "header.hpp", .visibility = abuild::TokenVisibility::Exported}});
        expect(abuild::Tokenizer{"export import\"header.hpp\";"}.next()).toBe(abuild::Token{abuild::ImportIncludeLocalToken{.name = "header.hpp", .visibility = abuild::TokenVisibility::Exported}});
        expect(abuild::Tokenizer{"  export    import   \"header.hpp\"   ;"}.next()).toBe(abuild::Token{abuild::ImportIncludeLocalToken{.name = "header.hpp", .visibility = abuild::TokenVisibility::Exported}});
        expect(abuild::Tokenizer{"export import \"  header.hpp  \";"}.next()).toBe(abuild::Token{abuild::ImportIncludeLocalToken{.name = "  header.hpp  ", .visibility = abuild::TokenVisibility::Exported}});
        expect(abuild::Tokenizer{"export\nimport\n\n\"header.hpp\"\n\n;"}.next()).toBe(abuild::Token{abuild::ImportIncludeLocalToken{.name = "header.hpp", .visibility = abuild::TokenVisibility::Exported}});
        expect(abuild::Tokenizer{"/*\n\n*/export/*\n\n*/import/*\n\n*/\"header.hpp\"/*\n\n*/;"}.next()).toBe(abuild::Token{abuild::ImportIncludeLocalToken{.name = "header.hpp", .visibility = abuild::TokenVisibility::Exported}});
    });

    test("export import external include", [] {
        expect(abuild::Tokenizer{"export import <header.hpp>;"}.next()).toBe(abuild::Token{abuild::ImportIncludeExternalToken{.name = "header.hpp", .visibility = abuild::TokenVisibility::Exported}});
        expect(abuild::Tokenizer{"export import<header.hpp>;"}.next()).toBe(abuild::Token{abuild::ImportIncludeExternalToken{.name = "header.hpp", .visibility = abuild::TokenVisibility::Exported}});
        expect(abuild::Tokenizer{"   export    import   <header.hpp>   ;"}.next()).toBe(abuild::Token{abuild::ImportIncludeExternalToken{.name = "header.hpp", .visibility = abuild::TokenVisibility::Exported}});
        expect(abuild::Tokenizer{"export import <  header.hpp  >;"}.next()).toBe(abuild::Token{abuild::ImportIncludeExternalToken{.name = "  header.hpp  ", .visibility = abuild::TokenVisibility::Exported}});
        expect(abuild::Tokenizer{"export\n\nimport\n\n<header.hpp>\n\n;"}.next()).toBe(abuild::Token{abuild::ImportIncludeExternalToken{.name = "header.hpp", .visibility = abuild::TokenVisibility::Exported}});
        expect(abuild::Tokenizer{"/*\n\n*/export/*\n\n*/import/*\n\n*/<header.hpp>/*\n\n*/;"}.next()).toBe(abuild::Token{abuild::ImportIncludeExternalToken{.name = "header.hpp", .visibility = abuild::TokenVisibility::Exported}});
    });

    test("export other tokens", [] {
        expect(abuild::Tokenizer{"export template<typename T> struct S;"}.next()).toBe(abuild::Token{});
        expect(abuild::Tokenizer{"export void foo()\n{\n}\n"}.next()).toBe(abuild::Token{});
        expect(abuild::Tokenizer{"export class C;"}.next()).toBe(abuild::Token{});
    });

    test("multiple tokens", [] {
        abuild::Tokenizer tokenizer{"module;\nexport module mymodule;\nimport othermodule;\nexport import <vector>;\n#include \"my/header.hpp\"\nimport : mypartition;"};
        std::vector<abuild::Token> tokens;

        for (abuild::Token token = tokenizer.next(); token != abuild::Token{}; token = tokenizer.next())
        {
            tokens.push_back(std::move(token));
        }

        expect(tokens).toBe(std::vector<abuild::Token>{
            abuild::Token{abuild::ModuleToken{.name = "mymodule", .visibility = abuild::TokenVisibility::Exported}},
            abuild::Token{abuild::ImportModuleToken{.name = "othermodule", .visibility = abuild::TokenVisibility::Private}},
            abuild::Token{abuild::ImportIncludeExternalToken{.name = "vector", .visibility = abuild::TokenVisibility::Exported}},
            abuild::Token{abuild::IncludeLocalToken{.name = "my/header.hpp"}},
            abuild::Token{abuild::ImportModulePartitionToken{.name = "mypartition", .visibility = abuild::TokenVisibility::Private}}});
    });

    test("multiple tokens with other code", [] {
        abuild::Tokenizer tokenizer{"module;\n/*\n some comment\n*/\n//some other comment\nexport module mymodule;\nauto foo() -> void\n{\n}\n\n\nimport othermodule;\nexport import <vector>;\nexport template<typename T> struct S\n{\n};\n\n#include \"my/header.hpp\"\nclass C;\nimport : mypartition;"};
        std::vector<abuild::Token> tokens;

        for (abuild::Token token = tokenizer.next(); token != abuild::Token{}; token = tokenizer.next())
        {
            tokens.push_back(std::move(token));
        }

        expect(tokens).toBe(std::vector<abuild::Token>{
            abuild::Token{abuild::ModuleToken{.name = "mymodule", .visibility = abuild::TokenVisibility::Exported}},
            abuild::Token{abuild::ImportModuleToken{.name = "othermodule", .visibility = abuild::TokenVisibility::Private}},
            abuild::Token{abuild::ImportIncludeExternalToken{.name = "vector", .visibility = abuild::TokenVisibility::Exported}},
            abuild::Token{abuild::IncludeLocalToken{.name = "my/header.hpp"}},
            abuild::Token{abuild::ImportModulePartitionToken{.name = "mypartition", .visibility = abuild::TokenVisibility::Private}}});
    });

    test("string literals", [] {
        expect(abuild::Tokenizer{"const char *c = \"import : myotherpartition;import : quoted;\";"}.next()).toBe(abuild::Token{});
        expect(abuild::Tokenizer{"const char *c = R\"(import : myotherpartition;import : quoted;)\";"}.next()).toBe(abuild::Token{});
        expect(abuild::Tokenizer{"const char *c = R\"asd(import : myotherpartition;import : quoted;)asd\";"}.next()).toBe(abuild::Token{});
    });

    test("string literals with real tokens", [] {
        abuild::Tokenizer tokenizer{"const char *c = R\"asd(import : myotherpartition;import : quoted;)asd\";import mymodule;c = \"import othermodule;\";import yetanothermodule;"};
        std::vector<abuild::Token> tokens;

        for (abuild::Token token = tokenizer.next(); token != abuild::Token{}; token = tokenizer.next())
        {
            tokens.push_back(std::move(token));
        }

        expect(tokens).toBe(std::vector<abuild::Token>{
            abuild::Token{abuild::ImportModuleToken{.name = "mymodule", .visibility = abuild::TokenVisibility::Private}},
            abuild::Token{abuild::ImportModuleToken{.name = "yetanothermodule", .visibility = abuild::TokenVisibility::Private}}});
    });
});
