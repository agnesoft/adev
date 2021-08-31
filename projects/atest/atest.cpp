export module atest;

export import astl;

#ifndef __clang__
import : failed_assertion;
import : test;
#else
// clang-format off
#include "failed_assertion.cpp"
#include "failure.cpp"
#include "test.cpp"
// clang-format on
#endif
