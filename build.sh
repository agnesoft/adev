#USER DEFINED
CLANG=clang++-11
LIBCXX_PREFIX="/home/mv/llvm-project/BUILD"

#BUILD SCRIPT
PROJECTS_ROOT="$(pwd)/projects"

mkdir -p build
cd build
mkdir -p bin

ASTL_INCLUDE_PATH="$PROJECTS_ROOT/astl/include"
CPP_FLAGS="-std=c++20 -Wall -Wextra -pedantic -Werror -nostdinc++ -fmodules -fprebuilt-module-path=. -I$ASTL_INCLUDE_PATH -I$LIBCXX_PREFIX/include/c++/v1"
CPP_AND_LINK_FLAGS="$CPP_FLAGS -L$LIBCXX_PREFIX/lib -lc++ -Wl,-rpath,$LIBCXX_PREFIX/lib"

#astl_test
"$CLANG" $CPP_AND_LINK_FLAGS "$PROJECTS_ROOT/astl/test/main.cpp" -o bin/astl_test

#atest
"$CLANG" $CPP_FLAGS -Xclang -emit-module-interface -c -x c++ "$PROJECTS_ROOT/atest/atest.ixx" -o atest.pcm
"$CLANG" $CPP_FLAGS -c -x c++ "$PROJECTS_ROOT/atest/atest.ixx" -o atest.obj

#atest_test
"$CLANG" $CPP_AND_LINK_FLAGS "$PROJECTS_ROOT/atest/test/atest_test.cpp" "$PROJECTS_ROOT/atest/test/expect_tobe_test.cpp" "$PROJECTS_ROOT/atest/test/expect_tothrow_test.cpp" "$PROJECTS_ROOT/atest/test/expect_tomatch_test.cpp" "$PROJECTS_ROOT/atest/test/printer_test.cpp" atest.obj -o bin/atest_test

cd ..
