import atest;
import abuild.cache;
import abuild.test_utilities;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("Configuration", [] { // NOLINT(cert-err58-cpp)
    test("", [] {
        const ::abuild::TestFile file{"./abuild.cache_test.yaml"};
        ::abuild::Cache cache{file.path()};

        ::abuild::Toolchain gcc = cache.add_toolchain("gcc");
        ::abuild::Configuration gccRelease = cache.add_configuration(gcc, "release");
    });
});
