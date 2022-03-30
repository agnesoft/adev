#ifndef __clang__
export module abuild.cache : flag;
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

    //! If set to `true` the flag affects the ABI
    //! meaning that mixing binaries with
    //! different setting of this flag can be
    //! incompatible resulting in crashes,
    //! corruption etc. For example optimization
    //! (e.g. `-O, /O`) flags or debugging flags
    //! (e.g. `-g`) affect ABI in this way.
    bool affectsABI = false;
};
}
