export module atest;

export import astl;

#ifndef __clang__
import : expect_to_match;
import : expect_to_throw;
import : matcher;
import : reporter;
import : printer;
#else
// clang-format off
#include "stringify.cpp"
#include "failed_assertion.cpp"
#include "failure.cpp"
#include "test.cpp"
#include "test_suite.cpp"
#include "global_tests.cpp"
#include "expect_base.cpp"
#include "expect_to_match.cpp"
#include "expect_to_throw.cpp"
#include "matcher_base.cpp"
#include "matcher.cpp"
#include "report.cpp"
#include "reporter.cpp"
#include "printer.cpp"
// clang-format on
#endif
