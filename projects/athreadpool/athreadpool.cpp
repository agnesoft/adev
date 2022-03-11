export module athreadpool;

export import astl;

#ifndef __clang__
export import : threadpool;
#else
// clang-format off
#include "job.cpp" //NOLINT(bugprone-suspicious-include)
#include "jobs.cpp" //NOLINT(bugprone-suspicious-include)
#include "queue.cpp" //NOLINT(bugprone-suspicious-include)
#include "monitor.cpp" //NOLINT(bugprone-suspicious-include)
#include "threadpool.cpp" //NOLINT(bugprone-suspicious-include)
// clang-format on
#endif
