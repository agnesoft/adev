export module acore;

#ifdef _MSC_VER
export import : acore_common;
export import : commandline;
#else
// clang-format off
#include    "acore_common.cpp"
#include    "commandline_option.cpp"
#include    "commandline_option_test.cpp"
// clang-format on
#endif
