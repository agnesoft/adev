import atest;
import abuild.cpptokenizer;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("__has_include", [] { // NOLINT(cert-err58-cpp)
    test("if __has_include(\"\")", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if __has_include(\"my_header.hpp\")");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::HasIncludeLocalToken>(condition.elements[0])).to_be(true);

        const auto &token = std::get<::abuild::HasIncludeLocalToken>(condition.elements[0]);

        expect(token.include).to_be("my_header.hpp");
    });

    test("if __has_include(<>)", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if __has_include(<my_header.hpp>)");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::HasIncludeExternalToken>(condition.elements[0])).to_be(true);

        const auto &token = std::get<::abuild::HasIncludeExternalToken>(condition.elements[0]);

        expect(token.include).to_be("my_header.hpp");
    });

    test("if __has_include(\"\")", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if __has_include(\"\")\n#endif");

        assert_(tokens.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[1])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::HasIncludeLocalToken>(condition.elements[0])).to_be(true);

        const auto &token = std::get<::abuild::HasIncludeLocalToken>(condition.elements[0]);

        expect(token.include).to_be("");
    });

    test("leading & trailing space", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if __has_include(\"   my_header.hpp   \")");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::HasIncludeLocalToken>(condition.elements[0])).to_be(true);

        const auto &token = std::get<::abuild::HasIncludeLocalToken>(condition.elements[0]);

        expect(token.include).to_be("my_header.hpp");
    });

    test("bad has include", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if __has_include(<my_header)\n#endif");

        assert_(tokens.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[1])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(0U);
    });

    test("missing brackets", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if __has_include <my_header>\n#endif");

        assert_(tokens.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[1])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(0U);
    });

    test("unknown include type", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if __has_include(my_header)\n#endif");

        assert_(tokens.size()).to_be(2U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);
        assert_(std::holds_alternative<::abuild::EndIfToken>(tokens[1])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(0U);
    });

    test("unexpected end of file", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#if __has_include(<my_header");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::IfToken>(tokens[0])).to_be(true);

        const auto &condition = std::get<::abuild::IfToken>(tokens[0]);

        assert_(condition.elements.size()).to_be(0U);
    });
});
