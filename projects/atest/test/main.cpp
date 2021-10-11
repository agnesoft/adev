import atest;

using ::atest::test;

auto main(int argc, char *argv[]) -> int
{
    test("Global test", [] {
    });

    constexpr int expectedFailures = 16;
    const int failures = ::atest::TestRunner{}.run(argc, argv);

    if (failures == expectedFailures)
    {
        return 0;
    }

    return failures;
}
