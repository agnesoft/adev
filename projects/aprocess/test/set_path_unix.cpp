module;

#ifndef _WIN32
#    include <stdlib.h>
#endif

export module aprocesstest : set_path_unix;

import astl;

#ifndef _WIN32
export auto set_path(const char *app) -> void
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
