import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::SDK", [] {
    test("add SDK", [] {
        abuild::BuildCache cache;
        cache.addSDK(abuild::SDK{.name = "MySDK"});

        assert_(cache.sdks().size()).toBe(1u);
        expect(cache.sdks()[0]->name).toBe("MySDK");
    });
});
