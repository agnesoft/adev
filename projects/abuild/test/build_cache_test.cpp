import atest;
import abuild;
import abuild_test_cache;

using atest::expect;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::BuildCache", [] {
    test("type traits", [] {
        expect(std::is_default_constructible_v<abuild::BuildCache>).toBe(false);
        expect(std::is_copy_constructible_v<abuild::BuildCache>).toBe(false);
        expect(std::is_nothrow_move_constructible_v<abuild::BuildCache>).toBe(true);
        expect(std::is_copy_assignable_v<abuild::BuildCache>).toBe(false);
        expect(std::is_nothrow_move_assignable_v<abuild::BuildCache>).toBe(true);
        expect(std::is_nothrow_destructible_v<abuild::BuildCache>).toBe(true);
    });

    test("missing cache", [] {
        TestCache testCache;

        expect(testCache.exists()).toBe(false);

        {
            abuild::BuildCache cache{testCache.file()};
        }

        expect(testCache.exists()).toBe(true);
        expect(testCache.content()).toBe(TestCache::jsonFormat("{}"));
    });

    test("existing cache", [] {
        TestCache testCache{"{ \"Value\": 1 }"};

        expect(testCache.exists()).toBe(true);

        {
            abuild::BuildCache cache{testCache.file()};
        }

        expect(testCache.content()).toBe(TestCache::jsonFormat("{ \"Value\": 1 }"));
    });
});
