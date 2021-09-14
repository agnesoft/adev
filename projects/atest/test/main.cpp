import atest;

using ::atest::test;

auto main(int argc, char *argv[]) -> int
{
    test("Global test", [] {
    });

    constexpr int EXPECTED_FAILURES = 16;
    const int failures = ::atest::TestRunner{argc, argv}.run();

    if (failures == EXPECTED_FAILURES)
    {
        return 0;
    }

    return failures;
}
