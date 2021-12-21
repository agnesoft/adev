import atest;
import abuild.cpptokenizer;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("define", [] { // NOLINT(cert-err58-cpp)
    test("fake comment", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("/ * /* / */");

        expect(tokens.size()).to_be(0U);
    });

    test("unknown preprocessor", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#error");

        expect(tokens.size()).to_be(0U);
    });

    test("bad preprocessor", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("# ");

        expect(tokens.size()).to_be(0U);
    });

    test("bad multiline comment", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("/* unended multiline comment...");

        expect(tokens.size()).to_be(0U);
    });

    test("string at start", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("\"some string\"");

        expect(tokens.size()).to_be(0U);
    });

    test("unended string", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("\"some string");

        expect(tokens.size()).to_be(0U);
    });

    test("bad raw string", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("Rasd(\"\"");

        expect(tokens.size()).to_be(0U);
    });

    test("unended raw string sequence", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("R\"asd");

        expect(tokens.size()).to_be(0U);
    });
});
