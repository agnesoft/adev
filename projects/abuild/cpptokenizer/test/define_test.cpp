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

    test("#define MY_MACRO", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#define MY_MACRO");

        assert_(tokens.size()).to_be(1);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[0])).to_be(true);

        const ::abuild::DefineToken token = std::get<::abuild::DefineToken>(tokens[0]);

        expect(token.name).to_be("MY_MACRO");
        expect(token.value).to_be("");
    });

    test("# \\\t\tdefine \\ MY_MACRO \\ 1", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("# \\\n\t\tdefine \\\n MY_MACRO \\\n 1");

        assert_(tokens.size()).to_be(1);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[0])).to_be(true);

        const ::abuild::DefineToken token = std::get<::abuild::DefineToken>(tokens[0]);

        expect(token.name).to_be("MY_MACRO");
        expect(token.value).to_be("1");
    });

    test("#define MY_MACRO \"111\"", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#define MY_MACRO \"111\"");

        assert_(tokens.size()).to_be(1);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[0])).to_be(true);

        const ::abuild::DefineToken token = std::get<::abuild::DefineToken>(tokens[0]);

        expect(token.name).to_be("MY_MACRO");
        expect(token.value).to_be("\"111\"");
    });

    test("#define MY_MACRO 1 \\ 1", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#define MY_MACRO 1 \\\n 1");

        expect(tokens.size()).to_be(0);
    });
});
