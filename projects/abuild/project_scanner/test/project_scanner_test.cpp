import atest;
import abuild.project_scanner;
import abuild.test_utilities;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("ProjectScanner", [] { // NOLINT(cert-err58-cpp)
    test("single project", [] {
        const ::abuild::TestProject testProject{
            "project_scanner_test",
            {{"main.cpp", ""}}};

        ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
        ::abuild::ProjectScanner{cache}.scan();

        expect_fail(cache.project("project_scanner_test")).to_be(nullptr);
    });
});
