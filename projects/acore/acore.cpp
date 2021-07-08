export module acore;

#ifdef _MSC_VER
export import : commandline;
export import : process;
#else
// clang-format off
#include    "commandline_option.cpp"
#include    "commandline.cpp"
#include    "process_unix.cpp"
#include    "process.cpp"
// clang-format on
#endif
