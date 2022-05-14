import atest;
import abuild.cpptokenizer;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("module", [] { // NOLINT(cert-err58-cpp)
    test("regular", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("module mymodule;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ModuleToken>(tokens[0])).to_be(true);

        expect(std::get<::abuild::ModuleToken>(tokens[0]).name).to_be("mymodule");
        expect(std::get<::abuild::ModuleToken>(tokens[0]).exported).to_be(false);
    });

    test("with spaces", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("   module     mymodule   ;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ModuleToken>(tokens[0])).to_be(true);

        expect(std::get<::abuild::ModuleToken>(tokens[0]).name).to_be("mymodule");
        expect(std::get<::abuild::ModuleToken>(tokens[0]).exported).to_be(false);
    });

    test("with end of lines", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("module\nmymodule\n;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ModuleToken>(tokens[0])).to_be(true);

        expect(std::get<::abuild::ModuleToken>(tokens[0]).name).to_be("mymodule");
        expect(std::get<::abuild::ModuleToken>(tokens[0]).exported).to_be(false);
    });

    test("with comments", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("/*\n\n*/module/*\n\n*/mymodule/*\n\n*/;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ModuleToken>(tokens[0])).to_be(true);

        expect(std::get<::abuild::ModuleToken>(tokens[0]).name).to_be("mymodule");
        expect(std::get<::abuild::ModuleToken>(tokens[0]).exported).to_be(false);
    });

    test("export regular", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("export module mymodule;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ModuleToken>(tokens[0])).to_be(true);

        expect(std::get<::abuild::ModuleToken>(tokens[0]).name).to_be("mymodule");
        expect(std::get<::abuild::ModuleToken>(tokens[0]).exported).to_be(true);
    });

    test("export with spaces", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("   export   module    mymodule   ;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ModuleToken>(tokens[0])).to_be(true);

        expect(std::get<::abuild::ModuleToken>(tokens[0]).name).to_be("mymodule");
        expect(std::get<::abuild::ModuleToken>(tokens[0]).exported).to_be(true);
    });

    test("export with end of lines", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("export\nmodule\nmymodule\n;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ModuleToken>(tokens[0])).to_be(true);

        expect(std::get<::abuild::ModuleToken>(tokens[0]).name).to_be("mymodule");
        expect(std::get<::abuild::ModuleToken>(tokens[0]).exported).to_be(true);
    });

    test("export with comments", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("/*\n*/export/*\n\n*/module/*\n\n*/mymodule/*\n*/;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ModuleToken>(tokens[0])).to_be(true);

        expect(std::get<::abuild::ModuleToken>(tokens[0]).name).to_be("mymodule");
        expect(std::get<::abuild::ModuleToken>(tokens[0]).exported).to_be(true);
    });

    test("name with dot", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("module my.module;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ModuleToken>(tokens[0])).to_be(true);

        expect(std::get<::abuild::ModuleToken>(tokens[0]).name).to_be("my.module");
        expect(std::get<::abuild::ModuleToken>(tokens[0]).exported).to_be(false);
    });

    test("missing semicolon with end of line", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("module mymodule\n\nmodule othermodule;");

        assert_(tokens.size()).to_be(0U);
    });

    test("global module fragment", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("module;");

        assert_(tokens.size()).to_be(0U);
    });

    test("empty module name", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("module ;");

        assert_(tokens.size()).to_be(0U);
    });

    test("missing semicolon", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("module mymodule");

        assert_(tokens.size()).to_be(0U);
    });

    test("incorrect space", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("module my module;");

        assert_(tokens.size()).to_be(0U);
    });

    test("non export token", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("extern \"C\" { }");

        assert_(tokens.size()).to_be(0U);
    });

    test("export function", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("export my_class;");

        assert_(tokens.size()).to_be(0U);
    });

    test("export function", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("export void foo();");

        assert_(tokens.size()).to_be(0U);
    });

    test("non module token", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("class my_class;");

        assert_(tokens.size()).to_be(0U);
    });
});
