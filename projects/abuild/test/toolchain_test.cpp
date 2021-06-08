import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::Toolchain", [] {
    test("add toolchain", [] {
        abuild::BuildCache cache;

        abuild::Toolchain toolchain{
            .name = "msvc",
            .type = abuild::Toolchain::Type::MSVC,
            .compiler = "cl.exe",
            .compilerFlags = {"/nologo", "/std:c++latest"},
            .linker = "link.exe",
            .linkerFlags = {"/NOLOGO"},
            .archiver = "archiver.exe",
            .archiverFlags = {},
            .includePath = "include/"};

        cache.addToolchain(toolchain);

        expect(cache.toolchains().size()).toBe(1u);
    });

    test("lookup toolchain", [] {
        abuild::BuildCache cache;
        cache.addToolchain(abuild::Toolchain{.name = "gcc-11"});
        cache.addToolchain(abuild::Toolchain{.name = "clang-11"});

        const abuild::Toolchain *toolchain = cache.toolchain("clang-11");

        assert_(toolchain != nullptr).toBe(true);
        expect(toolchain->name).toBe("clang-11");
    });

    test("lookup toolchain by incomplete name", [] {
        abuild::BuildCache cache;
        cache.addToolchain(abuild::Toolchain{.name = "gcc-11"});
        cache.addToolchain(abuild::Toolchain{.name = "clang-11"});

        const abuild::Toolchain *toolchain = cache.toolchain("clang");

        assert_(toolchain != nullptr).toBe(true);
        expect(toolchain->name).toBe("clang-11");
    });

    test("lookup toolchain with multiple candidates", [] {
        abuild::BuildCache cache;
        cache.addToolchain(abuild::Toolchain{.name = "clang-11"});
        cache.addToolchain(abuild::Toolchain{.name = "clang-10"});
        cache.addToolchain(abuild::Toolchain{.name = "clang-9"});

        const abuild::Toolchain *toolchain = cache.toolchain("clang");

        assert_(toolchain != nullptr).toBe(true);
        expect(toolchain->name).toBe("clang-11");
    });
});
