import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::ToolchainScanner", [] {
    test("msvc2019_x64_Community", [] {
        const std::filesystem::path msvcInstallDirectory = std::filesystem::current_path() / "abuild_toolchain_scanner_test";
        TestProjectWithContent testProject{"abuild_toolchain_scanner_test",
                                           {{".abuild", "{ \"settings\": { \"gccInstallDirectory\": \"\", \"clangInstallDirectory\": \"\", \"msvcInstallDirectory\": \"" + msvcInstallDirectory.generic_string() + "\" } }"},
                                            {"2019/Community/VC/Tools/MSVC/14.29.30037/bin/Hostx64/x64/cl.exe", ""},
                                            {"2019/Community/VC/Tools/MSVC/14.29.30037/bin/Hostx64/x64/link.exe", ""},
                                            {"2019/Community/VC/Tools/MSVC/14.29.30037/bin/Hostx64/x64/lib.exe", ""},
                                            {"2019/Community/VC/Tools/MSVC/14.29.30037/include/algorithm", ""},
                                            {"2019/Community/VC/Tools/MSVC/14.29.30037/lib/x64/libcmt.lib", ""}}};

        abuild::BuildCache cache(testProject.projectRoot());
        abuild::ToolchainScanner{cache};

        assert_(cache.toolchains().size()).toBe(1u);
        expect(cache.toolchains()[0]->name).toBe("msvc2019_x64_Community_14.29.30037");
        expect(cache.toolchains()[0]->compiler).toBe(testProject.projectRoot() / "2019/Community/VC/Tools/MSVC/14.29.30037/bin/Hostx64/x64/cl.exe");
        expect(cache.toolchains()[0]->linker).toBe(testProject.projectRoot() / "2019/Community/VC/Tools/MSVC/14.29.30037/bin/Hostx64/x64/link.exe");
        expect(cache.toolchains()[0]->archiver).toBe(testProject.projectRoot() / "2019/Community/VC/Tools/MSVC/14.29.30037/bin/Hostx64/x64/lib.exe");
        expect(cache.toolchains()[0]->compilerFlags).toBe(std::unordered_set<std::string>{"/nologo", "/EHsc", "/std:c++latest", "/W4", "/c", "/TP"});
        expect(cache.toolchains()[0]->linkerFlags).toBe(std::unordered_set<std::string>{"/NOLOGO"});
        expect(cache.toolchains()[0]->archiverFlags).toBe(std::unordered_set<std::string>{"/NOLOGO"});
        expect(cache.toolchains()[0]->includePath).toBe(testProject.projectRoot() / "2019/Community/VC/Tools/MSVC/14.29.30037/include");
        expect(cache.toolchains()[0]->libPath).toBe(testProject.projectRoot() / "2019/Community/VC/Tools/MSVC/14.29.30037/lib/x64");
    });

    test("msvc2019_x32_Professional", [] {
        const std::filesystem::path msvcInstallDirectory = std::filesystem::current_path() / "abuild_toolchain_scanner_test";
        TestProjectWithContent testProject{"abuild_toolchain_scanner_test",
                                           {{".abuild", "{ \"settings\": { \"gccInstallDirectory\": \"\", \"clangInstallDirectory\": \"\", \"msvcInstallDirectory\": \"" + msvcInstallDirectory.generic_string() + "\" } }"},
                                            {"2019/Professional/VC/Tools/MSVC/14.29.30037/bin/Hostx64/x64/cl.exe", ""},
                                            {"2019/Professional/VC/Tools/MSVC/14.29.30037/bin/Hostx64/x32/cl.exe", ""},
                                            {"2019/Professional/VC/Tools/MSVC/14.29.30037/bin/Hostx64/x64/link.exe", ""},
                                            {"2019/Professional/VC/Tools/MSVC/14.29.30037/bin/Hostx64/x32/link.exe", ""},
                                            {"2019/Professional/VC/Tools/MSVC/14.29.30037/bin/Hostx64/x64/lib.exe", ""},
                                            {"2019/Professional/VC/Tools/MSVC/14.29.30037/bin/Hostx64/x32/lib.exe", ""},
                                            {"2019/Professional/VC/Tools/MSVC/14.29.30037/include/algorithm", ""},
                                            {"2019/Professional/VC/Tools/MSVC/14.29.30037/lib/x64/libcmt.lib", ""},
                                            {"2019/Professional/VC/Tools/MSVC/14.29.30037/lib/x32/libcmt.lib", ""}}};

        abuild::BuildCache cache(testProject.projectRoot());
        abuild::ToolchainScanner{cache};

        assert_(cache.toolchains().size()).toBe(2u);
        expect(cache.toolchains()[1]->name).toBe("msvc2019_x32_Professional_14.29.30037");
        expect(cache.toolchains()[1]->compiler).toBe(testProject.projectRoot() / "2019/Professional/VC/Tools/MSVC/14.29.30037/bin/Hostx64/x32/cl.exe");
        expect(cache.toolchains()[1]->linker).toBe(testProject.projectRoot() / "2019/Professional/VC/Tools/MSVC/14.29.30037/bin/Hostx64/x32/link.exe");
        expect(cache.toolchains()[1]->archiver).toBe(testProject.projectRoot() / "2019/Professional/VC/Tools/MSVC/14.29.30037/bin/Hostx64/x32/lib.exe");
        expect(cache.toolchains()[1]->compilerFlags).toBe(std::unordered_set<std::string>{"/nologo", "/EHsc", "/std:c++latest", "/W4", "/c", "/TP"});
        expect(cache.toolchains()[1]->linkerFlags).toBe(std::unordered_set<std::string>{"/NOLOGO"});
        expect(cache.toolchains()[1]->archiverFlags).toBe(std::unordered_set<std::string>{"/NOLOGO"});
        expect(cache.toolchains()[1]->includePath).toBe(testProject.projectRoot() / "2019/Professional/VC/Tools/MSVC/14.29.30037/include");
        expect(cache.toolchains()[1]->libPath).toBe(testProject.projectRoot() / "2019/Professional/VC/Tools/MSVC/14.29.30037/lib/x32");
    });

    test("clang", [] {
        const std::filesystem::path clangInstallDirectory = std::filesystem::current_path() / "abuild_toolchain_scanner_test/LLVM";
        TestProjectWithContent testProject{"abuild_toolchain_scanner_test",
                                           {{".abuild", "{ \"settings\": { \"gccInstallDirectory\": \"\", \"clangInstallDirectory\": \"" + clangInstallDirectory.generic_string() + "\", \"msvcInstallDirectory\": \"\" } }"},
#ifdef _WIN32
                                            {"LLVM/bin/clang++.exe", ""},
                                            {"LLVM/bin/lld.exe", ""},
                                            {"LLVM/bin/llvm-ar.exe", ""},
#else
                                                    {"LLVM/bin/clang++", ""},
                                                    {"LLVM/bin/lld", ""},
                                                    {"LLVM/bin/llvm-ar", ""},
#endif
                                            {"LLVM/lib/clang/12/include/emmintrin.h", ""},
                                            {"LLVM/lib/clang/12/lib/linux/somelib.lib", ""}}};

        abuild::BuildCache cache(testProject.projectRoot());
        abuild::ToolchainScanner{cache};

        assert_(cache.toolchains().size()).toBe(1u);
        expect(cache.toolchains()[0]->name).toBe("clang");
#ifdef _WIN32
        expect(cache.toolchains()[0]->compiler).toBe(testProject.projectRoot() / "LLVM/bin/clang++.exe");
        expect(cache.toolchains()[0]->linker).toBe(testProject.projectRoot() / "LLVM/bin/lld.exe");
        expect(cache.toolchains()[0]->archiver).toBe(testProject.projectRoot() / "LLVM/bin/llvm-ar.exe");
#else
                expect(cache.toolchains()[0]->compiler).toBe(testProject.projectRoot() / "LLVM/bin/clang++");
                expect(cache.toolchains()[0]->linker).toBe(testProject.projectRoot() / "LLVM/bin/lld");
                expect(cache.toolchains()[0]->archiver).toBe(testProject.projectRoot() / "LLVM/bin/llvm-ar");
#endif
        expect(cache.toolchains()[0]->compilerFlags).toBe(std::unordered_set<std::string>{"-std=c++20", "-fmodules", "-Wall", "-Wextra", "-pedantic", "-c", "-x c++"});
        expect(cache.toolchains()[0]->linkerFlags).toBe(std::unordered_set<std::string>{});
        expect(cache.toolchains()[0]->archiverFlags).toBe(std::unordered_set<std::string>{});
        expect(cache.toolchains()[0]->includePath).toBe(testProject.projectRoot() / "LLVM/lib/clang/12/include");
        expect(cache.toolchains()[0]->libPath).toBe(testProject.projectRoot() / "LLVM/lib/clang/12/lib/linux");
    });

    test("clang-12", [] {
        const std::filesystem::path clangInstallDirectory = std::filesystem::current_path() / "abuild_toolchain_scanner_test/LLVM";
        TestProjectWithContent testProject{"abuild_toolchain_scanner_test",
                                           {{".abuild", "{ \"settings\": { \"gccInstallDirectory\": \"\", \"clangInstallDirectory\": \"" + clangInstallDirectory.generic_string() + "\", \"msvcInstallDirectory\": \"\" } }"},
#ifdef _WIN32
                                            {"LLVM/bin/clang++-12.exe", ""},
#else
                                                    {"LLVM/bin/clang++-12", ""},
                                                    {"LLVM/bin/ld", ""},
                                                    {"LLVM/bin/ar", ""},
#endif
                                            {"LLVM/lib/clang/12/include/emmintrin.h", ""},
                                            {"LLVM/lib/clang/12/lib/linux/somelib.lib", ""}}};

        abuild::BuildCache cache(testProject.projectRoot());
        abuild::ToolchainScanner{cache};

        assert_(cache.toolchains().size()).toBe(1u);
        expect(cache.toolchains()[0]->name).toBe("clang12");
#ifdef _WIN32
        expect(cache.toolchains()[0]->compiler).toBe(testProject.projectRoot() / "LLVM/bin/clang++-12.exe");
        expect(cache.toolchains()[0]->linker).toBe(testProject.projectRoot() / "LLVM/bin/ld");
        expect(cache.toolchains()[0]->archiver).toBe(testProject.projectRoot() / "LLVM/bin/ar");
#else
                expect(cache.toolchains()[0]->compiler).toBe(testProject.projectRoot() / "LLVM/bin/clang++-12");
                expect(cache.toolchains()[0]->linker).toBe(testProject.projectRoot() / "LLVM/bin/ld");
                expect(cache.toolchains()[0]->archiver).toBe(testProject.projectRoot() / "LLVM/bin/ar");
#endif
        expect(cache.toolchains()[0]->compilerFlags).toBe(std::unordered_set<std::string>{"-std=c++20", "-fmodules", "-Wall", "-Wextra", "-pedantic", "-c", "-x c++"});
        expect(cache.toolchains()[0]->linkerFlags).toBe(std::unordered_set<std::string>{});
        expect(cache.toolchains()[0]->archiverFlags).toBe(std::unordered_set<std::string>{});
        expect(cache.toolchains()[0]->includePath).toBe(testProject.projectRoot() / "LLVM/lib/clang/12/include");
        expect(cache.toolchains()[0]->libPath).toBe(testProject.projectRoot() / "LLVM/lib/clang/12/lib/linux");
    });

    test("gcc", [] {
        const std::filesystem::path gccInstallDirectory = std::filesystem::current_path() / "abuild_toolchain_scanner_test/GCC";
        TestProjectWithContent testProject{"abuild_toolchain_scanner_test",
                                           {{".abuild", "{ \"settings\": { \"gccInstallDirectory\": \"" + gccInstallDirectory.generic_string() + "\", \"clangInstallDirectory\": \"\", \"msvcInstallDirectory\": \"\" } }"},
                                            {"GCC/bin/g++", ""},
                                            {"GCC/bin/ld", ""},
                                            {"GCC/bin/ar", ""},
                                            {"GCC/lib/gcc/x86_64-linux-gnu/9/include/emmintrin.h", ""},
                                            {"GCC/lib/gcc/x86_64-linux-gnu/9/somelib.lib", ""}}};

        abuild::BuildCache cache(testProject.projectRoot());
        abuild::ToolchainScanner{cache};

        assert_(cache.toolchains().size()).toBe(1u);
        expect(cache.toolchains()[0]->name).toBe("gcc");
        expect(cache.toolchains()[0]->compiler).toBe(testProject.projectRoot() / "GCC/bin/g++");
        expect(cache.toolchains()[0]->linker).toBe(testProject.projectRoot() / "GCC/bin/ld");
        expect(cache.toolchains()[0]->archiver).toBe(testProject.projectRoot() / "GCC/bin/ar");
        expect(cache.toolchains()[0]->compilerFlags).toBe(std::unordered_set<std::string>{"-std=c++20", "-fmodules-ts", "-Wall", "-Wextra", "-pedantic", "-c", "-x c++"});
        expect(cache.toolchains()[0]->linkerFlags).toBe(std::unordered_set<std::string>{});
        expect(cache.toolchains()[0]->archiverFlags).toBe(std::unordered_set<std::string>{});
        expect(cache.toolchains()[0]->includePath).toBe(testProject.projectRoot() / "GCC/lib/gcc/x86_64-linux-gnu/9/include");
        expect(cache.toolchains()[0]->libPath).toBe(testProject.projectRoot() / "GCC/lib/gcc/x86_64-linux-gnu/9");
    });

    test("gcc 11", [] {
        const std::filesystem::path gccInstallDirectory = std::filesystem::current_path() / "abuild_toolchain_scanner_test/GCC";
        TestProjectWithContent testProject{"abuild_toolchain_scanner_test",
                                           {{".abuild", "{ \"settings\": { \"gccInstallDirectory\": \"" + gccInstallDirectory.generic_string() + "\", \"clangInstallDirectory\": \"\", \"msvcInstallDirectory\": \"\" } }"},
                                            {"GCC/bin/g++-11", ""},
                                            {"GCC/bin/ld", ""},
                                            {"GCC/bin/ar", ""},
                                            {"GCC/lib/gcc/x86_64-linux-gnu/11/include/emmintrin.h", ""},
                                            {"GCC/lib/gcc/x86_64-linux-gnu/11/somelib.lib", ""}}};

        abuild::BuildCache cache(testProject.projectRoot());
        abuild::ToolchainScanner{cache};

        assert_(cache.toolchains().size()).toBe(1u);
        expect(cache.toolchains()[0]->name).toBe("gcc11");
        expect(cache.toolchains()[0]->compiler).toBe(testProject.projectRoot() / "GCC/bin/g++-11");
        expect(cache.toolchains()[0]->linker).toBe(testProject.projectRoot() / "GCC/bin/ld");
        expect(cache.toolchains()[0]->archiver).toBe(testProject.projectRoot() / "GCC/bin/ar");
        expect(cache.toolchains()[0]->compilerFlags).toBe(std::unordered_set<std::string>{"-std=c++20", "-fmodules-ts", "-Wall", "-Wextra", "-pedantic", "-c", "-x c++"});
        expect(cache.toolchains()[0]->linkerFlags).toBe(std::unordered_set<std::string>{});
        expect(cache.toolchains()[0]->archiverFlags).toBe(std::unordered_set<std::string>{});
        expect(cache.toolchains()[0]->includePath).toBe(testProject.projectRoot() / "GCC/lib/gcc/x86_64-linux-gnu/11/include");
        expect(cache.toolchains()[0]->libPath).toBe(testProject.projectRoot() / "GCC/lib/gcc/x86_64-linux-gnu/11");
    });
});
