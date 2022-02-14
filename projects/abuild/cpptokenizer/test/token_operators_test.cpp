import atest;
import abuild.cpptokenizer;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("token operators", [] { // NOLINT(cert-err58-cpp)
    test("AndToken", [] {
        std::stringstream stream;
        stream << ::abuild::AndToken{};

        expect(stream.str()).to_be("&&");
    });

    test("DefinedToken", [] {
        std::stringstream stream;
        stream << ::abuild::DefinedToken{.name = "MY_MACRO"};

        expect(stream.str()).to_be("defined(MY_MACRO)");
    });

    test("EqualsToken", [] {
        std::stringstream stream;
        stream << ::abuild::EqualsToken{.left = "MY_MACRO", .right = "10"};

        expect(stream.str()).to_be("MY_MACRO == 10");
    });

    test("GreaterThanOrEqualsToken", [] {
        std::stringstream stream;
        stream << ::abuild::GreaterThanOrEqualsToken{.left = "MY_MACRO", .right = "10"};

        expect(stream.str()).to_be("MY_MACRO >= 10");
    });

    test("GreaterThanToken", [] {
        std::stringstream stream;
        stream << ::abuild::GreaterThanToken{.left = "MY_MACRO", .right = "10"};

        expect(stream.str()).to_be("MY_MACRO > 10");
    });

    test("LeftBracketToken", [] {
        std::stringstream stream;
        stream << ::abuild::LeftBracketToken{};

        expect(stream.str()).to_be("(");
    });

    test("LessThanOrEqualsToken", [] {
        std::stringstream stream;
        stream << ::abuild::LessThanOrEqualsToken{.left = "MY_MACRO", .right = "10"};

        expect(stream.str()).to_be("MY_MACRO <= 10");
    });

    test("LessThanToken", [] {
        std::stringstream stream;
        stream << ::abuild::LessThanToken{.left = "MY_MACRO", .right = "10"};

        expect(stream.str()).to_be("MY_MACRO < 10");
    });

    test("HasIncludeLocalToken", [] {
        std::stringstream stream;
        stream << ::abuild::HasIncludeLocalToken{.name = "include/my_header.hpp"};

        expect(stream.str()).to_be("__has_include(\"include/my_header.hpp\")");
    });

    test("HasIncludeExternalToken", [] {
        std::stringstream stream;
        stream << ::abuild::HasIncludeExternalToken{.name = "include/my_header.hpp"};

        expect(stream.str()).to_be("__has_include(<include/my_header.hpp>)");
    });

    test("NotToken", [] {
        std::stringstream stream;
        stream << ::abuild::NotToken{};

        expect(stream.str()).to_be("!");
    });

    test("OrToken", [] {
        std::stringstream stream;
        stream << ::abuild::OrToken{};

        expect(stream.str()).to_be("||");
    });

    test("RightBracketToken", [] {
        std::stringstream stream;
        stream << ::abuild::RightBracketToken{};

        expect(stream.str()).to_be(")");
    });

    test("IfToken", [] {
        std::stringstream stream;
        stream << ::abuild::IfToken{.elements{
            ::abuild::LeftBracketToken{},
            ::abuild::DefinedToken{.name = "MY_MACRO"},
            ::abuild::AndToken{},
            ::abuild::NotToken{},
            ::abuild::DefinedToken{.name = "NO_MY_MACRO"},
            ::abuild::RightBracketToken{}}};

        expect(stream.str()).to_be("#if ( defined(MY_MACRO) && ! defined(NO_MY_MACRO) ) ");
    });

    test("ElseToken", [] {
        std::stringstream stream;
        stream << ::abuild::ElseToken{};

        expect(stream.str()).to_be("#else");
    });

    test("EndIfToken", [] {
        std::stringstream stream;
        stream << ::abuild::EndIfToken{};

        expect(stream.str()).to_be("#endif");
    });

    test("DefineToken", [] {
        std::stringstream stream;
        stream << ::abuild::DefineToken{.name = "MY_MACRO"};

        expect(stream.str()).to_be("#define MY_MACRO");
    });

    test("DefineToken - with value", [] {
        std::stringstream stream;
        stream << ::abuild::DefineToken{.name = "MY_MACRO", .value = "10"};

        expect(stream.str()).to_be("#define MY_MACRO 10");
    });

    test("ImportIncludeExternalToken", [] {
        std::stringstream stream;
        stream << ::abuild::ImportIncludeExternalToken{.name = "include/my_header.hpp"};

        expect(stream.str()).to_be("import <include/my_header.hpp>;");
    });

    test("ImportIncludeExternalToken - exported", [] {
        std::stringstream stream;
        stream << ::abuild::ImportIncludeExternalToken{.name = "include/my_header.hpp", .exported = true};

        expect(stream.str()).to_be("export import <include/my_header.hpp>;");
    });

    test("ImportIncludeLocalToken", [] {
        std::stringstream stream;
        stream << ::abuild::ImportIncludeLocalToken{.name = "include/my_header.hpp"};

        expect(stream.str()).to_be("import \"include/my_header.hpp\";");
    });

    test("ImportIncludeLocalToken - exported", [] {
        std::stringstream stream;
        stream << ::abuild::ImportIncludeLocalToken{.name = "include/my_header.hpp", .exported = true};

        expect(stream.str()).to_be("export import \"include/my_header.hpp\";");
    });

    test("ImportModulePartitionToken", [] {
        std::stringstream stream;
        stream << ::abuild::ImportModulePartitionToken{.name = "my_partition"};

        expect(stream.str()).to_be("import : my_partition;");
    });

    test("ImportModulePartitionToken - exported", [] {
        std::stringstream stream;
        stream << ::abuild::ImportModulePartitionToken{.name = "my_partition", .exported = true};

        expect(stream.str()).to_be("export import : my_partition;");
    });

    test("ImportModuleToken", [] {
        std::stringstream stream;
        stream << ::abuild::ImportModuleToken{.name = "my_module"};

        expect(stream.str()).to_be("import my_module;");
    });

    test("ImportModuleToken - exported", [] {
        std::stringstream stream;
        stream << ::abuild::ImportModuleToken{.name = "my_module", .exported = true};

        expect(stream.str()).to_be("export import my_module;");
    });

    test("IncludeExternalToken", [] {
        std::stringstream stream;
        stream << ::abuild::IncludeExternalToken{.name = "vector"};

        expect(stream.str()).to_be("#include <vector>");
    });

    test("IncludeLocalToken", [] {
        std::stringstream stream;
        stream << ::abuild::IncludeLocalToken{.name = "my_header.hpp"};

        expect(stream.str()).to_be("#include \"my_header.hpp\"");
    });

    test("ModulePartitionToken", [] {
        std::stringstream stream;
        stream << ::abuild::ModulePartitionToken{.mod = "my_module", .name = "my_partition"};

        expect(stream.str()).to_be("module my_module : my_partition;");
    });

    test("ModulePartitionToken - exported", [] {
        std::stringstream stream;
        stream << ::abuild::ModulePartitionToken{.mod = "my_module", .name = "my_partition", .exported = true};

        expect(stream.str()).to_be("export module my_module : my_partition;");
    });

    test("ModuleToken", [] {
        std::stringstream stream;
        stream << ::abuild::ModuleToken{.name = "my_module"};

        expect(stream.str()).to_be("module my_module;");
    });

    test("ModuleToken - exported", [] {
        std::stringstream stream;
        stream << ::abuild::ModuleToken{.name = "my_module", .exported = true};

        expect(stream.str()).to_be("export module my_module;");
    });

    test("UndefToken", [] {
        std::stringstream stream;
        stream << ::abuild::UndefToken{.name = "MY_MACRO"};

        expect(stream.str()).to_be("#undef MY_MACRO");
    });

    test("Token", [] {
        std::stringstream stream;
        stream << ::abuild::Token{::abuild::ModuleToken{.name = "my_module", .exported = true}};

        expect(stream.str()).to_be("export module my_module;");
    });
});
