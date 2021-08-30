export module atest;

export import astl;

#ifndef __clang__
import : failed_assertion;
import : failure;
#else
// clang-format off
#include "failed_assertion.cpp"
#include "failure.cpp"
// clang-format on
#endif
