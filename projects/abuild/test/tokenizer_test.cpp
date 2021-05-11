import abuild;
import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

namespace std
{
auto operator<<(std::ostream &stream, const abuild::Token::Type &type) -> std::ostream &
{
    switch (type)
    {
    case abuild::Token::Type::None:
        return stream << "None";
    case abuild::Token::Type::IncludeLocal:
        return stream << "IncludeLocal";
    case abuild::Token::Type::IncludeExternal:
        return stream << "IncludeExternal";
    case abuild::Token::Type::Module:
        return stream << "Module";
    case abuild::Token::Type::ModulePartition:
        return stream << "ModulePartition";
    case abuild::Token::Type::ImportModule:
        return stream << "ImportModule";
    case abuild::Token::Type::ImportIncludeLocal:
        return stream << "ImportIncludeLocal";
    case abuild::Token::Type::ImportIncludeExternal:
        return stream << "ImportIncludeExternal";
    case abuild::Token::Type::ImportModulePartition:
        return stream << "ImportModulePartition";
    }

    return stream;
}

auto operator<<(std::ostream &stream, const abuild::Token &token) -> std::ostream &
{
    return stream << token.moduleName << (token.moduleName.empty() ? "" : ":") << token.name << '[' << token.type << ']' << (token.exported ? "[exported]" : "");
}
}

namespace abuild
{
[[nodiscard]] auto operator==(const abuild::Token &left, const abuild::Token &right) -> bool
{
    return left.name == right.name
        && left.moduleName == right.moduleName
        && left.type == right.type
        && left.exported == right.exported;
}
}

