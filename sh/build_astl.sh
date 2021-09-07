source "sh/common_build.sh" $1

PROJECT_DIR="projects/astl"
BUILD_DIR="$BUILD_ROOT/astl"

CLANG_BUILD="
mkdir -p \"$BUILD_DIR\"

$CLANG $CLANG_COMPILER_FLAGS \
       -Xclang -emit-module-interface \
       -o \"$BUILD_DIR/astl.pcm\" \
       -c \"$PROJECT_DIR/astl.cpp\"

$CLANG $CLANG_COMPILER_FLAGS \
       -o \"$BUILD_DIR/astl.obj\" \
       -c \"$PROJECT_DIR/astl.cpp\"

ar r \"$BUILD_DIR/astl.lib\" \
     \"$BUILD_DIR/astl.obj\"
"

GCC_BUILD="
mkdir -p \"$BUILD_DIR\"

$GCC $GCC_COMPILER_FLAGS \
     -x c++-header \
     \"$PROJECT_DIR/astl.hpp\"

$GCC $GCC_COMPILER_FLAGS \
     -o \"$BUILD_DIR/astl.obj\" \
     -c \"$PROJECT_DIR/astl.cpp\"

ar r \"$BUILD_DIR/astl.lib\" \
     \"$BUILD_DIR/astl.obj\"
"

MSVC_BUILD="
if not exist \"$BUILD_DIR\" mkdir \"$BUILD_DIR\" >nul

cl.exe $MSVC_COMPILER_FLAGS ^
       /exportHeader ^
       /ifcOutput \"$BUILD_DIR/astl.hpp.ifc\" ^
       /Fo\"$BUILD_DIR/astl.hpp.obj\" ^
       /c ^
       /TP ^
       \"$PROJECT_DIR/astl.hpp\"

cl.exe $MSVC_COMPILER_FLAGS ^
       /interface ^
       /ifcOutput \"$BUILD_DIR/astl.ifc\" ^
       /Fo\"$BUILD_DIR/astl.obj\" ^
       /c ^
       /TP ^
       \"$PROJECT_DIR/astl.cpp\"

lib.exe /NOLOGO ^
        /OUT:\"$BUILD_DIR/astl.lib\" ^
        \"$BUILD_DIR/astl.hpp.obj\" ^
        \"$BUILD_DIR/astl.obj\"
"

build "astl"
