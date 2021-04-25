export module abuild;

export import acore;

#ifdef _MSC_VER
export import : build_cache;
export import : environment_scanner;
#else
import "rapidjson.hpp";
#    include "build_cache.cpp"
#    include "environment_scanner.cpp"
#endif

namespace abuild
{
}
