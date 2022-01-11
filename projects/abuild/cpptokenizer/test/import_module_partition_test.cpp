import atest;
import abuild.cpptokenizer;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("import module partition", [] { // NOLINT(cert-err58-cpp)
    test("normal import", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("import : mypartition;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportModulePartitionToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportModulePartitionToken>(tokens[0]);

        expect(token.name).to_be("mypartition");
        expect(token.exported).to_be(false);
    });

    test("with spaces", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("   import  :   mypartition   ;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportModulePartitionToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportModulePartitionToken>(tokens[0]);

        expect(token.name).to_be("mypartition");
        expect(token.exported).to_be(false);
    });

    test("no spaces", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("   import:mypartition;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportModulePartitionToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportModulePartitionToken>(tokens[0]);

        expect(token.name).to_be("mypartition");
        expect(token.exported).to_be(false);
    });

    test("with newlines", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("import\n  :\n  mypartition  \n;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportModulePartitionToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportModulePartitionToken>(tokens[0]);

        expect(token.name).to_be("mypartition");
        expect(token.exported).to_be(false);
    });

    test("with comments", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("/*\n\n*/import/*\n\n*/:/*\n\n*/mypartition/*\n*/;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportModulePartitionToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportModulePartitionToken>(tokens[0]);

        expect(token.name).to_be("mypartition");
        expect(token.exported).to_be(false);
    });

    test("export normal import", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("export import : mypartition;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportModulePartitionToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportModulePartitionToken>(tokens[0]);

        expect(token.name).to_be("mypartition");
        expect(token.exported).to_be(true);
    });

    test("export with spaces", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("   export   import    :    mypartition   ;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportModulePartitionToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportModulePartitionToken>(tokens[0]);

        expect(token.name).to_be("mypartition");
        expect(token.exported).to_be(true);
    });

    test("export with newlines", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("export\nimport\n:\nmypartition\n;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportModulePartitionToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportModulePartitionToken>(tokens[0]);

        expect(token.name).to_be("mypartition");
        expect(token.exported).to_be(true);
    });

    test("export with comments", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("/*\n*/export/*\n\n*/import/*\n\n*/:/*\n\n*/nmypartition/*\n*/;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ImportModulePartitionToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ImportModulePartitionToken>(tokens[0]);

        expect(token.name).to_be("nmypartition");
        expect(token.exported).to_be(true);
    });

    test("missing semicolon", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("import : mypartition");

        assert_(tokens.size()).to_be(0U);
    });
});
