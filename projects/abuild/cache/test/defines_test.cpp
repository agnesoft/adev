import atest;
import abuild.cache;
import abuild.test_utilities;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("Defines", [] { // NOLINT(cert-err58-cpp)
    test("defines", [] {
        const ::abuild::TestFile file{"./abuild.cache_test.yaml"};
        const std::vector<::abuild::DefineToken> defines{
            {.name = "_WIN32_"},
            {                     .name = "__clang__"                },
            { .name = "SOME_DEFINE",                 .value = "1"}
        };

        {
            ::abuild::Cache cache{file.path()};
            assert_(cache.defines()).to_be(std::vector<::abuild::DefineToken>{});
            cache.set_defines(std::vector<::abuild::DefineToken>{defines});
            expect(cache.defines()).to_be(defines);
        }

        ::abuild::Cache cache{file.path()};
        expect(cache.defines()).to_be(defines);
    });
});
