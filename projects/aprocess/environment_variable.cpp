#ifndef __clang__
export module aprocess : environment_variable;
#endif

export import astl;

namespace aprocess
{
//! EnvironmentVariable
export struct EnvironmentVariable // NOLINT(bugprone-exception-escape)
{
    //! Name of the environment variable
    std::string name;

    //! Value of environment variable
    std::string value;
};
}
