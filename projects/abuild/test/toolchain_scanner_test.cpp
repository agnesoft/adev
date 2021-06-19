import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::ToolchainScanner", [] {
    test("msvc 2019", [] {
        TestProjectWithContent testProject{"abuild_toolchain_scanner_test",
                                           {{".abuild", "{ \"msvcInstallDirectory\": \".\" }"},
                                            {"2019/Community/VC/Tools/MSVC/14.29.30037/bin/Hostx64/x64/cl.exe", ""},
                                            {"2019/Community/VC/Tools/MSVC/14.29.30037/bin/Hostx64/x64/link.exe", ""},
                                            {"2019/Community/VC/Tools/MSVC/14.29.30037/bin/Hostx64/x64/lib.exe", ""},
                                            {"2019/Community/VC/Tools/MSVC/14.29.30037/include/algorithm", ""},
                                            {"2019/Community/VC/Tools/MSVC/14.29.30037/lib/x64/libcmt.lib", ""}}};

        abuild::BuildCache cache(testProject.projectRoot());
        abuild::ToolchainScanner{cache};

        assert_(cache.toolchains().size()).toBe(1);
        expect(cache.toolchains()[0]->name).toBe("msvc2019_x64_14.29.30037");
        expect(cache.toolchains()[0]->compiler).toBe(testProject.projectRoot() / "2019/Community/VC/Tools/MSVC/14.29.30037/bin/Hostx64/x64/cl.exe");
        expect(cache.toolchains()[0]->linker).toBe(testProject.projectRoot() / "2019/Community/VC/Tools/MSVC/14.29.30037/bin/Hostx64/x64/link.exe");
        expect(cache.toolchains()[0]->archiver).toBe(testProject.projectRoot() / "2019/Community/VC/Tools/MSVC/14.29.30037/bin/Hostx64/x64/lib.exe");
        expect(cache.toolchains()[0]->compilerFlags).toBe(std::unordered_set<std::string>{"/nologo", "/EHsc", "/std:c++latest", "/W4", "/c", "/TP"});
        expect(cache.toolchains()[0]->linkerFlags).toBe(std::unordered_set<std::string>{"/NOLOGO"});
        expect(cache.toolchains()[0]->archiverFlags).toBe(std::unordered_set<std::string>{"/NOLOGO"});
        expect(cache.toolchains()[0]->includePath).toBe(testProject.projectRoot() / "2019/Community/VC/Tools/MSVC/14.29.30037/include");
        expect(cache.toolchains()[0]->libPath).toBe(testProject.projectRoot() / "2019/Community/VC/Tools/MSVC/14.29.30037/bin/lib/x64");
    });

    test("gcc 11", [] {
    });

    test("clang 11", [] {
    });
});
