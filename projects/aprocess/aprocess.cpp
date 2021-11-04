export module aprocess;

#ifndef __clang__
export import : process;
#else
// clang-format off
#include "environment_variable.cpp"
#include "process_setup.cpp"
#include "async_reader.cpp"
#include "windows_process.cpp"
#include "process.cpp"
// clang-format on
#endif
