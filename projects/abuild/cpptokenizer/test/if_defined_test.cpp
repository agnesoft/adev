import atest;
import abuild.cpptokenizer;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("if defined", [] { // NOLINT(cert-err58-cpp)
    test("if defined", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if defined(MY_MACRO)\n#endif");

        assert_(tokens.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[1])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::DefinedToken>(condition.elements[0])).to_be(true);

        const auto &token = std::get<::abuild::DefinedToken>(condition.elements[0]);

        expect(token.name).to_be("MY_MACRO");
    });

    test("if \\ defined \\ ( \\  \\ )", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if \\\n defined \\\n ( \\\n MY_MACRO \\\n )\n#endif");

        assert_(tokens.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[1])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::DefinedToken>(condition.elements[0])).to_be(true);

        const auto &token = std::get<::abuild::DefinedToken>(condition.elements[0]);

        expect(token.name).to_be("MY_MACRO");
    });

    test("if !defined", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if !defined(MY_MACRO)\n#endif");

        assert_(tokens.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[1])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::NotDefinedToken>(condition.elements[0])).to_be(true);

        const auto &token = std::get<::abuild::NotDefinedToken>(condition.elements[0]);

        expect(token.name).to_be("MY_MACRO");
    });

    test("if \\ ! \\ defined \\ ( \\  \\ )", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if \\\n ! \\\n defined \\\n ( \\\n MY_MACRO \\\n )\n#endif");

        assert_(tokens.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[1])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::NotDefinedToken>(condition.elements[0])).to_be(true);

        const auto &token = std::get<::abuild::NotDefinedToken>(condition.elements[0]);

        expect(token.name).to_be("MY_MACRO");
    });
});
