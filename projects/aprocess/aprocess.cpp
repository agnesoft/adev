export module aprocess;

#ifndef __clang__
export import : process;
#else
import awinapi;
// clang-format off
#include "environment_variable.cpp" //NOLINT(bugprone-suspicious-include)
#include "process_setup.cpp" //NOLINT(bugprone-suspicious-include)
#include "async_reader.cpp" //NOLINT(bugprone-suspicious-include)
#include "windows_process.cpp" //NOLINT(bugprone-suspicious-include)
#include "process.cpp" //NOLINT(bugprone-suspicious-include)
// clang-format on
#endif
