source "sh/common_build.sh" $1

PROJECT_DIR="projects/acommandline"
BUILD_DIR="$BUILD_ROOT/acommandline"

CLANG_BUILD="
mkdir -p \"$BUILD_DIR\"

$CLANG $CLANG_COMPILER_FLAGS \
       -Xclang -emit-module-interface \
       -o \"$BUILD_DIR/acommandline.pcm\" \
       -c \"$PROJECT_DIR/acommandline.cpp\"

$CLANG $CLANG_COMPILER_FLAGS \
       -o \"$BUILD_DIR/acommandline.obj\" \
       -c \"$PROJECT_DIR/acommandline.cpp\"
"

GCC_BUILD="
mkdir -p \"$BUILD_DIR\"

$GCC $GCC_COMPILER_FLAGS -o \"$BUILD_DIR/command_line_option.obj\" -c \"$PROJECT_DIR/command_line_option.cpp\"
$GCC $GCC_COMPILER_FLAGS -o \"$BUILD_DIR/command_line.obj\" -c \"$PROJECT_DIR/command_line.cpp\"

$GCC $GCC_COMPILER_FLAGS \
     -o \"$BUILD_DIR/acommandline.obj\" \
     -c \"$PROJECT_DIR/acommandline.cpp\"

ar r \"$CMI_DIR/acommandline.lib\" \
     \"$BUILD_DIR/acommandline.obj\" \
     \"$BUILD_DIR/command_line.obj\" \
     \"$BUILD_DIR/command_line_option.obj\"
"

MSVC_BUILD="
if not exist \"$BUILD_DIR\" mkdir \"$BUILD_DIR\" >nul

cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/acommandline-command_line_option.ifc\" /Fo\"$BUILD_DIR/acommandline-command_line_option.obj\" /c /TP \"$PROJECT_DIR/command_line_option.cpp\"
cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/acommandline-command_line.ifc\" /Fo\"$BUILD_DIR/acommandline-command_line.obj\" /c /TP \"$PROJECT_DIR/command_line.cpp\"

cl.exe $MSVC_COMPILER_FLAGS ^
       /interface ^
       /ifcSearchDir \"$BUILD_DIR\" ^
       /ifcOutput\"$BUILD_DIR/acommandline.ifc\" ^
       /Fo\"$BUILD_DIR/acommandline.obj\" ^
       /c \"$PROJECT_DIR/acommandline.cpp\"

lib.exe /NOLOGO ^
        /OUT:\"$BUILD_DIR/acommandline.lib\" ^
        \"$BUILD_DIR/acommandline.obj\" ^
        \"$BUILD_DIR/acommandline-command_line_option.obj\" ^
        \"$BUILD_DIR/acommandline-command_line.obj\"
"

sh/build_astl.sh $1
build "acommandline"
