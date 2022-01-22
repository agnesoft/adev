export module abuild.cache;

#ifndef __clang__
export import : file;
#else
export import astl;
// clang-format off
#include "file.cpp" //NOLINT(bugprone-suspicious-include)
// clang-format on
#endif
