export module aprocess;

#ifndef __clang__
export import :process;
#else
import awinapi;
#    ifndef _WIN32
import fcntl_h;
import unistd_h;
import wait_h;
import poll_h;
#    endif
// clang-format off
#include "environment_variable.cpp" //NOLINT(bugprone-suspicious-include)
#include "pipe.cpp" //NOLINT(bugprone-suspicious-include)
#include "process_setup.cpp" //NOLINT(bugprone-suspicious-include)
#include "unix_process.cpp" //NOLINT(bugprone-suspicious-include)
#include "windows_process.cpp" //NOLINT(bugprone-suspicious-include)
#include "process.cpp" //NOLINT(bugprone-suspicious-include)
// clang-format on
#endif
