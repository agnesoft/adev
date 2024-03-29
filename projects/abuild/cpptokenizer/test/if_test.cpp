import atest;
import abuild.cpptokenizer;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("if", [] { // NOLINT(cert-err58-cpp)
    test("equals", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if MY_MACRO == 2\n#endif");

        assert_(tokens.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[1])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::EqualsToken>(condition.elements[0])).to_be(true);

        const auto &token = std::get<::abuild::EqualsToken>(condition.elements[0]);

        expect(token.left).to_be("MY_MACRO");
        expect(token.right).to_be("2");
    });

    test("not equals", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if MY_MACRO != 2\n#endif");

        assert_(tokens.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[1])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::NotToken>(condition.elements[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EqualsToken>(condition.elements[1])).to_be(true);

        const auto &token = std::get<::abuild::EqualsToken>(condition.elements[1]);

        expect(token.left).to_be("MY_MACRO");
        expect(token.right).to_be("2");
    });

    test("less than", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if MY_MACRO < 3\n#endif");

        assert_(tokens.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[1])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::LessThanToken>(condition.elements[0])).to_be(true);

        const auto &token = std::get<::abuild::LessThanToken>(condition.elements[0]);

        expect(token.left).to_be("MY_MACRO");
        expect(token.right).to_be("3");
    });

    test("less than or equals", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if MY_MACRO <= 3\n#endif");

        assert_(tokens.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[1])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::LessThanOrEqualsToken>(condition.elements[0])).to_be(true);

        const auto &token = std::get<::abuild::LessThanOrEqualsToken>(condition.elements[0]);

        expect(token.left).to_be("MY_MACRO");
        expect(token.right).to_be("3");
    });

    test("greater than", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if MY_MACRO > 3\n#endif");

        assert_(tokens.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[1])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::GreaterThanToken>(condition.elements[0])).to_be(true);

        const auto &token = std::get<::abuild::GreaterThanToken>(condition.elements[0]);

        expect(token.left).to_be("MY_MACRO");
        expect(token.right).to_be("3");
    });

    test("greater than or equals", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if MY_MACRO >= 3\n#endif");

        assert_(tokens.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[1])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::GreaterThanOrEqualsToken>(condition.elements[0])).to_be(true);

        const auto &token = std::get<::abuild::GreaterThanOrEqualsToken>(condition.elements[0]);

        expect(token.left).to_be("MY_MACRO");
        expect(token.right).to_be("3");
    });

    test("string on left hand side", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if \"some string\" == MY_MACRO\n#endif");

        assert_(tokens.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[1])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::EqualsToken>(condition.elements[0])).to_be(true);

        const auto &token = std::get<::abuild::EqualsToken>(condition.elements[0]);

        expect(token.left).to_be("\"some string\"");
        expect(token.right).to_be("MY_MACRO");
    });

    test("multiple conditions and", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if MY_MACRO < 3 && MY_OTHER_MACRO > 5\n#endif");

        assert_(tokens.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[1])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(3U);
        assert_(std::holds_alternative<::abuild::LessThanToken>(condition.elements[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::AndToken>(condition.elements[1])).to_be(true);
        assert_(std::holds_alternative<::abuild::GreaterThanToken>(condition.elements[2])).to_be(true);

        const auto &token = std::get<::abuild::LessThanToken>(condition.elements[0]);

        expect(token.left).to_be("MY_MACRO");
        expect(token.right).to_be("3");

        const auto &token2 = std::get<::abuild::GreaterThanToken>(condition.elements[2]);

        expect(token2.left).to_be("MY_OTHER_MACRO");
        expect(token2.right).to_be("5");
    });

    test("multiple conditions or", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if MY_MACRO < 3 || MY_OTHER_MACRO > 5\n#endif");

        assert_(tokens.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[1])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(3U);
        assert_(std::holds_alternative<::abuild::LessThanToken>(condition.elements[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::OrToken>(condition.elements[1])).to_be(true);
        assert_(std::holds_alternative<::abuild::GreaterThanToken>(condition.elements[2])).to_be(true);

        const auto &token = std::get<::abuild::LessThanToken>(condition.elements[0]);

        expect(token.left).to_be("MY_MACRO");
        expect(token.right).to_be("3");

        const auto &token2 = std::get<::abuild::GreaterThanToken>(condition.elements[2]);

        expect(token2.left).to_be("MY_OTHER_MACRO");
        expect(token2.right).to_be("5");
    });

    test("multiple defined and", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if MY_MACRO && MY_OTHER_MACRO\n#endif");

        assert_(tokens.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[1])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(5U); // NOLINT(readability-magic-numbers)
        assert_(std::holds_alternative<::abuild::NotToken>(condition.elements[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EqualsToken>(condition.elements[1])).to_be(true);
        assert_(std::holds_alternative<::abuild::AndToken>(condition.elements[2])).to_be(true);
        assert_(std::holds_alternative<::abuild::NotToken>(condition.elements[3])).to_be(true);
        assert_(std::holds_alternative<::abuild::EqualsToken>(condition.elements[4])).to_be(true);

        const auto &token = std::get<::abuild::EqualsToken>(condition.elements[1]);

        expect(token.left).to_be("MY_MACRO");
        expect(token.right).to_be("0");

        const auto &token2 = std::get<::abuild::EqualsToken>(condition.elements[4]);

        expect(token2.left).to_be("MY_OTHER_MACRO");
        expect(token2.right).to_be("0");
    });

    test("single equal sign", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if MY_MACRO = 2\n#endif");

        assert_(tokens.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[1])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::NotToken>(condition.elements[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EqualsToken>(condition.elements[1])).to_be(true);

        const auto &token = std::get<::abuild::EqualsToken>(condition.elements[1]);

        expect(token.left).to_be("MY_MACRO");
        expect(token.right).to_be("0");
    });

    test("single and", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if MY_MACRO & 2\n#endif");

        assert_(tokens.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[1])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::NotToken>(condition.elements[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EqualsToken>(condition.elements[1])).to_be(true);

        const auto &token = std::get<::abuild::EqualsToken>(condition.elements[1]);

        expect(token.left).to_be("MY_MACRO");
        expect(token.right).to_be("0");
    });

    test("single or", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if MY_MACRO | 3\n#endif");

        assert_(tokens.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[1])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::NotToken>(condition.elements[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EqualsToken>(condition.elements[1])).to_be(true);

        const auto &token = std::get<::abuild::EqualsToken>(condition.elements[1]);

        expect(token.left).to_be("MY_MACRO");
        expect(token.right).to_be("0");
    });

    test("not sign", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if MY_MACRO ! 3\n#endif");

        assert_(tokens.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[1])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(5U); // NOLINT(readability-magic-numbers)
        assert_(std::holds_alternative<::abuild::NotToken>(condition.elements[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EqualsToken>(condition.elements[1])).to_be(true);
        assert_(std::holds_alternative<::abuild::NotToken>(condition.elements[2])).to_be(true);
        assert_(std::holds_alternative<::abuild::NotToken>(condition.elements[3])).to_be(true);
        assert_(std::holds_alternative<::abuild::EqualsToken>(condition.elements[4])).to_be(true);

        const auto &token = std::get<::abuild::EqualsToken>(condition.elements[1]);

        expect(token.left).to_be("MY_MACRO");
        expect(token.right).to_be("0");

        const auto &token2 = std::get<::abuild::EqualsToken>(condition.elements[4]);

        expect(token2.left).to_be("3");
        expect(token2.right).to_be("0");
    });

    test("single or with comment", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if MY_MACRO | 3 //end of bad macro");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::NotToken>(condition.elements[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EqualsToken>(condition.elements[1])).to_be(true);

        const auto &token = std::get<::abuild::EqualsToken>(condition.elements[1]);

        expect(token.left).to_be("MY_MACRO");
        expect(token.right).to_be("0");
    });
});
