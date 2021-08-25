source "sh/common_build.sh" $1

PROJECT_DIR="projects/astl"
BUILD_DIR="$BUILD_ROOT/astl"

MSVC="
if not exist \"$BUILD_DIR\" mkdir \"$BUILD_DIR\" >nul
if not exist \"$IFC_DIR\" mkdir \"$IFC_DIR\" >nul

cl.exe $MSVC_COMPILER_FLAGS ^
       /exportHeader ^
       /ifcOutput \"$BUILD_DIR/astl.hpp.ifc\" ^
       /Fo\"$BUILD_DIR/astl.hpp.obj\" ^
       /c ^
       /TP ^
       \"$PROJECT_DIR/astl.hpp\"

cl.exe $MSVC_COMPILER_FLAGS ^
       /interface ^
       /ifcSearchDir \"$BUILD_DIR\" ^
       /headerUnit \"projects/astl/astl.hpp=$BUILD_DIR/astl.hpp.ifc\" ^
       /ifcOutput \"$IFC_DIR/astl.ifc\" ^
       /Fo\"$BUILD_DIR/astl.obj\" ^
       /c ^
       /TP ^
       \"$PROJECT_DIR/astl.cpp\"

lib.exe /NOLOGO ^
        /OUT:\"$IFC_DIR/astl.lib\" ^
        \"$BUILD_DIR/astl.hpp.obj\" ^
        \"$BUILD_DIR/astl.obj\"
"

CLANG="
mkdir -p \"$BUILD_DIR\"
mkdir -p \"$PCM_DIR\"

$CLANG $CLANG_COMPILER_FLAGS \
       -Xclang -emit-module-interface \
       -o \"$PCM_DIR/astl.pcm\" \
       -c \"$PROJECT_DIR/astl.cpp\"

$CLANG $CLANG_COMPILER_FLAGS \
       -o \"$BUILD_DIR/astl.obj\" \
       -c \"$PROJECT_DIR/astl.cpp\"

ar r \"$PCM_DIR/astl.lib\" \
     \"$BUILD_DIR/astl.obj\"
"

build "atest"
