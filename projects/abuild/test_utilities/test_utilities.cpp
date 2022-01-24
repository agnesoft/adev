export module abuild.test_utilities;

#ifndef __clang__
export import : test_file;
export import : token_operators;
#else
export import abuild.cpptokenizer;
// clang-format off
#include "test_file.cpp" //NOLINT(bugprone-suspicious-include)
#include "token_operators.cpp" //NOLINT(bugprone-suspicious-include)
// clang-format on
#endif
