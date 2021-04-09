#USER DEFINED
CLANG=clang++-11
LIBCXX_PREFIX="/home/mv/llvm-project/BUILD"

#BUILD SCRIPT
PROJECTS_ROOT="$(pwd)/projects"

mkdir -p build
cd build
mkdir -p bin

BUILD_ROOT=$(pwd)
ASTL_INCLUDE_PATH="$PROJECTS_ROOT/astl/include"
CPP_FLAGS="-std=c++20 $1 -Wall -Wextra -pedantic -Wno-missing-field-initializers -Werror -nostdinc++ -fmodules -fprebuilt-module-path=$BUILD_ROOT/acore -fprebuilt-module-path=$BUILD_ROOT/atest -I$ASTL_INCLUDE_PATH -I$LIBCXX_PREFIX/include/c++/v1"
CPP_AND_LINK_FLAGS="$CPP_FLAGS -L$LIBCXX_PREFIX/lib -lc++ -Wl,-rpath,$LIBCXX_PREFIX/lib"

#acore
mkdir -p acore
cd acore
"$CLANG" $CPP_FLAGS -Xclang -emit-module-interface -c "$PROJECTS_ROOT/acore/acore.cpp" -o acore.pcm
"$CLANG" $CPP_FLAGS -c "$PROJECTS_ROOT/acore/acore.cpp" -o acore.obj
cd ..

#atest
mkdir -p atest
cd atest
"$CLANG" $CPP_FLAGS -Xclang -emit-module-interface -c "$PROJECTS_ROOT/atest/atest.cpp" -o atest.pcm
"$CLANG" $CPP_FLAGS -c "$PROJECTS_ROOT/atest/atest.cpp" -o atest.obj
cd ..

#atest_test
mkdir -p atest_test
cd atest_test
"$CLANG" $CPP_AND_LINK_FLAGS "$PROJECTS_ROOT/atest/test/main.cpp" "$PROJECTS_ROOT/atest/test/expect_tobe_test.cpp" "$PROJECTS_ROOT/atest/test/expect_tothrow_test.cpp" "$PROJECTS_ROOT/atest/test/expect_tomatch_test.cpp" "$PROJECTS_ROOT/atest/test/printer_test.cpp" "$PROJECTS_ROOT/atest/test/bad_test_suite.cpp" "$BUILD_ROOT/atest/atest.obj" -o "$BUILD_ROOT/bin/atest_test"
cd ..

#astl_test
"$CLANG" $CPP_AND_LINK_FLAGS "$PROJECTS_ROOT/astl/test/main.cpp" -o "$BUILD_ROOT/bin/astl_test"

mkdir -p acore_test
cd acore_test
"$CLANG" $CPP_AND_LINK_FLAGS "$PROJECTS_ROOT/acore/test/main.cpp" "$BUILD_ROOT/acore/acore.obj" "$BUILD_ROOT/atest/atest.obj" -o "$BUILD_ROOT/bin/acore_test"
cd ..

cd ..
