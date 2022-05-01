import atest;
import abuild.cache;
import abuild.test_utilities;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("Cache", [] { // NOLINT(cert-err58-cpp)
    test("configuration name", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::string name = "release";

        {
            ::abuild::Cache cache{testFile.path()};
            cache.set_configuration_name(name);
            expect(cache.configuration_name()).to_be(name);
        }

        ::abuild::Cache cache{testFile.path()};
        expect(cache.configuration_name()).to_be(name);
    });

    test("build root", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        expect(::abuild::Cache{testFile.path()}.build_root()).to_be(testFile.path().parent_path());
    });
});
