export module abuild;

#ifdef _MSC_VER
export import : settings;
export import : build_cache;
export import : project_scanner;
export import : code_scanner;
export import : dependency_scanner;
export import : build_graph;
export import : toolchain_scanner;
#else
// clang-format off
export import <astl.hpp>;
#include "settings.cpp"
#include "project.cpp"
#include "dependency.cpp"
#include "file.cpp"
#include "header.cpp"
#include "source.cpp"
#include "module.cpp"
#include "error.cpp"
#include "warning.cpp"
#include "build_task.cpp"
#include "build_cache_index.cpp"
#include "toolchain.cpp"
#include "build_cache.cpp"
#include "project_scanner.cpp"
#include "token.cpp"
#include "tokenizer.cpp"
#include "code_scanner.cpp"
#include "dependency_scanner.cpp"
#include "build_graph.cpp"
#include "toolchain_scanner.cpp"
// clang-format on
#endif
