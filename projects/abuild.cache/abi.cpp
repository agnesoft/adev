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
        //! ARM
        ARM,

        //! Intel x86
        X86
    };

    //! Architecture's register size.
    enum class Bitness
    {
        //! 32 bits
        X32,

        //! 64 bits
        X64
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

    //! Processor architecture
    Architecture architecture = Architecture::X86;

    //! Register size
    Bitness bitness = Bitness::X64;

    //! Target platform
    Platform platform = Platform::Linux;
};
}
