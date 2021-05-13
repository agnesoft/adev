export module abuild;

#ifdef _MSC_VER
export import : settings;
export import : build_cache;
#else
// clang-format off
import <astl.hpp>;
#include "settings.cpp"
#include "project.cpp"
#include "build_cache.cpp"
// clang-format on
#endif
