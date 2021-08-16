source "sh/common_build.sh" $1

PROJECT_DIR="projects/atest"
BUILD_DIR="$BUILD_ROOT/atest"

MSVC="
if not exist \"$BUILD_DIR/test\" mkdir \"$BUILD_DIR/test\" >nul
cl.exe $MSVC_COMPILER_FLAGS /interface /ifcOutput\"$BUILD_DIR/atest.ifc\" /Fo\"$BUILD_DIR/atest.obj\" /c \"$PROJECT_DIR/atest.cpp\"
lib.exe /NOLOGO ^
        /OUT:\"$BUILD_DIR/atest.lib\" ^
        $BUILD_DIR/atest.obj

cl.exe $MSVC_COMPILER_FLAGS /Fo\"$BUILD_DIR/test/main.obj\" /c \"$PROJECT_DIR/test/main.cpp\"
link.exe /NOLOGO ^
         /OUT:\"$BIN_DIR/atest_test.exe\" ^
         \"$BUILD_DIR/test/main.obj\" ^
         \"$BUILD_DIR/atest.lib\"
"

CLANG="
mkdir -p \"$BUILD_DIR\"
$CLANG $CLANG_COMPILER_FLAGS -Xclang -emit-module-interface -o \"$BUILD_DIR/atest.pcm\" -c \"$PROJECT_DIR/atest.cpp\"
$CLANG $CLANG_COMPILER_FLAGS -o \"$BUILD_DIR/atest.obj\" -c \"$PROJECT_DIR/atest.cpp\"

$CLANG $CLANG_COMPILER_AND_LINKER_FLAGS \
        -fprebuilt-module-path=\"$BUILD_DIR\" \
        -o \"$BIN_DIR/atest_test$EXECUTABLE_SUFFIX\" \
        \"$PROJECT_DIR/test/main.cpp\" \
        \"$BUILD_DIR/atest.obj\"
"

build

