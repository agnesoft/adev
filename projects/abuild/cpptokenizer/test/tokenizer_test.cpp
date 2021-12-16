import atest;
import abuild.cpptokenizer;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("define", [] { // NOLINT(cert-err58-cpp)
    test("fake comment", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("/ * /* / */");

        expect(tokens.size()).to_be(0u);
    });

    test("unknown preprocessor", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#error");

        expect(tokens.size()).to_be(0u);
    });

    test("bad preprocessor", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("# ");

        expect(tokens.size()).to_be(0u);
    });
});
