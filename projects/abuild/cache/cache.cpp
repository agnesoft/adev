export module abuild.cache;

#ifndef __clang__
export import : source_file_base;
#else
export import abuild.cpptokenizer;
// clang-format off
#include "file.cpp" //NOLINT(bugprone-suspicious-include)
#include "source_file_base.cpp" //NOLINT(bugprone-suspicious-include)
// clang-format on
#endif
