#ifndef __clang__
export module abuild.cache : header_unit;
export import : file;
export imrpot : header;
#endif

namespace abuild
{
export struct Header;

export struct HeaderUnit : Header
{
    File precompiledHeaderUnit;
};
}
