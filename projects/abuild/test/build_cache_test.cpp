import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::BuildCache", [] {
    test("type traits", [] {
        expect(std::is_default_constructible_v<abuild::BuildCache>).toBe(true);
        expect(std::is_copy_constructible_v<abuild::BuildCache>).toBe(false);

        // clang-format off
#ifdef _WIN32
        bool isNothrowMoveConstructible = false;
#else
        bool isNothrowMoveConstructible = true;
#endif
        // clang-format on

        expect(std::is_nothrow_move_constructible_v<abuild::BuildCache>).toBe(isNothrowMoveConstructible);
        expect(std::is_copy_assignable_v<abuild::BuildCache>).toBe(false);
        expect(std::is_nothrow_move_assignable_v<abuild::BuildCache>).toBe(true);
        expect(std::is_nothrow_destructible_v<abuild::BuildCache>).toBe(true);
    });

    test("Error", [] {
        abuild::BuildCache cache;
        cache.addError(abuild::Error{.component = "BuildCache", .what = "Some error"});

        assert_(cache.errors().size()).toBe(1u);
        expect(cache.errors()[0].component).toBe("BuildCache");
        expect(cache.errors()[0].what).toBe("Some error");
    });

    test("Warning", [] {
        abuild::BuildCache cache;
        cache.addWarning(abuild::Warning{.component = "BuildCache", .what = "Some warning"});

        assert_(cache.warnings().size()).toBe(1u);
        expect(cache.warnings()[0].component).toBe("BuildCache");
        expect(cache.warnings()[0].what).toBe("Some warning");
    });

    test("project root", [] {
        TestProject testProject{"abuild_build_cache_test", {}};

        const abuild::BuildCache cache{testProject.projectRoot()};

        expect(cache.projectRoot()).toBe(testProject.projectRoot());
    });
});
