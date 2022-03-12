export module abuild.scanners;

#ifndef __clang__
export import : project_scanner;
#else
export import abuild.cache;
import athreadpool;
// clang-format off
#include "project_scanner.cpp" //NOLINT(bugprone-suspicious-include)
// clang-format on
#endif
