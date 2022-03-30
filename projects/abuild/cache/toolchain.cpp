#ifndef __clang__
export module abuild.cache : toolchain;
export import : abi;
#endif

namespace abuild
{
//! The `Toolchain` represents a compiler
//! toolchain.
export struct Toolchain
{
    //! Name of the toolchain.
    std::string name;

    //! Path to the C compiler.
    std::filesystem::path compilerC;

    //! Path to the C++ compiler.
    std::filesystem::path compilerCpp;

    //! Path to the linker.
    std::filesystem::path linker;

    //! Archiver for creating static libraries.
    std::filesystem::path archiver;

    //! Application binary interface of the code &
    //! binaries created by the toolchain.
    ABI abi;
};
}
