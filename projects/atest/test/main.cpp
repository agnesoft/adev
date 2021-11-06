import atest;

using ::atest::test;

auto main(int argc, char *argv[]) -> int
{
    try
    {
        test("Global test", [] {
        });
    }
    catch (...)
    {
    }

    return ::atest::TestRunner{}.run(argc, argv);
}
