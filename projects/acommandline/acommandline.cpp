export module acommandline;

export import astl;

#ifndef __clang__
import : option_builder_base;
#else
// clang-format off
#include "option_data.cpp"
#include "option_builder_base.cpp"
// clang-format on
#endif
