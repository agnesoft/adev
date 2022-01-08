import atest;
import abuild.cpptokenizer;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("if else", [] { // NOLINT(cert-err58-cpp)
    test("else", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#ifndef MY_MACRO\n#define MY_MACRO\n#else\n#define MY_OTHER_MACRO\n#endif");

        assert_(tokens.size()).to_be(5U); // NOLINT(readability-magic-numbers)
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[1])).to_be(true);
        assert_(std::holds_alternative<::abuild::ElseToken>(tokens[2])).to_be(true);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[3])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[4])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::NotToken>(condition.elements[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::DefinedToken>(condition.elements[1])).to_be(true);

        const auto &definedToken = std::get<::abuild::DefinedToken>(condition.elements[1]);

        expect(definedToken.name).to_be("MY_MACRO");

        const auto &define1 = std::get<::abuild::DefineToken>(tokens[1]);

        expect(define1.name).to_be("MY_MACRO");
        expect(define1.value).to_be("");

        const auto &define2 = std::get<::abuild::DefineToken>(tokens[3]);

        expect(define2.name).to_be("MY_OTHER_MACRO");
        expect(define2.value).to_be("");
    });

    test("elif", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#ifndef MY_MACRO\n#define MY_MACRO\n#elif MY_MACRO == 2\n#define MY_OTHER_MACRO\n#endif");

        assert_(tokens.size()).to_be(6U); // NOLINT(readability-magic-numbers)
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[1])).to_be(true);
        assert_(std::holds_alternative<::abuild::ElseToken>(tokens[2])).to_be(true);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[3])).to_be(true);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[4])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[5])).to_be(true); // NOLINT(readability-magic-numbers)

        const auto &condition1 = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition1.elements.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::NotToken>(condition1.elements[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::DefinedToken>(condition1.elements[1])).to_be(true);

        const auto &definedToken = std::get<::abuild::DefinedToken>(condition1.elements[1]);

        expect(definedToken.name).to_be("MY_MACRO");

        const auto &define1 = std::get<::abuild::DefineToken>(tokens[1]);

        expect(define1.name).to_be("MY_MACRO");
        expect(define1.value).to_be("");

        const auto &condition2 = std::get<::abuild::IfToken>(tokens[3]);
        assert_(condition2.elements.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::EqualsToken>(condition2.elements[0])).to_be(true);

        const auto &equalsToken = std::get<::abuild::EqualsToken>(condition2.elements[0]);

        expect(equalsToken.left).to_be("MY_MACRO");
        expect(equalsToken.right).to_be("2");

        const auto &define2 = std::get<::abuild::DefineToken>(tokens[4]);

        expect(define2.name).to_be("MY_OTHER_MACRO");
        expect(define2.value).to_be("");
    });

    test("elifdef", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#ifndef MY_MACRO\n#define MY_MACRO\n#elifdef MY_MACRO\n#define MY_OTHER_MACRO\n#endif");

        assert_(tokens.size()).to_be(6U); // NOLINT(readability-magic-numbers)
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[1])).to_be(true);
        assert_(std::holds_alternative<::abuild::ElseToken>(tokens[2])).to_be(true);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[3])).to_be(true);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[4])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[5])).to_be(true); // NOLINT(readability-magic-numbers)

        const auto &condition1 = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition1.elements.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::NotToken>(condition1.elements[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::DefinedToken>(condition1.elements[1])).to_be(true);

        const auto &definedToken = std::get<::abuild::DefinedToken>(condition1.elements[1]);

        expect(definedToken.name).to_be("MY_MACRO");

        const auto &define1 = std::get<::abuild::DefineToken>(tokens[1]);

        expect(define1.name).to_be("MY_MACRO");
        expect(define1.value).to_be("");

        const auto &condition2 = std::get<::abuild::IfToken>(tokens[3]);
        assert_(condition2.elements.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::DefinedToken>(condition2.elements[0])).to_be(true);

        const auto &equalsToken = std::get<::abuild::DefinedToken>(condition2.elements[0]);

        expect(equalsToken.name).to_be("MY_MACRO");

        const auto &define2 = std::get<::abuild::DefineToken>(tokens[4]);

        expect(define2.name).to_be("MY_OTHER_MACRO");
        expect(define2.value).to_be("");
    });

    test("elifndef", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#ifndef MY_MACRO\n#define MY_MACRO\n#elifndef MY_MACRO\n#define MY_OTHER_MACRO\n#endif");

        assert_(tokens.size()).to_be(6U); // NOLINT(readability-magic-numbers)
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[1])).to_be(true);
        assert_(std::holds_alternative<::abuild::ElseToken>(tokens[2])).to_be(true);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[3])).to_be(true);
        assert_(std::holds_alternative<::abuild::DefineToken>(tokens[4])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[5])).to_be(true); // NOLINT(readability-magic-numbers)

        const auto &condition1 = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition1.elements.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::NotToken>(condition1.elements[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::DefinedToken>(condition1.elements[1])).to_be(true);

        const auto &definedToken = std::get<::abuild::DefinedToken>(condition1.elements[1]);

        expect(definedToken.name).to_be("MY_MACRO");

        const auto &define1 = std::get<::abuild::DefineToken>(tokens[1]);

        expect(define1.name).to_be("MY_MACRO");
        expect(define1.value).to_be("");

        const auto &condition2 = std::get<::abuild::IfToken>(tokens[3]);
        assert_(condition2.elements.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::NotToken>(condition2.elements[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::DefinedToken>(condition2.elements[1])).to_be(true);

        const auto &equalsToken = std::get<::abuild::DefinedToken>(condition2.elements[1]);

        expect(equalsToken.name).to_be("MY_MACRO");

        const auto &define2 = std::get<::abuild::DefineToken>(tokens[4]);

        expect(define2.name).to_be("MY_OTHER_MACRO");
        expect(define2.value).to_be("");
    });
});
