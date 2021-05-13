import abuild;
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
        expect(std::is_copy_constructible_v<abuild::BuildCache>).toBe(true);

        // clang-format off
#if defined(_MSC_VER)
        bool isNothrowMoveConstructible = false;
#else
        bool isNothrowMoveConstructible = true;
#endif
        // clang-format on

        expect(std::is_nothrow_move_constructible_v<abuild::BuildCache>).toBe(isNothrowMoveConstructible);
        expect(std::is_copy_assignable_v<abuild::BuildCache>).toBe(true);
        expect(std::is_nothrow_move_assignable_v<abuild::BuildCache>).toBe(true);
        expect(std::is_nothrow_destructible_v<abuild::BuildCache>).toBe(true);
    });
});
