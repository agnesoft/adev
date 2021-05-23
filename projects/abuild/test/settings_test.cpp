import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::Settings", [] {
    test("type traits", [] {
        expect(std::is_default_constructible_v<abuild::Settings>).toBe(true);
        expect(std::is_copy_constructible_v<abuild::Settings>).toBe(true);
        expect(std::is_nothrow_move_constructible_v<abuild::Settings>).toBe(true);
        expect(std::is_copy_assignable_v<abuild::Settings>).toBe(true);
        expect(std::is_nothrow_move_assignable_v<abuild::Settings>).toBe(true);
        expect(std::is_nothrow_destructible_v<abuild::Settings>).toBe(true);
    });

    test("c++ source extensions", [] {
        expect(abuild::Settings::cppSourceExtensions())
            .toBe(std::unordered_set<std::string>{
                ".cc",
                ".cpp",
                ".cxx",
                ".ixx"});
    });

    test("c++ header extensions", [] {
        expect(abuild::Settings::cppHeaderExtensions())
            .toBe(std::unordered_set<std::string>{
                ".h",
                ".hpp",
                ".hxx"});
    });

    test("ignore directories", [] {
        expect(abuild::Settings::ignoreDirectories())
            .toBe(std::unordered_set<std::string>{
                "build"});
    });

    test("project name Separator", [] {
        expect(abuild::Settings::projectNameSeparator()).toBe(".");
    });

    test("skip directories", [] {
        expect(abuild::Settings::skipDirectories())
            .toBe(std::unordered_set<std::string>{
                "Projects",
                "projects"});
    });

    test("squash directories", [] {
        expect(abuild::Settings::squashDirectories())
            .toBe(std::unordered_set<std::string>{
                "Include",
                "Includes",
                "SRC",
                "Source",
                "Sources",
                "Src",
                "include",
                "includes",
                "source",
                "sources",
                "src",
                "srcs"});
    });

    test("test directories", [] {
        expect(abuild::Settings::testDirectories())
            .toBe(std::unordered_set<std::string>{
                "Test",
                "Tests",
                "test",
                "tests"});
    });

    test("executable filenames", [] {
        expect(abuild::Settings::executableFilenames())
            .toBe(std::unordered_set<std::string>{
                "main",
                "Main",
                "WinMain"});
    });
});
