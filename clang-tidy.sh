./build.sh

#acore
clang-tidy-11 projects/acore/acore.cpp

#acore_test
clang-tidy-11 projects/acore/test/main.cpp

#astl_test
clang-tidy-11 projects/astl/test/main.cpp

#atest
clang-tidy-11 projects/atest/atest.cpp

#atest_test
clang-tidy-11 projects/atest/test/main.cpp
clang-tidy-11 projects/atest/test/bad_test_suite.cpp
clang-tidy-11 projects/atest/test/expect_tobe_test.cpp
clang-tidy-11 projects/atest/test/expect_tomatch_test.cpp
clang-tidy-11 projects/atest/test/expect_tothrow_test.cpp
clang-tidy-11 projects/atest/test/printer_test.cpp
