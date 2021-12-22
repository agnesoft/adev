import atest;
import abuild.cpptokenizer;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("undef", [] { // NOLINT(cert-err58-cpp)
    test("single", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#undef MY_MACRO");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::UndefToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::UndefToken>(tokens[0]);

        expect(token.name).to_be("MY_MACRO");
    });

    test("multiple", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#   undef MY_MACRO\t\n#undef /* SOME_OTHER_MACRO */ MY_OTHER_MACRO");

        assert_(tokens.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::UndefToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::UndefToken>(tokens[1])).to_be(true);

        const auto &token = std::get<::abuild::UndefToken>(tokens[0]);

        expect(token.name).to_be("MY_MACRO");

        const auto &token2 = std::get<::abuild::UndefToken>(tokens[1]);

        expect(token2.name).to_be("MY_OTHER_MACRO");
    });
});
