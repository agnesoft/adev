export module awinapi;

#ifndef __clang__
export import : last_error_message;
export import : handle;
#else
export import "windows.hpp";
export import astl;
// clang-format off
#include "last_error_message.cpp" //NOLINT(bugprone-suspicious-include)
#include "handle.cpp" //NOLINT(bugprone-suspicious-include)
// clang-format on
#endif
