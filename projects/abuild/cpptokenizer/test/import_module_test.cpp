import atest;
import abuild.cpptokenizer;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("import module", [] { // NOLINT(cert-err58-cpp)
    test("normal import", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("import mymodule;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportModuleToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportModuleToken>(tokens[0]);

        expect(token.name).to_be("mymodule");
        expect(token.exported).to_be(false);
    });

    test("with spaces", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("   import    mymodule   ;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportModuleToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportModuleToken>(tokens[0]);

        expect(token.name).to_be("mymodule");
        expect(token.exported).to_be(false);
    });

    test("with newlines", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("import\n  mymodule  \n;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportModuleToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportModuleToken>(tokens[0]);

        expect(token.name).to_be("mymodule");
        expect(token.exported).to_be(false);
    });

    test("with comments", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("/*\n\n*/import/*\n\n*/mymodule/*\n*/;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportModuleToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportModuleToken>(tokens[0]);

        expect(token.name).to_be("mymodule");
        expect(token.exported).to_be(false);
    });

    test("export normal import", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("export import mymodule;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportModuleToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportModuleToken>(tokens[0]);

        expect(token.name).to_be("mymodule");
        expect(token.exported).to_be(true);
    });

    test("export with spaces", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("   export   import    mymodule   ;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportModuleToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportModuleToken>(tokens[0]);

        expect(token.name).to_be("mymodule");
        expect(token.exported).to_be(true);
    });

    test("export with newlines", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("export\nimport\nmymodule\n;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportModuleToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportModuleToken>(tokens[0]);

        expect(token.name).to_be("mymodule");
        expect(token.exported).to_be(true);
    });

    test("export with comments", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("/*\n*/export/*\n\n*/import/*\n\n*/mymodule/*\n*/;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportModuleToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportModuleToken>(tokens[0]);

        expect(token.name).to_be("mymodule");
        expect(token.exported).to_be(true);
    });

    test("empty module name", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("import ;");

        assert_(tokens.size()).to_be(0U);
    });

    test("missing semicolon", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("import mymodule");

        assert_(tokens.size()).to_be(0U);
    });
});
