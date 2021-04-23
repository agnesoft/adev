export module abuild;

export import acore;

#ifdef _MSC_VER
export import : build_cache;
#else
import "rapidjson.hpp";
#    include "build_cache.cpp"
#endif

namespace abuild
{
}
