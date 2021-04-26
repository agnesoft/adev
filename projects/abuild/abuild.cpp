export module abuild;

export import acore;

#ifdef _MSC_VER
export import : build_cache;
export import : settings;
export import : toolchains;
export import : projects;

#else
// clang-format off
import "rapidjson.hpp";
#    include "build_cache.cpp"
#    include "settings.cpp"
#    include "toolchains.cpp"
#    include "projects.cpp"
// clang-format on
#endif
