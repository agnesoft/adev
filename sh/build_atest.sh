source "sh/common_build.sh" $1

PROJECT_DIR="projects/atest"
BUILD_DIR="$BUILD_ROOT/atest"

CLANG_BUILD="
mkdir -p \"$BUILD_DIR\"
$CLANG $CLANG_COMPILER_FLAGS \
       -Xclang -emit-module-interface \
       -o \"$PCM_DIR/atest.pcm\" \
       -c \"$PROJECT_DIR/atest.cpp\"

$CLANG $CLANG_COMPILER_FLAGS \
       -o \"$BUILD_DIR/atest.obj\" \
       -c \"$PROJECT_DIR/atest.cpp\"

$CLANG $CLANG_COMPILER_LINKER_FLAGS \
        -o \"$BIN_DIR/atest_test$EXECUTABLE_SUFFIX\" \
        \"$PROJECT_DIR/test/main.cpp\" \
        \"$BUILD_DIR/atest.obj\"
"

GCC_BUILD="
mkdir -p \"$BUILD_DIR\"

$GCC $GCC_COMPILER_FLAGS \
     -o \"$BUILD_DIR/atest.obj\" \
     -c \"$PROJECT_DIR/atest.cpp\"

ar r \"$CMI_DIR/atest.lib\" \
     \"$BUILD_DIR/atest.obj\"

$GCC $GCC_COMPILER_FLAGS \
     -o \"$BIN_DIR/atest_test\" \
     \"$PROJECT_DIR/test/main.cpp\" \
     \"$CMI_DIR/atest.lib\" \
     \"$CMI_DIR/astl.lib\"
"

MSVC_BUILD="
if not exist \"$BUILD_DIR/test\" mkdir \"$BUILD_DIR/test\" >nul

cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/atest-failed_assertion.ifc\" /Fo\"$BUILD_DIR/atest-failed_assertion.obj\" /c /TP \"$PROJECT_DIR/failed_assertion.cpp\"

cl.exe $MSVC_COMPILER_FLAGS ^
       /interface ^
       /ifcSearchDir \"$BUILD_DIR\" ^
       /ifcOutput\"$IFC_DIR/atest.ifc\" ^
       /Fo\"$BUILD_DIR/atest.obj\" ^
       /c \"$PROJECT_DIR/atest.cpp\"

lib.exe /NOLOGO ^
        /OUT:\"$IFC_DIR/atest.lib\" ^
        \"$BUILD_DIR/atest.obj\" ^
        \"$BUILD_DIR/atest-failed_assertion.obj\"

cl.exe $MSVC_COMPILER_FLAGS ^
       /Fo\"$BUILD_DIR/test/main.obj\" ^
       /c \"$PROJECT_DIR/test/main.cpp\"

link.exe /NOLOGO ^
         /OUT:\"$BIN_DIR/atest_test.exe\" ^
         \"$BUILD_DIR/test/main.obj\" ^
         \"$IFC_DIR/atest.lib\" ^
         \"$IFC_DIR/astl.lib\"
"

build "atest"
