import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::Project", [] {
    test("type traits", [] {
        expect(std::is_default_constructible_v<abuild::Project>).toBe(false);
        expect(std::is_copy_constructible_v<abuild::Project>).toBe(true);
        expect(std::is_nothrow_move_constructible_v<abuild::Project>).toBe(true);
        expect(std::is_copy_assignable_v<abuild::Project>).toBe(true);
        expect(std::is_nothrow_move_assignable_v<abuild::Project>).toBe(true);
        expect(std::is_nothrow_destructible_v<abuild::Project>).toBe(true);
    });

    test("name", [] {
        const abuild::Project project{"myproject"};

        assert_(noexcept(project.name())).toBe(true);
        expect(project.name()).toBe("myproject");
    });
});
