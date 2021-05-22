import atest;

auto main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) -> int
{
    return atest::TestRunner{argc, argv}.run();
}
