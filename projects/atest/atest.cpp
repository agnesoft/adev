export module atest;

export import astl;

#ifndef __clang__
import : failed_assertion;
#else
// clang-format off
#include "failed_assertion.cpp"
// clang-format on
#endif
