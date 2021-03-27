#USER DEFINED
CLANG=clang++-11
LIBCXX_PREFIX="/home/mv/llvm-project/BUILD"

#BUILD SCRIPT
PROJECTS_ROOT="$(pwd)/projects"

mkdir -p build_clang
cd build_clang
mkdir -p bin

ASTL_INCLUDE_PATH="$PROJECTS_ROOT/astl/include"
CPP_FLAGS="-std=c++20 -nostdinc++ -fmodules -I$ASTL_INCLUDE_PATH -I$LIBCXX_PREFIX/include/c++/v1 -L$LIBCXX_PREFIX/lib"
CPP_AND_LINK_FLAGS="$CPP_FLAGS -lc++ -Wl,-rpath,$LIBCXX_PREFIX/lib"

#astl_test
"$CLANG" $CPP_AND_LINK_FLAGS "$PROJECTS_ROOT/astl/test/main.cpp" -o bin/astl_test

cd ..
