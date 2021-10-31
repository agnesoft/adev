export module aprocess;

#ifndef __clang__
export import : process;
#else
// clang-format off
#include "environment_variable.cpp"
#include "process_setup.cpp"
#include "windows/windows_h.cpp"
#include "windows/handle.cpp"
#include "windows/security_attributes.cpp"
#include "windows/pipe_windows.cpp"
#include "windows/startup_info.cpp"
#include "windows/process_info.cpp"
#include "process.cpp"
// clang-format on
#endif
