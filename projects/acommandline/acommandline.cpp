export module acommandline;

export import astl;

#ifndef __clang__
import : option_data;
#else
// clang-format off
#include "option_data.cpp"
// clang-format on
#endif
