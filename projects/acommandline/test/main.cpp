import atest;

auto main(int argc, char *argv[]) -> int
{
    return atest::TestRunner{argc, argv}.run();
}
