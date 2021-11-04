import atest;

using ::atest::test;

auto main(int argc, char *argv[]) -> int
{
    test("Global test", [] {
    });

    return ::atest::TestRunner{}.run(argc, argv);
}
