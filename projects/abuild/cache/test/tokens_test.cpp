import atest;
import abuild.cache;
import abuild.test_utilities;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("Tokens", [] { // NOLINT(cert-err58-cpp)
    test("Token - IfToken", [] {
        const ::abuild::TestProject testProject{
            "cache_test",
            {{"main.cpp", ""}}};
        const ::abuild::Token token{::abuild::IfToken{
            .elements = {
                ::abuild::DefinedToken{.name = "MY_MACRO"},
                ::abuild::AndToken{},
                ::abuild::LeftBracketToken{},
                ::abuild::EqualsToken{.left = "10", .right = "SOME_VALUE"},
                ::abuild::OrToken{},
                ::abuild::GreaterThanOrEqualsToken{.left = "10", .right = "SOME_VALUE"},
                ::abuild::OrToken{},
                ::abuild::GreaterThanToken{.left = "10", .right = "SOME_VALUE"},
                ::abuild::OrToken{},
                ::abuild::LessThanOrEqualsToken{.left = "10", .right = "SOME_VALUE"},
                ::abuild::OrToken{},
                ::abuild::LessThanToken{.left = "10", .right = "SOME_VALUE"},
                ::abuild::RightBracketToken{},
                ::abuild::AndToken{},
                ::abuild::NotToken{},
                ::abuild::HasIncludeExternalToken{.name = "my_header.hpp"},
                ::abuild::OrToken{},
                ::abuild::HasIncludeLocalToken{.name = "include/some_header.h"}}}};

        {
            ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
            ::abuild::SourceFile *source = cache.add_source_file(::abuild::File{testProject.root() / "main.cpp"}, "my_project");
            source->tokens.push_back(token);
        }

        ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
        ::abuild::SourceFile *source = cache.exact_source_file(testProject.root() / "main.cpp");
        assert_(source).not_to_be(nullptr);
        assert_(source->tokens.size()).to_be(1U);
        expect(source->tokens[0]).to_be(token);
    });

    test("Token - ElseToken", [] {
        const ::abuild::TestProject testProject{
            "cache_test",
            {{"main.cpp", ""}}};
        const ::abuild::Token token = ::abuild::ElseToken{};

        {
            ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
            ::abuild::SourceFile *source = cache.add_source_file(::abuild::File{testProject.root() / "main.cpp"}, "my_project");
            source->tokens.push_back(token);
        }

        ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
        ::abuild::SourceFile *source = cache.exact_source_file(testProject.root() / "main.cpp");
        assert_(source).not_to_be(nullptr);
        assert_(source->tokens.size()).to_be(1U);
        expect(source->tokens[0]).to_be(token);
    });

    test("Token - EndIfToken", [] {
        const ::abuild::TestProject testProject{
            "cache_test",
            {{"main.cpp", ""}}};
        const ::abuild::Token token = ::abuild::EndIfToken{};

        {
            ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
            ::abuild::SourceFile *source = cache.add_source_file(::abuild::File{testProject.root() / "main.cpp"}, "my_project");
            source->tokens.push_back(token);
        }

        ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
        ::abuild::SourceFile *source = cache.exact_source_file(testProject.root() / "main.cpp");
        assert_(source).not_to_be(nullptr);
        assert_(source->tokens.size()).to_be(1U);
        expect(source->tokens[0]).to_be(token);
    });

    test("Token - DefineToken", [] {
        const ::abuild::TestProject testProject{
            "cache_test",
            {{"main.cpp", ""}}};
        const ::abuild::Token token = ::abuild::DefineToken{.name = "MY_MACRO", .value = "1"};

        {
            ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
            ::abuild::SourceFile *source = cache.add_source_file(::abuild::File{testProject.root() / "main.cpp"}, "my_project");
            source->tokens.push_back(token);
        }

        ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
        ::abuild::SourceFile *source = cache.exact_source_file(testProject.root() / "main.cpp");
        assert_(source).not_to_be(nullptr);
        assert_(source->tokens.size()).to_be(1U);
        expect(source->tokens[0]).to_be(token);
    });

    test("Token - ImportIncludeExternalToken", [] {
        const ::abuild::TestProject testProject{
            "cache_test",
            {{"main.cpp", ""}}};
        const ::abuild::Token token = ::abuild::ImportIncludeExternalToken{.name = "include/my_header.hpp", .exported = true};

        {
            ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
            ::abuild::SourceFile *source = cache.add_source_file(::abuild::File{testProject.root() / "main.cpp"}, "my_project");
            source->tokens.push_back(token);
        }

        ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
        ::abuild::SourceFile *source = cache.exact_source_file(testProject.root() / "main.cpp");
        assert_(source).not_to_be(nullptr);
        assert_(source->tokens.size()).to_be(1U);
        expect(source->tokens[0]).to_be(token);
    });

    test("Token - ImportIncludeLocalToken", [] {
        const ::abuild::TestProject testProject{
            "cache_test",
            {{"main.cpp", ""}}};
        const ::abuild::Token token = ::abuild::ImportIncludeLocalToken{.name = "include/my_header.hpp", .exported = true};

        {
            ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
            ::abuild::SourceFile *source = cache.add_source_file(::abuild::File{testProject.root() / "main.cpp"}, "my_project");
            source->tokens.push_back(token);
        }

        ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
        ::abuild::SourceFile *source = cache.exact_source_file(testProject.root() / "main.cpp");
        assert_(source).not_to_be(nullptr);
        assert_(source->tokens.size()).to_be(1U);
        expect(source->tokens[0]).to_be(token);
    });

    test("Token - ImportModulePartitionToken", [] {
        const ::abuild::TestProject testProject{
            "cache_test",
            {{"main.cpp", ""}}};
        const ::abuild::Token token = ::abuild::ImportModulePartitionToken{.name = "my_partition", .exported = true};

        {
            ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
            ::abuild::SourceFile *source = cache.add_source_file(::abuild::File{testProject.root() / "main.cpp"}, "my_project");
            source->tokens.push_back(token);
        }

        ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
        ::abuild::SourceFile *source = cache.exact_source_file(testProject.root() / "main.cpp");
        assert_(source).not_to_be(nullptr);
        assert_(source->tokens.size()).to_be(1U);
        expect(source->tokens[0]).to_be(token);
    });

    test("Token - ImportModuleToken", [] {
        const ::abuild::TestProject testProject{
            "cache_test",
            {{"main.cpp", ""}}};
        const ::abuild::Token token = ::abuild::ImportModuleToken{.name = "my_module", .exported = true};

        {
            ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
            ::abuild::SourceFile *source = cache.add_source_file(::abuild::File{testProject.root() / "main.cpp"}, "my_project");
            source->tokens.push_back(token);
        }

        ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
        ::abuild::SourceFile *source = cache.exact_source_file(testProject.root() / "main.cpp");
        assert_(source).not_to_be(nullptr);
        assert_(source->tokens.size()).to_be(1U);
        expect(source->tokens[0]).to_be(token);
    });

    test("Token - IncludeExternalToken", [] {
        const ::abuild::TestProject testProject{
            "cache_test",
            {{"main.cpp", ""}}};
        const ::abuild::Token token = ::abuild::IncludeExternalToken{.name = "include/my_header.hpp"};

        {
            ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
            ::abuild::SourceFile *source = cache.add_source_file(::abuild::File{testProject.root() / "main.cpp"}, "my_project");
            source->tokens.push_back(token);
        }

        ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
        ::abuild::SourceFile *source = cache.exact_source_file(testProject.root() / "main.cpp");
        assert_(source).not_to_be(nullptr);
        assert_(source->tokens.size()).to_be(1U);
        expect(source->tokens[0]).to_be(token);
    });

    test("Token - IncludeLocalToken", [] {
        const ::abuild::TestProject testProject{
            "cache_test",
            {{"main.cpp", ""}}};
        const ::abuild::Token token = ::abuild::IncludeLocalToken{.name = "include/my_header.hpp"};

        {
            ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
            ::abuild::SourceFile *source = cache.add_source_file(::abuild::File{testProject.root() / "main.cpp"}, "my_project");
            source->tokens.push_back(token);
        }

        ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
        ::abuild::SourceFile *source = cache.exact_source_file(testProject.root() / "main.cpp");
        assert_(source).not_to_be(nullptr);
        assert_(source->tokens.size()).to_be(1U);
        expect(source->tokens[0]).to_be(token);
    });

    test("Token - ModulePartitionToken", [] {
        const ::abuild::TestProject testProject{
            "cache_test",
            {{"main.cpp", ""}}};
        const ::abuild::Token token = ::abuild::ModulePartitionToken{.mod = "my_module", .name = "my_partition", .exported = true};

        {
            ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
            ::abuild::SourceFile *source = cache.add_source_file(::abuild::File{testProject.root() / "main.cpp"}, "my_project");
            source->tokens.push_back(token);
        }

        ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
        ::abuild::SourceFile *source = cache.exact_source_file(testProject.root() / "main.cpp");
        assert_(source).not_to_be(nullptr);
        assert_(source->tokens.size()).to_be(1U);
        expect(source->tokens[0]).to_be(token);
    });

    test("Token - ModuleToken", [] {
        const ::abuild::TestProject testProject{
            "cache_test",
            {{"main.cpp", ""}}};
        const ::abuild::Token token = ::abuild::ModuleToken{.name = "my_module", .exported = true};

        {
            ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
            ::abuild::SourceFile *source = cache.add_source_file(::abuild::File{testProject.root() / "main.cpp"}, "my_project");
            source->tokens.push_back(token);
        }

        ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
        ::abuild::SourceFile *source = cache.exact_source_file(testProject.root() / "main.cpp");
        assert_(source).not_to_be(nullptr);
        assert_(source->tokens.size()).to_be(1U);
        expect(source->tokens[0]).to_be(token);
    });

    test("Token - UndefToken", [] {
        const ::abuild::TestProject testProject{
            "cache_test",
            {{"main.cpp", ""}}};
        const ::abuild::Token token = ::abuild::UndefToken{.name = "MY_MACRO"};

        {
            ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
            ::abuild::SourceFile *source = cache.add_source_file(::abuild::File{testProject.root() / "main.cpp"}, "my_project");
            source->tokens.push_back(token);
        }

        ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
        ::abuild::SourceFile *source = cache.exact_source_file(testProject.root() / "main.cpp");
        assert_(source).not_to_be(nullptr);
        assert_(source->tokens.size()).to_be(1U);
        expect(source->tokens[0]).to_be(token);
    });
});
