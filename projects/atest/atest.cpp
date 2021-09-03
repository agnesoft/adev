export module atest;

export import astl;

#ifndef __clang__
import : failed_assertion;
import : global_tests;
#else
// clang-format off
#include "failed_assertion.cpp"
#include "failure.cpp"
#include "test.cpp"
#include "test_suite.cpp"
#include "global_tests.cpp"
// clang-format on
#endif
