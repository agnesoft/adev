import atest;
import abuild.cpptokenizer;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("include local", [] { // NOLINT(cert-err58-cpp)
    test("normal include", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#include \"header.hpp\"");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::IncludeLocalToken>(tokens[0])).to_be(true);

        expect(std::get<::abuild::IncludeLocalToken>(tokens[0]).name).to_be("header.hpp");
    });

    test("no space", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#include\"header.hpp\"");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::IncludeLocalToken>(tokens[0])).to_be(true);

        expect(std::get<::abuild::IncludeLocalToken>(tokens[0]).name).to_be("header.hpp");
    });

    test("extra spaces", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("    #   include    \"header.hpp\"");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::IncludeLocalToken>(tokens[0])).to_be(true);

        expect(std::get<::abuild::IncludeLocalToken>(tokens[0]).name).to_be("header.hpp");
    });

    test("with comments", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("/*\n\n*/#include/*\n\n*/\"header.hpp\"");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::IncludeLocalToken>(tokens[0])).to_be(true);

        expect(std::get<::abuild::IncludeLocalToken>(tokens[0]).name).to_be("header.hpp");
    });

    test("space in the middle", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#include \"hea der.hpp\"");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::IncludeLocalToken>(tokens[0])).to_be(true);

        expect(std::get<::abuild::IncludeLocalToken>(tokens[0]).name).to_be("hea der.hpp");
    });

    test("extra leading & trailing space", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#include \"  header.hpp  \"");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::IncludeLocalToken>(tokens[0])).to_be(true);

        expect(std::get<::abuild::IncludeLocalToken>(tokens[0]).name).to_be("header.hpp");
    });

    test("trailing comment", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#include \"header.hpp\" /* some comment */");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::IncludeLocalToken>(tokens[0])).to_be(true);

        expect(std::get<::abuild::IncludeLocalToken>(tokens[0]).name).to_be("header.hpp");
    });

    test("with macro newlines", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("# \\\ninclude \\ \n\"my_header.hpp\"\n");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::IncludeLocalToken>(tokens[0])).to_be(true);

        expect(std::get<::abuild::IncludeLocalToken>(tokens[0]).name).to_be("my_header.hpp");
    });

    test("missing closing quote", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#include \"aaa");

        assert_(tokens.size()).to_be(0U);
    });

    test("missing opening quote", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#include aaa\"");

        assert_(tokens.size()).to_be(0U);
    });

    test("missing opening quote with another include", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("#include \"header.hpp\n#include \"otherheader.hpp\"");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::IncludeLocalToken>(tokens[0])).to_be(true);

        expect(std::get<::abuild::IncludeLocalToken>(tokens[0]).name).to_be("otherheader.hpp");
    });
});
