source "sh/common_build.sh" $1

PROJECT_DIR="projects/atest/test"
BUILD_DIR="$BUILD_ROOT/atest/test"

CLANG_BUILD="
mkdir -p \"$BUILD_DIR\"

$CLANG $CLANG_COMPILER_LINKER_FLAGS \
       -fprebuilt-module-path=$BUILD_ROOT/atest \
       -o \"$BIN_DIR/atest_test$EXECUTABLE_SUFFIX\" \
       \"$PROJECT_DIR/assert_test.cpp\" \
       \"$PROJECT_DIR/bad_test_suite.cpp\" \
       \"$PROJECT_DIR/expect_to_be_test.cpp\" \
       \"$PROJECT_DIR/expect_to_match_test.cpp\" \
       \"$PROJECT_DIR/expect_to_throw_test.cpp\" \
       \"$PROJECT_DIR/printer_test.cpp\" \
       \"$PROJECT_DIR/main.cpp\" \
       \"$BUILD_ROOT/atest/atest.obj\"
"

GCC_BUILD="
mkdir -p \"$BUILD_DIR\"

$GCC $GCC_COMPILER_FLAGS \
     -o \"$BIN_DIR/atest_test\" \
     \"$PROJECT_DIR/test/assert_test.cpp\" \
     \"$PROJECT_DIR/test/bad_test_suite.cpp\" \
     \"$PROJECT_DIR/test/expect_to_be_test.cpp\" \
     \"$PROJECT_DIR/test/expect_to_match_test.cpp\" \
     \"$PROJECT_DIR/test/expect_to_throw_test.cpp\" \
     \"$PROJECT_DIR/test/printer_test.cpp\" \
     \"$PROJECT_DIR/test/main.cpp\" \
     \"$CMI_DIR/atest.lib\" \
     \"$CMI_DIR/astl.lib\"
"

MSVC_BUILD="
if not exist \"$BUILD_DIR\" mkdir \"$BUILD_DIR\" >nul

cl.exe $MSVC_COMPILER_FLAGS ^
       /ifcSearchDir \"$BUILD_DIR\" ^
       /Fo\"$BUILD_DIR/\"
       /Fe\"$BIN_DIR/atest_test.exe\" ^
       \"$PROJECT_DIR/test/assert_test.cpp\" ^
       \"$PROJECT_DIR/test/bad_test_suite.cpp\" ^
       \"$PROJECT_DIR/test/expect_to_be_test.cpp\" ^
       \"$PROJECT_DIR/test/expect_to_match_test.cpp\" ^
       \"$PROJECT_DIR/test/expect_to_throw_test.cpp\" ^
       \"$PROJECT_DIR/test/printer_test.cpp\" ^
       \"$PROJECT_DIR/test/main.cpp\" ^
       \"$BUILD_DIR/atest.lib\" ^
       \"$BUILD_ROOT/astl/astl.lib\"
"
sh/build_atest.sh $1
build "atest_test"
