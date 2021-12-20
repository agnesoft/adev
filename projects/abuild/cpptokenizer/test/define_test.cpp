import atest;
import abuild.cpptokenizer;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("define", [] { // NOLINT(cert-err58-cpp)
    test("with value", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#define MY_MACRO 1");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[0])).to_be(true);

        const ::abuild::DefineToken token = std::get<::abuild::DefineToken>(tokens[0]);

        expect(token.name).to_be("MY_MACRO");
        expect(token.value).to_be("1");
    });

    test("no value", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#define MY_MACRO");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[0])).to_be(true);

        const ::abuild::DefineToken token = std::get<::abuild::DefineToken>(tokens[0]);

        expect(token.name).to_be("MY_MACRO");
        expect(token.value).to_be("");
    });

    test("multiline macro", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("# \\\n\t\tdefine \\\n MY_MACRO \\\n 1");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[0])).to_be(true);

        const ::abuild::DefineToken token = std::get<::abuild::DefineToken>(tokens[0]);

        expect(token.name).to_be("MY_MACRO");
        expect(token.value).to_be("1");
    });

    test("quoted", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#define MY_MACRO \"some string\"");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[0])).to_be(true);

        const ::abuild::DefineToken token = std::get<::abuild::DefineToken>(tokens[0]);

        expect(token.name).to_be("MY_MACRO");
        expect(token.value).to_be("\"some string\"");
    });

    test("with comments", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#define /*some\nlong\nmultiline\n\n comment*/   MY_MACRO \"111\" // other comment\n");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[0])).to_be(true);

        const ::abuild::DefineToken token = std::get<::abuild::DefineToken>(tokens[0]);

        expect(token.name).to_be("MY_MACRO");
        expect(token.value).to_be("\"111\"");
    });

    test("with line comments", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#define MY_MACRO //some comment");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[0])).to_be(true);

        const ::abuild::DefineToken token = std::get<::abuild::DefineToken>(tokens[0]);

        expect(token.name).to_be("MY_MACRO");
        expect(token.value).to_be("");
    });

    test("with string", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#define MY_MACRO\n#define MY_STRING std::string str = \"some string\";\n#define MY_OTHER_MACRO 123\n");

        assert_(tokens.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[1])).to_be(true);

        const ::abuild::DefineToken token = std::get<::abuild::DefineToken>(tokens[0]);

        expect(token.name).to_be("MY_MACRO");
        expect(token.value).to_be("");

        const ::abuild::DefineToken token2 = std::get<::abuild::DefineToken>(tokens[1]);

        expect(token2.name).to_be("MY_OTHER_MACRO");
        expect(token2.value).to_be("123");
    });

    test("preceded by line comment", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("//some comment\n#define MY_MACRO");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[0])).to_be(true);

        const ::abuild::DefineToken token = std::get<::abuild::DefineToken>(tokens[0]);

        expect(token.name).to_be("MY_MACRO");
        expect(token.value).to_be("");
    });

    test("succeeded by multiline comment", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#define MY_MACRO 123/* What about \"this\" comment? \n */\n#define MY_OTHER_MACRO 5");

        assert_(tokens.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[0])).to_be(true);

        const ::abuild::DefineToken token = std::get<::abuild::DefineToken>(tokens[0]);

        expect(token.name).to_be("MY_MACRO");
        expect(token.value).to_be("123");

        const ::abuild::DefineToken token2 = std::get<::abuild::DefineToken>(tokens[1]);

        expect(token2.name).to_be("MY_OTHER_MACRO");
        expect(token2.value).to_be("5");
    });

    test("commented", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("//#define MY_MACRO");

        expect(tokens.size()).to_be(0U);
    });

    test("multiline value", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#define MY_MACRO 1 \\\n 1");

        expect(tokens.size()).to_be(0U);
    });

    test("multiline value with multiline comment", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#define MY_MACRO 1 \\\n 1 /* pretty\ncrazy\ncode\narrangement */");

        expect(tokens.size()).to_be(0U);
    });

    test("value with space", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#define MY_MACRO some string");

        expect(tokens.size()).to_be(0U);
    });

    test("no define", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("int main()\n{\nreturn 1;\n}\n");

        expect(tokens.size()).to_be(0U);
    });
});
