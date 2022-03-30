#ifndef __clang__
export module abuild.cache : header_unit;
export import : file;
export import : header;
#endif

namespace abuild
{
export struct HeaderUnit : Header
{
    File precompiledHeaderUnit;
};
}
