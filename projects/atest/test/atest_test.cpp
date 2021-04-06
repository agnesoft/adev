import atest;

auto main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) -> int
{
    atest::test("Global test", [] {
    });

    constexpr int EXPECTED_FAILURES = 11;
    const int failures = atest::TestRunner{argc, argv}.run();

    if (failures == EXPECTED_FAILURES)
    {
        return 0;
    }
    else
    {
        return failures;
    }
}
