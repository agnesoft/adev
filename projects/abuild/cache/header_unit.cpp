#ifndef __clang__
export module abuild.cache:header_unit;
export import :file;
#endif

namespace abuild
{
export struct HeaderFile;

//! The `HeaderUnit` is precompiled header in a
//! module format.
export struct HeaderUnit
{
    HeaderFile *headerFile = nullptr;

    //! Precompiled header unit file.
    File precompiledHeaderUnit;
};
}
