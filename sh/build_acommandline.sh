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

$GCC $GCC_COMPILER_FLAGS -o \"$BUILD_DIR/option_data.obj\" -c \"$PROJECT_DIR/option_data.cpp\"
$GCC $GCC_COMPILER_FLAGS -o \"$BUILD_DIR/option_builder_base.obj\" -c \"$PROJECT_DIR/option_builder_base.cpp\"
$GCC $GCC_COMPILER_FLAGS -o \"$BUILD_DIR/option_builder_described.obj\" -c \"$PROJECT_DIR/option_builder_described.cpp\"
$GCC $GCC_COMPILER_FLAGS -o \"$BUILD_DIR/option_builder_defined.obj\" -c \"$PROJECT_DIR/option_builder_defined.cpp\"
$GCC $GCC_COMPILER_FLAGS -o \"$BUILD_DIR/option_builder_value.obj\" -c \"$PROJECT_DIR/option_builder_value.cpp\"
$GCC $GCC_COMPILER_FLAGS -o \"$BUILD_DIR/option_builder_named.obj\" -c \"$PROJECT_DIR/option_builder_named.cpp\"
$GCC $GCC_COMPILER_FLAGS -o \"$BUILD_DIR/option_builder.obj\" -c \"$PROJECT_DIR/option_builder.cpp\"
$GCC $GCC_COMPILER_FLAGS -o \"$BUILD_DIR/option_matcher.obj\" -c \"$PROJECT_DIR/option_matcher.cpp\"

$GCC $GCC_COMPILER_FLAGS \
     -o \"$BUILD_DIR/acommandline.obj\" \
     -c \"$PROJECT_DIR/acommandline.cpp\"

ar r \"$BUILD_DIR/acommandline.lib\" \
     \"$BUILD_DIR/acommandline.obj\" \
     \"$BUILD_DIR/option_data.obj\" \
     \"$BUILD_DIR/option_builder_base.obj\" \
     \"$BUILD_DIR/option_builder_described.obj\" \
     \"$BUILD_DIR/option_builder_defined.obj\" \
     \"$BUILD_DIR/option_builder_value.obj\" \
     \"$BUILD_DIR/option_builder_named.obj\" \
     \"$BUILD_DIR/option_builder.obj\" \
     \"$BUILD_DIR/option_matcher.obj\"
"

MSVC_BUILD="
if not exist \"$BUILD_DIR\" mkdir \"$BUILD_DIR\" >nul

cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/acommandline-option_data.ifc\" /Fo\"$BUILD_DIR/acommandline-option_data.obj\" /c /TP \"$PROJECT_DIR/option_data.cpp\"
cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/acommandline-option_builder_base.ifc\" /Fo\"$BUILD_DIR/acommandline-option_builder_base.obj\" /c /TP \"$PROJECT_DIR/option_builder_base.cpp\"
cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/acommandline-option_builder_described.ifc\" /Fo\"$BUILD_DIR/acommandline-option_builder_described.obj\" /c /TP \"$PROJECT_DIR/option_builder_described.cpp\"
cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/acommandline-option_builder_defined.ifc\" /Fo\"$BUILD_DIR/acommandline-option_builder_defined.obj\" /c /TP \"$PROJECT_DIR/option_builder_defined.cpp\"
cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/acommandline-option_builder_value.ifc\" /Fo\"$BUILD_DIR/acommandline-option_builder_value.obj\" /c /TP \"$PROJECT_DIR/option_builder_value.cpp\"
cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/acommandline-option_builder_named.ifc\" /Fo\"$BUILD_DIR/acommandline-option_builder_named.obj\" /c /TP \"$PROJECT_DIR/option_builder_named.cpp\"
cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/acommandline-option_builder.ifc\" /Fo\"$BUILD_DIR/acommandline-option_builder.obj\" /c /TP \"$PROJECT_DIR/option_builder.cpp\"
cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/acommandline-option_matcher.ifc\" /Fo\"$BUILD_DIR/acommandline-option_matcher.obj\" /c /TP \"$PROJECT_DIR/option_matcher.cpp\"

cl.exe $MSVC_COMPILER_FLAGS ^
       /interface ^
       /ifcSearchDir \"$BUILD_DIR\" ^
       /ifcOutput\"$BUILD_DIR/acommandline.ifc\" ^
       /Fo\"$BUILD_DIR/acommandline.obj\" ^
       /c \"$PROJECT_DIR/acommandline.cpp\"

lib.exe /NOLOGO ^
        /OUT:\"$BUILD_DIR/acommandline.lib\" ^
        \"$BUILD_DIR/acommandline.obj\" ^
        \"$BUILD_DIR/acommandline-option_data.obj\" ^
        \"$BUILD_DIR/acommandline-option_builder_base.obj\" ^
        \"$BUILD_DIR/acommandline-option_builder_described.obj\" ^
        \"$BUILD_DIR/acommandline-option_builder_defined.obj\" ^
        \"$BUILD_DIR/acommandline-option_builder_value.obj\" ^
        \"$BUILD_DIR/acommandline-option_builder_named.obj\" ^
        \"$BUILD_DIR/acommandline-option_builder.obj\" ^
        \"$BUILD_DIR/acommandline-option_matcher.obj\"
"

sh/build_astl.sh $1
build "acommandline"
