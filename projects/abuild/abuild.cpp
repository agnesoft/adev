export module abuild;

export import acore;

#ifdef _MSC_VER
export import : file;
export import : cpp_file;
export import : project;

export import : build_cache;
export import : toolchain_scanner;
export import : project_scanner;
export import : tokenizer;
export import : code_scanner;
export import : dependency_scanner;
#else
// clang-format off
import "rapidjson.hpp";
#include "file.cpp"
#include "cpp_file.cpp"
#include "project.cpp"
#include "build_cache.cpp"
#include "toolchain_scanner.cpp"
#include "project_scanner.cpp"
#include "tokenizer.cpp"
#include "code_scanner.cpp"
#include "dependency_scanner.cpp"
// clang-format on
#endif
