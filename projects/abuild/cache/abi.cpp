#ifndef __clang__
export module abuild.cache:abi;
export import astl;
#endif

namespace abuild
{
//! The `ABI` represent the compiler toolchain
//! application binary interface.
export struct ABI
{
    //! Processor architecture.
    enum class Architecture
    {
        //! Intel x86
        x86,

        //! ARM
        ARM
    };

    //! Platform or operating system.
    enum class Platform
    {
        //! Linux
        Linux,

        //! Unix
        Unix,

        //! Windows
        Windows
    };

    //! Architecture's register size.
    enum class Bitness
    {
        //! 8 bits
        x8,

        //! 16 bits
        x16,

        //! 32 bits
        x32,

        //! 64 bits
        x64
    };

    //! Processor architecture
    Architecture architecture;

    //! Target platform
    Platform platform;

    //! Register size
    Bitness bitness;
};
}
