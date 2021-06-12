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
                                           {{".abuild", "{ \"settings\": { \"executableFilenames\": [ \"MAIN\" ] } }"}}};

        abuild::Settings settings;
        abuild::Override{testProject.projectRoot()}.applyOverride(&settings);

        expect(settings.executableFilenames()).toBe(std::unordered_set<std::string>{"MAIN"});
    });

    test("ignoreDirectories", [] {
        TestProjectWithContent testProject{"abuild_test_override_test",
                                           {{".abuild", "{ \"settings\": { \"ignoreDirectories\": [ \"!old\", \"_old\" ] } }"}}};

        abuild::Settings settings;
        abuild::Override{testProject.projectRoot()}.applyOverride(&settings);

        expect(settings.ignoreDirectories()).toBe(std::unordered_set<std::string>{"!old", "_old"});
    });

    test("projectNameSeparator", [] {
        TestProjectWithContent testProject{"abuild_test_override_test",
                                           {{".abuild", "{ \"settings\": { \"projectNameSeparator\": \"_\" } }"}}};

        abuild::Settings settings;
        abuild::Override{testProject.projectRoot()}.applyOverride(&settings);

        expect(settings.projectNameSeparator()).toBe("_");
    });

    test("skipDirectories", [] {
        TestProjectWithContent testProject{"abuild_test_override_test",
                                           {{".abuild", "{ \"settings\": { \"skipDirectories\": [ \"abc\", \"cde\", \"efg\" ] } }"}}};

        abuild::Settings settings;
        abuild::Override{testProject.projectRoot()}.applyOverride(&settings);

        expect(settings.skipDirectories()).toBe(std::unordered_set<std::string>{"abc", "cde", "efg"});
    });

    test("squashDirectories", [] {
        TestProjectWithContent testProject{"abuild_test_override_test",
                                           {{".abuild", "{ \"settings\": { \"squashDirectories\": [ \"SrC\", \"inc\" ] } }"}}};

        abuild::Settings settings;
        abuild::Override{testProject.projectRoot()}.applyOverride(&settings);

        expect(settings.squashDirectories()).toBe(std::unordered_set<std::string>{"SrC", "inc"});
    });

    test("testDirectories", [] {
        TestProjectWithContent testProject{"abuild_test_override_test",
                                           {{".abuild", "{ \"settings\": { \"testDirectories\": [ \"tst\" ] } }"}}};

        abuild::Settings settings;
        abuild::Override{testProject.projectRoot()}.applyOverride(&settings);

        expect(settings.testDirectories()).toBe(std::unordered_set<std::string>{"tst"});
    });
});