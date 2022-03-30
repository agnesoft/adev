#ifndef __clang__
export module abuild.cache : visibility;
#endif

namespace abuild
{
export enum class Visibility {
    Public,
    Private
};
}