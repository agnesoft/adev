#ifndef __clang__
export module abuild.cache:header_unit;
export import :file;
export import :header;
#endif

namespace abuild
{
//! The `HeaderUnit` is precompiled header in a
//! module format.
export struct HeaderUnit
{
    CppFile *cppFile = nullptr;

    //! Precompiled header unit file.
    File precompiledHeaderUnit;
};
}
