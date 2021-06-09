import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::BuildCache", [] {
    test("non existing override file", [] {
        const std::filesystem::path root = TestProject{"abuild_test_override_test", {}}.projectRoot();
        expect([&] { abuild::Override{root}; }).toThrow<std::filesystem::filesystem_error>();
    });

    test("existing override file", [] {
        TestProjectWithContent testProject{"abuild_test_override_test",
                                           {{".abuild", "{}"}}};

        abuild::Override dataOverride{testProject.projectRoot()};
    });

    test("bad override file", [] {
        TestProjectWithContent testProject{"abuild_test_override_test",
                                           {{".abuild", "{"}}};

        abuild::Override dataOverride{testProject.projectRoot()};
    });
});