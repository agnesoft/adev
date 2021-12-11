import atest;
import abuild.cpptokenizer;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("define", [] { // NOLINT(cert-err58-cpp)
    test("#define MY_MACRO 1", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#define MY_MACRO 1");

        assert_(tokens.size()).to_be(1);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[0])).to_be(true);

        const ::abuild::DefineToken token = std::get<::abuild::DefineToken>(tokens[0]);

        expect(token.name).to_be("MY_MACRO");
        expect(token.value).to_be("1");
    });
});
