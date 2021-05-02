export module abuild;

export import acore;

#ifdef _MSC_VER
export import : build_cache;
export import : settings;
export import : toolchains;
export import : projects;
export import : dependencies;

#else
// clang-format off
import "rapidjson.hpp";
#include "build_cache.cpp"
#include "default_settings.cpp"
#include "settings.cpp"
#include "toolchain_scanner.cpp"
#include "toolchains.cpp"
#include "project_scanner.cpp"
#include "projects.cpp"
#include "tokenizer.cpp"
#include "code_scanner.cpp"
// clang-format on
#endif
