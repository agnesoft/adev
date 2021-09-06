export module atest;

export import astl;

#ifndef __clang__
export import : test_runner;
import : expect;
#else
// clang-format off
#include "stringify.cpp"
#include "failed_assertion.cpp"
#include "failure.cpp"
#include "test.cpp"
#include "test_suite.cpp"
#include "global_tests.cpp"
#include "matcher_base.cpp"
#include "matcher.cpp"
#include "expect_base.cpp"
#include "expect_to_match.cpp"
#include "expect_to_throw.cpp"
#include "expect.cpp"
#include "report.cpp"
#include "reporter.cpp"
#include "printer.cpp"
#include "test_runner.cpp"
// clang-format on
#endif
