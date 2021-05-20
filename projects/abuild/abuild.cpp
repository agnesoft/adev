export module abuild;

#ifdef _MSC_VER
export import : settings;
export import : build_cache;
export import : project_scanner;

export import : tokenizer;
#else
// clang-format off
import <astl.hpp>;
#include "settings.cpp"
#include "project.cpp"
#include "dependency.cpp"
#include "file.cpp"
#include "header.cpp"
#include "source.cpp"
#include "module.cpp"
#include "build_cache.cpp"
#include "project_scanner.cpp"
#include "tokenize.cpp"
// clang-format on
#endif
