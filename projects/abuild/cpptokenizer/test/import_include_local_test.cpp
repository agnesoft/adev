import atest;
import abuild.cpptokenizer;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("import include local", [] { // NOLINT(cert-err58-cpp)
    test("normal import", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("import \"header.hpp\";");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportIncludeLocalToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportIncludeLocalToken>(tokens[0]);

        expect(token.name).to_be("header.hpp");
        expect(token.exported).to_be(false);
    });

    test("no space", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("import\"header.hpp\";");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportIncludeLocalToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportIncludeLocalToken>(tokens[0]);

        expect(token.name).to_be("header.hpp");
        expect(token.exported).to_be(false);
    });

    test("extra spaces", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("    import    \"header.hpp\"   ;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportIncludeLocalToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportIncludeLocalToken>(tokens[0]);

        expect(token.name).to_be("header.hpp");
        expect(token.exported).to_be(false);
    });

    test("with comments", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("/*\n\n*/import/*\n\n*/\"header.hpp\";");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportIncludeLocalToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportIncludeLocalToken>(tokens[0]);

        expect(token.name).to_be("header.hpp");
        expect(token.exported).to_be(false);
    });

    test("space in the middle", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("import \"hea der.hpp\";");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportIncludeLocalToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportIncludeLocalToken>(tokens[0]);

        expect(token.name).to_be("hea der.hpp");
        expect(token.exported).to_be(false);
    });

    test("extra leading & trailing space", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("import \"  header.hpp  \";");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportIncludeLocalToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportIncludeLocalToken>(tokens[0]);

        expect(token.name).to_be("header.hpp");
        expect(token.exported).to_be(false);
    });

    test("trailing comment", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("import \"header.hpp\" /* some comment */ ;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportIncludeLocalToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportIncludeLocalToken>(tokens[0]);

        expect(token.name).to_be("header.hpp");
        expect(token.exported).to_be(false);
    });

    test("with newlines", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("\nimport \n\"my_header.hpp\"\n;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportIncludeLocalToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportIncludeLocalToken>(tokens[0]);

        expect(token.name).to_be("my_header.hpp");
        expect(token.exported).to_be(false);
    });

    test("missing opening quote with another import", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("import \"header.hpp;\nimport \"otherheader.hpp\";");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportIncludeLocalToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportIncludeLocalToken>(tokens[0]);

        expect(token.name).to_be("otherheader.hpp");
        expect(token.exported).to_be(false);
    });

    test("export regular import", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("export import \"header.hpp\";");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportIncludeLocalToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportIncludeLocalToken>(tokens[0]);

        expect(token.name).to_be("header.hpp");
        expect(token.exported).to_be(true);
    });

    test("export no space", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("export import\"header.hpp\";");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportIncludeLocalToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportIncludeLocalToken>(tokens[0]);

        expect(token.name).to_be("header.hpp");
        expect(token.exported).to_be(true);
    });

    test("export with extra spaces", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("  export    import   \"header.hpp\"   ;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportIncludeLocalToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportIncludeLocalToken>(tokens[0]);

        expect(token.name).to_be("header.hpp");
        expect(token.exported).to_be(true);
    });

    test("export with extra leading & trailing space", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("export import \"  header.hpp  \";");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportIncludeLocalToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportIncludeLocalToken>(tokens[0]);

        expect(token.name).to_be("header.hpp");
        expect(token.exported).to_be(true);
    });

    test("export with newlines", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("export\nimport\n\n\"header.hpp\"\n\n;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportIncludeLocalToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportIncludeLocalToken>(tokens[0]);

        expect(token.name).to_be("header.hpp");
        expect(token.exported).to_be(true);
    });

    test("export with comments", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("/*\n\n*/export/*\n\n*/import/*\n\n*/\"header.hpp\"/*\n\n*/;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportIncludeLocalToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportIncludeLocalToken>(tokens[0]);

        expect(token.name).to_be("header.hpp");
        expect(token.exported).to_be(true);
    });

    test("missing closing quote", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("import \"aaa;");

        assert_(tokens.size()).to_be(0U);
    });

    test("missing opening quote", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("import aaa\";");

        assert_(tokens.size()).to_be(0U);
    });
});
