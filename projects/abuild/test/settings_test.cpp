import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::Settings", [] {
    test("c++ source extensions", [] {
        expect(abuild::Settings{}.cppSourceExtensions())
            .toBe(std::unordered_set<std::string>{
                ".cc",
                ".cpp",
                ".cxx",
                ".ixx"});
    });

    test("c++ header extensions", [] {
        expect(abuild::Settings{}.cppHeaderExtensions())
            .toBe(std::unordered_set<std::string>{
                ".h",
                ".hpp",
                ".hxx"});
    });

    test("ignore directories", [] {
        expect(abuild::Settings{}.ignoreDirectories())
            .toBe(std::unordered_set<std::string>{
                "build"});
    });

    test("project name Separator", [] {
        expect(abuild::Settings{}.projectNameSeparator()).toBe(".");
    });

    test("skip directories", [] {
        expect(abuild::Settings{}.skipDirectories())
            .toBe(std::unordered_set<std::string>{
                "Projects",
                "projects"});
    });

    test("squash directories", [] {
        expect(abuild::Settings{}.squashDirectories())
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
        expect(abuild::Settings{}.testDirectories())
            .toBe(std::unordered_set<std::string>{
                "Test",
                "Tests",
                "test",
                "tests"});
    });

    test("executable filenames", [] {
        expect(abuild::Settings{}.executableFilenames())
            .toBe(std::unordered_set<std::string>{
                "main",
                "Main",
                "WinMain"});
    });

    test("clang install directory", [] {
#ifdef _WIN32
        expect(abuild::Settings{}.clangInstallDirectory()).toBe("C:/Program Files/LLVM");
#else
                expect(abuild::Settings{}.clangInstallDirectory()).toBe("/usr");
#endif
    });

    test("gcc install directory", [] {
        expect(abuild::Settings{}.gccInstallDirectory()).toBe("/usr");
    });

    test("MSVC install directory", [] {
        expect(abuild::Settings{}.msvcInstallDirectory()).toBe("C:/Program Files (x86)/Microsoft Visual Studio/");
    });
});
