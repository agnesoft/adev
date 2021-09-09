export module acommandline;

export import astl;

#ifndef __clang__
export import : command_line;
#else
// clang-format off
#include "command_line.cpp"
#include "command_line_option.cpp"
// clang-format on
#endif
