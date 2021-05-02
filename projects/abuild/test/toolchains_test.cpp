import abuild;
import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::Toolchains", [] {
    test("type traits", [] {
        expect(std::is_default_constructible_v<abuild::Toolchains>).toBe(false);
        expect(std::is_copy_constructible_v<abuild::Toolchains>).toBe(true);
        expect(std::is_nothrow_move_constructible_v<abuild::Toolchains>).toBe(true);
        expect(std::is_copy_assignable_v<abuild::Toolchains>).toBe(false);
        expect(std::is_nothrow_move_assignable_v<abuild::Toolchains>).toBe(false);
        expect(std::is_nothrow_destructible_v<abuild::Toolchains>).toBe(true);
    });

    test("type", [] {
        TestCache testCache;

        abuild::BuildCache cache{testCache.file()};
        abuild::ToolchainScanner{cache};
        abuild::Toolchains toolchains{cache};

        for (auto toolchain = toolchains.toolchains().MemberBegin(); toolchain != toolchains.toolchains().MemberEnd(); ++toolchain)
        {
            assert_(toolchain->value.HasMember("type")).toBe(true);
            expect(std::vector<std::string>{"msvc", "clang", "gcc"}).toMatch<OneOfMatcher>(toolchain->value["type"].GetString());
        }
    });

    test("compiler", [] {
        TestCache testCache;

        abuild::BuildCache cache{testCache.file()};
        abuild::ToolchainScanner{cache};
        abuild::Toolchains toolchains{cache};

        for (auto toolchain = toolchains.toolchains().MemberBegin(); toolchain != toolchains.toolchains().MemberEnd(); ++toolchain)
        {
            assert_(toolchain->value.HasMember("compiler")).toBe(true);
            expect(toolchain->value["compiler"].IsString()).toBe(true);
        }
    });

    test("compiler_flags", [] {
        TestCache testCache;

        abuild::BuildCache cache{testCache.file()};
        abuild::ToolchainScanner{cache};
        abuild::Toolchains toolchains{cache};

        for (auto toolchain = toolchains.toolchains().MemberBegin(); toolchain != toolchains.toolchains().MemberEnd(); ++toolchain)
        {
            assert_(toolchain->value.HasMember("type")).toBe(true);
            assert_(toolchain->value.HasMember("compiler_flags")).toBe(true);
            assert_(toolchain->value["compiler_flags"].IsArray()).toBe(true);

            const std::string type = toolchain->value["type"].GetString();
            const std::vector<std::string> flags = asVector(toolchain->value["compiler_flags"]);

            if (type == "msvc")
            {
                expect(flags).toBe(std::vector<std::string>{"/nologo",
                                                            "/std:c++latest",
                                                            "/W4",
                                                            "/WX",
                                                            "/EHsc"});
            }
            else if (type == "clang")
            {
                expect(flags).toBe(std::vector<std::string>{"-std=c++20",
                                                            "-Wall",
                                                            "-Wextra",
                                                            "-pedantic",
                                                            "-Werror",
                                                            "-fmodules"});
            }
            else if (type == "gcc")
            {
                expect(flags).toBe(std::vector<std::string>{"-std=c++20",
                                                            "-Wall",
                                                            "-Wextra",
                                                            "-pedantic",
                                                            "-Werror"});
            }
        }
    });

    test("linker", [] {
        TestCache testCache;

        abuild::BuildCache cache{testCache.file()};
        abuild::ToolchainScanner{cache};
        abuild::Toolchains toolchains{cache};

        for (auto toolchain = toolchains.toolchains().MemberBegin(); toolchain != toolchains.toolchains().MemberEnd(); ++toolchain)
        {
            assert_(toolchain->value.HasMember("linker")).toBe(true);
            expect(toolchain->value["linker"].IsString()).toBe(true);
        }
    });

    test("linker_flags", [] {
        TestCache testCache;

        abuild::BuildCache cache{testCache.file()};
        abuild::ToolchainScanner{cache};
        abuild::Toolchains toolchains{cache};

        for (auto toolchain = toolchains.toolchains().MemberBegin(); toolchain != toolchains.toolchains().MemberEnd(); ++toolchain)
        {
            assert_(toolchain->value.HasMember("type")).toBe(true);
            assert_(toolchain->value.HasMember("linker_flags")).toBe(true);
            assert_(toolchain->value["linker_flags"].IsArray()).toBe(true);

            const std::string type = toolchain->value["type"].GetString();
            const std::vector<std::string> flags = asVector(toolchain->value["linker_flags"]);

            if (type == "msvc")
            {
                expect(flags).toBe(std::vector<std::string>{"/WX"});
            }
            else if (type == "clang")
            {
                expect(flags).toBe(std::vector<std::string>{});
            }
            else if (type == "gcc")
            {
                expect(flags).toBe(std::vector<std::string>{});
            }
        }
    });

    test("archiver", [] {
        TestCache testCache;

        abuild::BuildCache cache{testCache.file()};
        abuild::ToolchainScanner{cache};
        abuild::Toolchains toolchains{cache};

        for (auto toolchain = toolchains.toolchains().MemberBegin(); toolchain != toolchains.toolchains().MemberEnd(); ++toolchain)
        {
            assert_(toolchain->value.HasMember("archiver")).toBe(true);
            expect(toolchain->value["archiver"].IsString()).toBe(true);
        }
    });

    test("archiver_flags", [] {
        TestCache testCache;

        abuild::BuildCache cache{testCache.file()};
        abuild::ToolchainScanner{cache};
        abuild::Toolchains toolchains{cache};

        for (auto toolchain = toolchains.toolchains().MemberBegin(); toolchain != toolchains.toolchains().MemberEnd(); ++toolchain)
        {
            assert_(toolchain->value.HasMember("type")).toBe(true);
            assert_(toolchain->value.HasMember("archiver_flags")).toBe(true);
            assert_(toolchain->value["archiver_flags"].IsArray()).toBe(true);

            const std::string type = toolchain->value["type"].GetString();
            const std::vector<std::string> flags = asVector(toolchain->value["archiver_flags"]);

            if (type == "msvc")
            {
                expect(flags).toBe(std::vector<std::string>{"/NOLOGO"});
            }
            else if (type == "clang")
            {
                expect(flags).toBe(std::vector<std::string>{});
            }
            else if (type == "gcc")
            {
                expect(flags).toBe(std::vector<std::string>{});
            }
        }
    });

    test("ifc", [] {
        TestCache testCache;

        abuild::BuildCache cache{testCache.file()};
        abuild::ToolchainScanner{cache};
        abuild::Toolchains toolchains{cache};

        for (auto toolchain = toolchains.toolchains().MemberBegin(); toolchain != toolchains.toolchains().MemberEnd(); ++toolchain)
        {
            assert_(toolchain->value.HasMember("ifc")).toBe(true);
            expect(toolchain->value["ifc"].IsString()).toBe(true);
        }
    });

    test("include", [] {
        TestCache testCache;

        abuild::BuildCache cache{testCache.file()};
        abuild::ToolchainScanner{cache};
        abuild::Toolchains toolchains{cache};

        for (auto toolchain = toolchains.toolchains().MemberBegin(); toolchain != toolchains.toolchains().MemberEnd(); ++toolchain)
        {
            assert_(toolchain->value.HasMember("include")).toBe(true);
            expect(toolchain->value["include"].IsString()).toBe(true);
        }
    });

    test("lib", [] {
        TestCache testCache;

        abuild::BuildCache cache{testCache.file()};
        abuild::ToolchainScanner{cache};
        abuild::Toolchains toolchains{cache};

        for (auto toolchain = toolchains.toolchains().MemberBegin(); toolchain != toolchains.toolchains().MemberEnd(); ++toolchain)
        {
            assert_(toolchain->value.HasMember("lib")).toBe(true);
            expect(toolchain->value["lib"].IsString()).toBe(true);
        }
    });
});
