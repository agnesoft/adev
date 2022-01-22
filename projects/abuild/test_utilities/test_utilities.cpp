export module abuild.test_utilities;

#ifndef __clang__
export import : test_file;
#else
export import astl;
// clang-format off
#include "test_file.cpp" //NOLINT(bugprone-suspicious-include)
// clang-format on
#endif
