#ifndef __clang__
export module abuild.cache : header_unit;
export import : file;
#endif

namespace abuild
{
export struct Header;

export struct HeaderUnit
{
    Header *file = nullptr;
    File precompiledHeaderUnit;
};
}
