export module abuild;

#ifdef _MSC_VER
export import : settings;
export import : build_cache;
export import : project_scanner;
#else
// clang-format off
import <astl.hpp>;
#include "settings.cpp"
#include "project.cpp"
#include "file.cpp"
#include "build_cache.cpp"
#include "project_scanner.cpp"
// clang-format on
#endif
