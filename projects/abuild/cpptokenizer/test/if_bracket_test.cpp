import atest;
import abuild.cpptokenizer;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("if ()", [] { // NOLINT(cert-err58-cpp)
    test("if MY_MACRO == 3 && (MY_OTHER_MACRO == 5 || defined(YET_ANOTHER))", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if MY_MACRO == 3 && (MY_OTHER_MACRO < 5 || defined(YET_ANOTHER))\n#endif");

        assert_(tokens.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[1])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(7U);
        assert_(std::holds_alternative<::abuild::EqualsToken>(condition.elements[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::AndToken>(condition.elements[1])).to_be(true);
        assert_(std::holds_alternative<::abuild::LeftBracketToken>(condition.elements[2])).to_be(true);
        assert_(std::holds_alternative<::abuild::LessThanToken>(condition.elements[3])).to_be(true);
        assert_(std::holds_alternative<::abuild::OrToken>(condition.elements[4])).to_be(true);
        assert_(std::holds_alternative<::abuild::DefinedToken>(condition.elements[5])).to_be(true);
        assert_(std::holds_alternative<::abuild::RightBracketToken>(condition.elements[6])).to_be(true);

        const auto &token = std::get<::abuild::EqualsToken>(condition.elements[0]);

        expect(token.left).to_be("MY_MACRO");
        expect(token.right).to_be("3");

        const auto &token3 = std::get<::abuild::LessThanToken>(condition.elements[3]);

        expect(token3.left).to_be("MY_OTHER_MACRO");
        expect(token3.right).to_be("5");

        const auto &token5 = std::get<::abuild::DefinedToken>(condition.elements[5]);

        expect(token5.name).to_be("YET_ANOTHER");
    });
});
