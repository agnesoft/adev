import abuild;
import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::Header", [] {
    test("type traits", [] {
        expect(std::is_default_constructible_v<abuild::Header>).toBe(false);
        expect(std::is_copy_constructible_v<abuild::Header>).toBe(true);
        expect(std::is_nothrow_move_constructible_v<abuild::Header>).toBe(true);
        expect(std::is_copy_assignable_v<abuild::Header>).toBe(true);
        expect(std::is_nothrow_move_assignable_v<abuild::Header>).toBe(true);
        expect(std::is_nothrow_destructible_v<abuild::Header>).toBe(true);
    });
});
