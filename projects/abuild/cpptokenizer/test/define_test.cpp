import atest;
import abuild.cpptokenizer;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("define", [] { // NOLINT(cert-err58-cpp)
    test("with value", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#define MY_MACRO 1");

        assert_(tokens.size()).to_be(1u);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[0])).to_be(true);

        const ::abuild::DefineToken token = std::get<::abuild::DefineToken>(tokens[0]);

        expect(token.name).to_be("MY_MACRO");
        expect(token.value).to_be("1");
    });

    test("no value", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#define MY_MACRO");

        assert_(tokens.size()).to_be(1u);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[0])).to_be(true);

        const ::abuild::DefineToken token = std::get<::abuild::DefineToken>(tokens[0]);

        expect(token.name).to_be("MY_MACRO");
        expect(token.value).to_be("");
    });

    test("multiline macro", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("# \\\n\t\tdefine \\\n MY_MACRO \\\n 1");

        assert_(tokens.size()).to_be(1u);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[0])).to_be(true);

        const ::abuild::DefineToken token = std::get<::abuild::DefineToken>(tokens[0]);

        expect(token.name).to_be("MY_MACRO");
        expect(token.value).to_be("1");
    });

    test("quoted", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#define MY_MACRO \"some string\"");

        assert_(tokens.size()).to_be(1u);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[0])).to_be(true);

        const ::abuild::DefineToken token = std::get<::abuild::DefineToken>(tokens[0]);

        expect(token.name).to_be("MY_MACRO");
        expect(token.value).to_be("\"some string\"");
    });

    test("with comments", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#define /*some\nlong\nmultiline\n\n comment*/   MY_MACRO \"111\" // other comment\n");

        assert_(tokens.size()).to_be(1u);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[0])).to_be(true);

        const ::abuild::DefineToken token = std::get<::abuild::DefineToken>(tokens[0]);

        expect(token.name).to_be("MY_MACRO");
        expect(token.value).to_be("\"111\"");
    });

    test("multiline value", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#define MY_MACRO 1 \\ 1");

        expect(tokens.size()).to_be(0u);
    });

    test("value with space", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#define MY_MACRO some string");

        assert_(tokens.size()).to_be(1u);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[0])).to_be(true);

        const ::abuild::DefineToken token = std::get<::abuild::DefineToken>(tokens[0]);

        expect(token.name).to_be("MY_MACRO");
        expect(token.value).to_be("some string");
    });

    test("no define", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("int main()\n{\nreturn 1;\n}\n");

        expect(tokens.size()).to_be(0u);
    });
});
