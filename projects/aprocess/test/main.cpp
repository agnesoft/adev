import atest;
import aprocess;

auto set_path([[maybe_unused]] const char *command) -> void
{
#ifndef _WIN32
    std::string path = std::filesystem::canonical(std::filesystem::path{command}.parent_path()).string();
    path += ':';
    path += std::getenv("PATH");
    ::setenv("PATH", path.data(), 1);
#endif
}

auto main(int argc, char *argv[]) -> int
{
    ::set_path(argv[0]);
    return ::atest::TestRunner{}.run(argc, argv);
}
