import atest;
import aprocess;

auto set_path([[maybe_unused]] const char *command) -> void
{
#ifndef _WIN32
    std::string path = std::filesystem::canonical(std::filesystem::path{command}.parent_path()).string();
    path += ':';
    path += std::getenv("PATH"); //NOLINT(concurrency-mt-unsafe)
    ::setenv("PATH", path.data(), 1); //NOLINT(concurrency-mt-unsafe)
#endif
}

auto main(int argc, char *argv[]) -> int
{
    ::set_path(argv[0]); //NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return ::atest::TestRunner{}.run(argc, argv);
}
