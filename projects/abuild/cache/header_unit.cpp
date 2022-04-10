#ifndef __clang__
export module abuild.cache:header_unit;
export import :file;
export import :header;
export import :visibility;
#endif

namespace abuild
{
export struct HeaderUnit : Header
{
    File precompiledHeaderUnit;
    Visibility visibility = Visibility::Private;
};
}
