export module acommandline;

export import astl;

#ifndef __clang__
import : option_builder_value;
#else
// clang-format off
#include "option_data.cpp"
#include "option_builder_base.cpp"
#include "option_builder_described.cpp"
#include "option_builder_defined.cpp"
#include "option_builder_value.cpp"
// clang-format on
#endif
