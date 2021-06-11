import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::Override (settings)", [] {
    test("cppHeaderExtensions", [] {
        TestProjectWithContent testProject{"abuild_test_override_test",
                                           {{".abuild", "{ \"settings\": { \"cppHeaderExtensions\": [ \".a\", \".b\" ] } }"}}};

        abuild::Settings settings;
        abuild::Override{testProject.projectRoot()}.applyOverride(&settings);

        expect(settings.cppHeaderExtensions()).toBe(std::unordered_set<std::string>{".a", ".b"});
    });

    test("cppSourceExtensions", [] {
        TestProjectWithContent testProject{"abuild_test_override_test",
                                           {{".abuild", "{ \"settings\": { \"cppSourceExtensions\": [ \".c\", \".d\", \".e\" ] } }"}}};

        abuild::Settings settings;
        abuild::Override{testProject.projectRoot()}.applyOverride(&settings);

        expect(settings.cppSourceExtensions()).toBe(std::unordered_set<std::string>{".c", ".d", ".e"});
    });

    test("executableFilenames", [] {
        TestProjectWithContent testProject{"abuild_test_override_test",
                                           {{".abuild", "{ \"settings\": {  } }"}}};

        abuild::Settings settings;
        abuild::Override{testProject.projectRoot()}.applyOverride(&settings);
    });

    test("ignoreDirectories", [] {
        TestProjectWithContent testProject{"abuild_test_override_test",
                                           {{".abuild", "{ \"settings\": {  } }"}}};

        abuild::Settings settings;
        abuild::Override{testProject.projectRoot()}.applyOverride(&settings);
    });

    test("projectNameSeparator", [] {
        TestProjectWithContent testProject{"abuild_test_override_test",
                                           {{".abuild", "{ \"settings\": {  } }"}}};

        abuild::Settings settings;
        abuild::Override{testProject.projectRoot()}.applyOverride(&settings);
    });

    test("skipDirectories", [] {
        TestProjectWithContent testProject{"abuild_test_override_test",
                                           {{".abuild", "{ \"settings\": {  } }"}}};

        abuild::Settings settings;
        abuild::Override{testProject.projectRoot()}.applyOverride(&settings);
    });

    test("squashDirectories", [] {
        TestProjectWithContent testProject{"abuild_test_override_test",
                                           {{".abuild", "{ \"settings\": {  } }"}}};

        abuild::Settings settings;
        abuild::Override{testProject.projectRoot()}.applyOverride(&settings);
    });

    test("testDirectories", [] {
        TestProjectWithContent testProject{"abuild_test_override_test",
                                           {{".abuild", "{ \"settings\": {  } }"}}};

        abuild::Settings settings;
        abuild::Override{testProject.projectRoot()}.applyOverride(&settings);
    });
});