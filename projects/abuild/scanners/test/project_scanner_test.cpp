import atest;
import abuild.scanners;
import abuild.test_utilities;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("ProjectScanner", [] { // NOLINT(cert-err58-cpp)
    test("single file project", [] {
        const ::abuild::TestProject testProject{
            "project_scanner_test",
            {{"main.cpp", ""}}};

        ::abuild::Cache cache{testProject.root() / "abuild.cache_test.yaml"};
        ::abuild::ProjectScanner{cache}.scan();

        ::abuild::Project *project = cache.project("project_scanner_test");

        assert_(project).not_to_be(nullptr);
        expect(project->name).to_be("project_scanner_test");
        assert_(project->sources.size()).to_be(1U);
        expect(project->sources[0]->path).to_be("project_scanner_test/main.cpp");
    });
});
