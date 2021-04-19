#ifndef _MSC_VER
module;

#    include <unistd.h>
#endif

export module acore;

#ifdef _MSC_VER
export import : acore_common;
export import : commandline;
export import : process;
#else
// clang-format off
#include    "acore_common.cpp"
#include    "commandline_option.cpp"
#include    "commandline.cpp"
#include    "process_unix.cpp"
#include    "process.cpp"
// clang-format on
#endif
