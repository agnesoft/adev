export module atest;

export import astl;

#ifndef __clang__
import : stringify;
import : expect_base;
#else
// clang-format off
#include "stringify.cpp"
#include "failed_assertion.cpp"
#include "failure.cpp"
#include "test.cpp"
#include "test_suite.cpp"
#include "global_tests.cpp"
#include "expect_base.cpp"
// clang-format on
#endif
