import atest;

auto main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) -> int
{
    //std::cout << "\x1b[31mHI\x1b[0m";
    return atest::TestRunner{argc, argv}.run();
}
