import atest;
import abuild.cache;
import abuild.test_utilities;

using ::atest::assert_;
using ::atest::assert_fail;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("Cache", [] { // NOLINT(cert-err58-cpp)
    test("Token - IfToken", [] {
        const ::abuild::TestFile file{"./abuild.cache_test.yaml"};
        const ::abuild::Token token{::abuild::IfToken{
            .elements = {
                ::abuild::DefinedToken{.name = "MY_MACRO"},
                ::abuild::EqualsToken{.left = "10", .right = "SOME_VALUE"},
                ::abuild::GreaterThanOrEqualsToken{.left = "10", .right = "SOME_VALUE"},
                ::abuild::GreaterThanToken{.left = "10", .right = "SOME_VALUE"},
                ::abuild::LessThanOrEqualsToken{.left = "10", .right = "SOME_VALUE"},
                ::abuild::LessThanToken{.left = "10", .right = "SOME_VALUE"},
                ::abuild::LeftBracketToken{},
                ::abuild::RightBracketToken{},
                ::abuild::HasIncludeExternalToken{.name = "my_header.hpp"},
                ::abuild::HasIncludeLocalToken{.name = "include/some_header.h"},
                ::abuild::OrToken{},
                ::abuild::NotToken{},
                ::abuild::RightBracketToken{}}}};

        {
            ::abuild::Cache cache{file.path()};
            ::abuild::SourceFile *source = cache.add_source_file("main.cpp");
            source->tokens.push_back(token);
        }

        ::abuild::Cache cache{file.path()};
        ::abuild::SourceFile *source = cache.source_file("main.cpp");
        assert_fail(source).to_be(nullptr);
        assert_(source->tokens.size()).to_be(1U);
        expect(source->tokens[0]).to_be(token);
    });

    test("Token - ElseToken", [] {
    });

    test("Token - EndIfToken", [] {
    });

    test("Token - DefineToken", [] {
    });

    test("Token - ImportIncludeExternalToken", [] {
    });

    test("Token - ImportIncludeLocalToken", [] {
    });

    test("Token - ImportModulePartitionToken", [] {
    });

    test("Token - ImportModuleToken", [] {
    });

    test("Token - IncludeExternalToken", [] {
    });

    test("Token - IncludeLocalToken", [] {
    });

    test("Token - ModulePartitionToken", [] {
    });

    test("Token - ModuleToken", [] {
    });

    test("Token - UndefToken", [] {
    });
});