static const auto testSuite = suite("abuild::Tokenizer", [] {
    test("empty content", [] {
        expect(abuild::Tokenizer{""}.next().has_value()).toBe(false);
    });

    test("local include", [] {
        expect(*abuild::Tokenizer{"#include \"header.hpp\""}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::IncludeLocal, false});
        expect(*abuild::Tokenizer{"#include\"header.hpp\""}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::IncludeLocal, false});
        expect(*abuild::Tokenizer{"    #   include    \"header.hpp\""}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::IncludeLocal, false});
        expect(*abuild::Tokenizer{"/*\n\n*/#include/*\n\n*/\"header.hpp\""}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::IncludeLocal, false});
        expect(*abuild::Tokenizer{"#include \"hea der.hpp\""}.next()).toBe(abuild::Token{"hea der.hpp", "", abuild::Token::Type::IncludeLocal, false});
        expect(*abuild::Tokenizer{"#include \"  header.hpp  \""}.next()).toBe(abuild::Token{"  header.hpp  ", "", abuild::Token::Type::IncludeLocal, false});
        expect(*abuild::Tokenizer{"#include \"header.hpp\" /* some comment */"}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::IncludeLocal, false});
    });

    test("external include", [] {
        expect(*abuild::Tokenizer{"#include <header.hpp>"}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::IncludeExternal, false});
        expect(*abuild::Tokenizer{"#include<header.hpp>"}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::IncludeExternal, false});
        expect(*abuild::Tokenizer{"    #   include    <header.hpp>"}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::IncludeExternal, false});
        expect(*abuild::Tokenizer{"/*\n\n*/#include/*\n\n*/<header.hpp>"}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::IncludeExternal, false});
        expect(*abuild::Tokenizer{"#include <hea der.hpp>"}.next()).toBe(abuild::Token{"hea der.hpp", "", abuild::Token::Type::IncludeExternal, false});
        expect(*abuild::Tokenizer{"#include <  header.hpp  >"}.next()).toBe(abuild::Token{"  header.hpp  ", "", abuild::Token::Type::IncludeExternal, false});
        expect(*abuild::Tokenizer{"#include <header.hpp> /* some comment */"}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::IncludeExternal, false});
    });

    test("bad include", [] {
        expect(abuild::Tokenizer{"#include \"aaa"}.next().has_value()).toBe(false);
        expect(abuild::Tokenizer{"#include aaa\""}.next().has_value()).toBe(false);
        expect(*abuild::Tokenizer{"#include <header.hpp\n#include \"otherheader.hpp\""}.next()).toBe(abuild::Token{"otherheader.hpp", "", abuild::Token::Type::IncludeLocal, false});
        expect(*abuild::Tokenizer{"#include \"header.hpp\n#include \"otherheader.hpp\""}.next()).toBe(abuild::Token{"otherheader.hpp", "", abuild::Token::Type::IncludeLocal, false});
    });

    test("module", [] {
        expect(*abuild::Tokenizer{"module mymodule;"}.next()).toBe(abuild::Token{"mymodule", "", abuild::Token::Type::Module, false});
        expect(*abuild::Tokenizer{"   module     mymodule   ;"}.next()).toBe(abuild::Token{"mymodule", "", abuild::Token::Type::Module, false});
        expect(*abuild::Tokenizer{"module\nmymodule\n;"}.next()).toBe(abuild::Token{"mymodule", "", abuild::Token::Type::Module, false});
        expect(*abuild::Tokenizer{"/*\n\n*/module/*\n\n*/mymodule/*\n\n*/;"}.next()).toBe(abuild::Token{"mymodule", "", abuild::Token::Type::Module, false});
    });

    test("bad module", [] {
        expect(abuild::Tokenizer{"module mymodule"}.next().has_value()).toBe(false);
        expect(abuild::Tokenizer{"module my module;"}.next().has_value()).toBe(false);
        expect(*abuild::Tokenizer{"module mymodule\n\nmodule othermodule;"}.next()).toBe(abuild::Token{"othermodule", "", abuild::Token::Type::Module, false});
    });

    test("module partition", [] {
        expect(*abuild::Tokenizer{"module mymodule : mypartition;"}.next()).toBe(abuild::Token{"mypartition", "mymodule", abuild::Token::Type::ModulePartition, false});
        expect(*abuild::Tokenizer{"module mymodule:mypartition;"}.next()).toBe(abuild::Token{"mypartition", "mymodule", abuild::Token::Type::ModulePartition, false});
        expect(*abuild::Tokenizer{"   module    mymodule   :   mypartition  ;"}.next()).toBe(abuild::Token{"mypartition", "mymodule", abuild::Token::Type::ModulePartition, false});
        expect(*abuild::Tokenizer{"module\nmymodule\n:\nmypartition\n;"}.next()).toBe(abuild::Token{"mypartition", "mymodule", abuild::Token::Type::ModulePartition, false});
        expect(*abuild::Tokenizer{"/*\n\n*/module/*\n\n*/mymodule/*\n\n*/:/*\n\n*/mypartition/*\n\n*/;"}.next()).toBe(abuild::Token{"mypartition", "mymodule", abuild::Token::Type::ModulePartition, false});
    });

    test("bad module partitions", [] {
        expect(abuild::Tokenizer{"module mymodule : mypartition"}.next().has_value()).toBe(false);
        expect(abuild::Tokenizer{"module mymodule : my partition;"}.next().has_value()).toBe(false);
        expect(*abuild::Tokenizer{"module mymodule : mypartition\n\nmodule mymodule : otherpartition;"}.next()).toBe(abuild::Token{"otherpartition", "mymodule", abuild::Token::Type::ModulePartition, false});
    });

    test("import module", [] {
        expect(*abuild::Tokenizer{"import mymodule;"}.next()).toBe(abuild::Token{"mymodule", "", abuild::Token::Type::ImportModule, false});
        expect(*abuild::Tokenizer{"   import    mymodule   ;"}.next()).toBe(abuild::Token{"mymodule", "", abuild::Token::Type::ImportModule, false});
        expect(*abuild::Tokenizer{"import\n  mymodule  \n;"}.next()).toBe(abuild::Token{"mymodule", "", abuild::Token::Type::ImportModule, false});
        expect(*abuild::Tokenizer{"/*\n\n*/import/*\n\n*/mymodule/*\n*/;"}.next()).toBe(abuild::Token{"mymodule", "", abuild::Token::Type::ImportModule, false});
    });

    test("import module partition", [] {
        expect(*abuild::Tokenizer{"import : mypartition;"}.next()).toBe(abuild::Token{"mypartition", "", abuild::Token::Type::ImportModulePartition, false});
        expect(*abuild::Tokenizer{"  import:   mypartition;"}.next()).toBe(abuild::Token{"mypartition", "", abuild::Token::Type::ImportModulePartition, false});
        expect(*abuild::Tokenizer{"import:mypartition;"}.next()).toBe(abuild::Token{"mypartition", "", abuild::Token::Type::ImportModulePartition, false});
        expect(*abuild::Tokenizer{"import\n:\n\nmypartition\n\n\n;"}.next()).toBe(abuild::Token{"mypartition", "", abuild::Token::Type::ImportModulePartition, false});
        expect(*abuild::Tokenizer{"/*\n\n*/import/*\n*/:/*\n*/mypartition/*\n\n*/;"}.next()).toBe(abuild::Token{"mypartition", "", abuild::Token::Type::ImportModulePartition, false});
    });

    test("import local include", [] {
        expect(*abuild::Tokenizer{"import \"header.hpp\";"}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::ImportIncludeLocal, false});
        expect(*abuild::Tokenizer{"import\"header.hpp\";"}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::ImportIncludeLocal, false});
        expect(*abuild::Tokenizer{"   import   \"header.hpp\"   ;"}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::ImportIncludeLocal, false});
        expect(*abuild::Tokenizer{"import \"  header.hpp  \";"}.next()).toBe(abuild::Token{"  header.hpp  ", "", abuild::Token::Type::ImportIncludeLocal, false});
        expect(*abuild::Tokenizer{"import\n\n\"header.hpp\"\n\n;"}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::ImportIncludeLocal, false});
        expect(*abuild::Tokenizer{"/*\n\n*/import/*\n\n*/\"header.hpp\"/*\n\n*/;"}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::ImportIncludeLocal, false});
    });

    test("import external include", [] {
        expect(*abuild::Tokenizer{"import <header.hpp>;"}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::ImportIncludeExternal, false});
        expect(*abuild::Tokenizer{"import<header.hpp>;"}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::ImportIncludeExternal, false});
        expect(*abuild::Tokenizer{"   import   <header.hpp>   ;"}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::ImportIncludeExternal, false});
        expect(*abuild::Tokenizer{"import <  header.hpp  >;"}.next()).toBe(abuild::Token{"  header.hpp  ", "", abuild::Token::Type::ImportIncludeExternal, false});
        expect(*abuild::Tokenizer{"import\n\n<header.hpp>\n\n;"}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::ImportIncludeExternal, false});
        expect(*abuild::Tokenizer{"/*\n\n*/import/*\n\n*/<header.hpp>/*\n\n*/;"}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::ImportIncludeExternal, false});
    });

    test("export module", [] {
        expect(*abuild::Tokenizer{"export module mymodule;"}.next()).toBe(abuild::Token{"mymodule", "", abuild::Token::Type::Module, true});
        expect(*abuild::Tokenizer{"   export   module    mymodule   ;"}.next()).toBe(abuild::Token{"mymodule", "", abuild::Token::Type::Module, true});
        expect(*abuild::Tokenizer{"export\nmodule\nmymodule\n;"}.next()).toBe(abuild::Token{"mymodule", "", abuild::Token::Type::Module, true});
        expect(*abuild::Tokenizer{"/*\n*/export/*\n\n*/module/*\n\n*/mymodule/*\n*/;"}.next()).toBe(abuild::Token{"mymodule", "", abuild::Token::Type::Module, true});
    });

    test("export import module", [] {
        expect(*abuild::Tokenizer{"export import mymodule;"}.next()).toBe(abuild::Token{"mymodule", "", abuild::Token::Type::ImportModule, true});
        expect(*abuild::Tokenizer{"   export   import    mymodule   ;"}.next()).toBe(abuild::Token{"mymodule", "", abuild::Token::Type::ImportModule, true});
        expect(*abuild::Tokenizer{"export\nimport\nmymodule\n;"}.next()).toBe(abuild::Token{"mymodule", "", abuild::Token::Type::ImportModule, true});
        expect(*abuild::Tokenizer{"/*\n*/export/*\n\n*/import/*\n\n*/mymodule/*\n*/;"}.next()).toBe(abuild::Token{"mymodule", "", abuild::Token::Type::ImportModule, true});
    });

    test("export import module partition", [] {
        expect(*abuild::Tokenizer{"export import : mypartition;"}.next()).toBe(abuild::Token{"mypartition", "", abuild::Token::Type::ImportModulePartition, true});
        expect(*abuild::Tokenizer{" export import:   mypartition;"}.next()).toBe(abuild::Token{"mypartition", "", abuild::Token::Type::ImportModulePartition, true});
        expect(*abuild::Tokenizer{"export import:mypartition;"}.next()).toBe(abuild::Token{"mypartition", "", abuild::Token::Type::ImportModulePartition, true});
        expect(*abuild::Tokenizer{"export\nimport\n:\n\nmypartition\n\n\n;"}.next()).toBe(abuild::Token{"mypartition", "", abuild::Token::Type::ImportModulePartition, true});
        expect(*abuild::Tokenizer{"/*\n*/export/*\n\n*/import/*\n*/:/*\n*/mypartition/*\n\n*/;"}.next()).toBe(abuild::Token{"mypartition", "", abuild::Token::Type::ImportModulePartition, true});
    });

    test("export import local include", [] {
        expect(*abuild::Tokenizer{"export import \"header.hpp\";"}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::ImportIncludeLocal, true});
        expect(*abuild::Tokenizer{"export import\"header.hpp\";"}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::ImportIncludeLocal, true});
        expect(*abuild::Tokenizer{"  export    import   \"header.hpp\"   ;"}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::ImportIncludeLocal, true});
        expect(*abuild::Tokenizer{"export import \"  header.hpp  \";"}.next()).toBe(abuild::Token{"  header.hpp  ", "", abuild::Token::Type::ImportIncludeLocal, true});
        expect(*abuild::Tokenizer{"export\nimport\n\n\"header.hpp\"\n\n;"}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::ImportIncludeLocal, true});
        expect(*abuild::Tokenizer{"/*\n\n*/export/*\n\n*/import/*\n\n*/\"header.hpp\"/*\n\n*/;"}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::ImportIncludeLocal, true});
    });

    test("export import external include", [] {
        expect(*abuild::Tokenizer{"export import <header.hpp>;"}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::ImportIncludeExternal, true});
        expect(*abuild::Tokenizer{"export import<header.hpp>;"}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::ImportIncludeExternal, true});
        expect(*abuild::Tokenizer{"   export    import   <header.hpp>   ;"}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::ImportIncludeExternal, true});
        expect(*abuild::Tokenizer{"export import <  header.hpp  >;"}.next()).toBe(abuild::Token{"  header.hpp  ", "", abuild::Token::Type::ImportIncludeExternal, true});
        expect(*abuild::Tokenizer{"export\n\nimport\n\n<header.hpp>\n\n;"}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::ImportIncludeExternal, true});
        expect(*abuild::Tokenizer{"/*\n\n*/export/*\n\n*/import/*\n\n*/<header.hpp>/*\n\n*/;"}.next()).toBe(abuild::Token{"header.hpp", "", abuild::Token::Type::ImportIncludeExternal, true});
    });

    test("export other tokens", [] {
        expect(abuild::Tokenizer{"export template<typename T> struct S;"}.next().has_value()).toBe(false);
        expect(abuild::Tokenizer{"export void foo()\n{\n}\n"}.next().has_value()).toBe(false);
        expect(abuild::Tokenizer{"export class C;"}.next().has_value()).toBe(false);
    });

    test("multiple tokens", [] {
        abuild::Tokenizer tokenizer{"module;\nexport module mymodule;\nimport othermodule;\nexport import <vector>;\n#include \"my/header.hpp\"\nimport : mypartition;"};
        std::vector<abuild::Token> tokens;

        while (const auto token = tokenizer.next())
        {
            tokens.push_back(*token);
        }

        expect(tokens).toBe(std::vector<abuild::Token>{
            abuild::Token{"mymodule", "", abuild::Token::Type::Module, true},
            abuild::Token{"othermodule", "", abuild::Token::Type::ImportModule, false},
            abuild::Token{"vector", "", abuild::Token::Type::ImportIncludeExternal, true},
            abuild::Token{"my/header.hpp", "", abuild::Token::Type::IncludeLocal, false},
            abuild::Token{"mypartition", "", abuild::Token::Type::ImportModulePartition, false}});
    });

    test("multiple tokens with other code", [] {
        abuild::Tokenizer tokenizer{"module;\n/*\n some comment\n*/\n//some other comment\nexport module mymodule;\nauto foo() -> void\n{\n}\n\n\nimport othermodule;\nexport import <vector>;\nexport template<typename T> struct S\n{\n};\n\n#include \"my/header.hpp\"\nclass C;\nimport : mypartition;"};
        std::vector<abuild::Token> tokens;

        while (const auto token = tokenizer.next())
        {
            tokens.push_back(*token);
        }

        expect(tokens).toBe(std::vector<abuild::Token>{
            abuild::Token{"mymodule", "", abuild::Token::Type::Module, true},
            abuild::Token{"othermodule", "", abuild::Token::Type::ImportModule, false},
            abuild::Token{"vector", "", abuild::Token::Type::ImportIncludeExternal, true},
            abuild::Token{"my/header.hpp", "", abuild::Token::Type::IncludeLocal, false},
            abuild::Token{"mypartition", "", abuild::Token::Type::ImportModulePartition, false}});
    });

    test("string literals", [] {
        expect(abuild::Tokenizer{"const char *c = \"import : myotherpartition;import : quoted;\";"}.next().has_value()).toBe(false);
        expect(abuild::Tokenizer{"const char *c = R\"(import : myotherpartition;import : quoted;)\";"}.next().has_value()).toBe(false);
        expect(abuild::Tokenizer{"const char *c = R\"asd(import : myotherpartition;import : quoted;)asd\";"}.next().has_value()).toBe(false);
    });

    test("string literals with real tokens", [] {
        abuild::Tokenizer tokenizer{"const char *c = R\"asd(import : myotherpartition;import : quoted;)asd\";import mymodule;c = \"import othermodule;\";import yetanothermodule;"};
        std::vector<abuild::Token> tokens;

        while (const auto token = tokenizer.next())
        {
            tokens.push_back(*token);
        }

        expect(tokens).toBe(std::vector<abuild::Token>{
            abuild::Token{"mymodule", "", abuild::Token::Type::ImportModule, false},
            abuild::Token{"yetanothermodule", "", abuild::Token::Type::ImportModule, false}});
    });
});
