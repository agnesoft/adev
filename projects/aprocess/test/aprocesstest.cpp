export module aprocesstest;

#ifndef __clang__
export import : set_path_unix;
export import : set_path_windows;
#else
// clang-format off
#include "set_path_unix.cpp"
#include "set_path_windows.cpp"
// clang-format on
#endif
