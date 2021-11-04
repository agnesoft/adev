export module awinapi;

#ifndef __clang__
export import : pipe;
#else
export import "windows.hpp";
export import astl;
// clang-format off
#include "buffer.cpp" //NOLINT(bugprone-suspicious-include)
#include "last_error_message.cpp" //NOLINT(bugprone-suspicious-include)
#include "handle.cpp" //NOLINT(bugprone-suspicious-include)
#include "pipe.cpp" //NOLINT(bugprone-suspicious-include)
// clang-format on
#endif
