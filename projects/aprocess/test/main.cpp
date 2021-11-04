import atest;
import aprocess;

using ::atest::test;

#ifdef _WIN32
import awinapi;

auto set_path(const char *app) -> void
{
    const std::string bin = std::filesystem::absolute(std::filesystem::path{app}.parent_path()).string();
    constexpr std::size_t BUFFER_SIZE = 4096;
    char path[BUFFER_SIZE]{};

    if (::GetEnvironmentVariable("PATH", path, BUFFER_SIZE) == FALSE)
    {
        std::cout << "Failed to retrieve environment variable PATH:\n  " << ::awinapi::last_error_message() << "\n";
    }

    if (::SetEnvironmentVariable("PATH", (bin + ';' + path).c_str()) == FALSE)
    {
        std::cout << "Failed to set environment variable PATH:\n  " << ::awinapi::last_error_message() << "\n";
    }
}
#else
#    include <stdlib.h>

auto set_path(const char *app) -> void
{
    const std::string bin = std::filesystem::absolute(std::filesystem::path{app}.parent_path()).string();
    const int error = ::setenv("PATH", (std::getenv("PATH") + ':' + bin).c_str(), 1);

    if (error == ENOMEM)
    {
        std::cout << "Failed to set PATH: Insufficient memory to add a new variable to the environment.\n";
    }
    else
    {
        std::cout << "Failed to set PATH: unknown error (" << error << ")\n";
    }
}
#endif

auto main(int argc, char *argv[]) -> int
{
    ::set_path(argv[0]);
    return ::atest::TestRunner{}.run(argc, argv);
}
