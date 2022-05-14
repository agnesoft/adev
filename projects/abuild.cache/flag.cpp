#ifndef __clang__
export module abuild.cache:flag;
export import astl;
#endif

namespace abuild
{
//! Represents a toolchain (e.g. compiler, linker
//! etc.) flag.
export struct Flag
{
    //! Name of the flag.
    std::string name;

    //! Value of the flag.
    std::string value;
};
}
