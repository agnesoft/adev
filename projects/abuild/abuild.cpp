export module abuild;

export import acore;

#ifdef _MSC_VER
export import : build_cache;
#else
#    include "build_cache.cpp"
#endif

namespace abuild
{
}
