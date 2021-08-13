source "sh/common_build.sh"

PROJECT_DIR="projects/atest"
BUILD_DIR="$BUILD_ROOT/atest"

MSVC="
mkdir \"$BUILD_DIR/test\"
cl.exe $MSVC_COMPILER_FLAGS /interface /ifcOutput\"$BUILD_DIR/atest.ifc\" /Fo\"$BUILD_DIR/atest.obj\" \"$PROJECT_DIR/atest.cpp\"
lib.exe /NOLOGO ^
        /OUT:\"$BUILD_DIR/atest.lib\" ^
        $BUILD_DIR/atest.obj

cl.exe $MSVC_COMPILER_FLAGS /ifcSearchDir\"$BUILD_DIR\" /Fo\"$BUILD_DIR/test/main.obj\" \"$PROJECT_DIR/test/main.cpp\"
link.exe /NOLOGO ^
         /OUT:\"$BIN_DIR/atest_test.exe\" ^
         \"$BUILD_DIR/test/main.obj\" ^
         \"$BUILD_DIR/atest.lib\"
"

if isWindows; then
    buildWindows "$MSVC"
else
    echo "TODO"
fi

exit $STATUS
