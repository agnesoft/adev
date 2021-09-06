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

$GCC $GCC_COMPILER_FLAGS -o \"$BUILD_DIR/stringify.obj\" -c \"$PROJECT_DIR/stringify.cpp\"
$GCC $GCC_COMPILER_FLAGS -o \"$BUILD_DIR/failed_assertion.obj\" -c \"$PROJECT_DIR/failed_assertion.cpp\"
$GCC $GCC_COMPILER_FLAGS -o \"$BUILD_DIR/failure.obj\" -c \"$PROJECT_DIR/failure.cpp\"
$GCC $GCC_COMPILER_FLAGS -o \"$BUILD_DIR/test.obj\" -c \"$PROJECT_DIR/test.cpp\"
$GCC $GCC_COMPILER_FLAGS -o \"$BUILD_DIR/test_suite.obj\" -c \"$PROJECT_DIR/test_suite.cpp\"
$GCC $GCC_COMPILER_FLAGS -o \"$BUILD_DIR/global_tests.obj\" -c \"$PROJECT_DIR/global_tests.cpp\"
$GCC $GCC_COMPILER_FLAGS -o \"$BUILD_DIR/matcher_base.obj\" -c \"$PROJECT_DIR/matcher_base.cpp\"
$GCC $GCC_COMPILER_FLAGS -o \"$BUILD_DIR/matcher.obj\" -c \"$PROJECT_DIR/matcher.cpp\"
$GCC $GCC_COMPILER_FLAGS -o \"$BUILD_DIR/expect_base.obj\" -c \"$PROJECT_DIR/expect_base.cpp\"
$GCC $GCC_COMPILER_FLAGS -o \"$BUILD_DIR/expect_to_match.obj\" -c \"$PROJECT_DIR/expect_to_match.cpp\"
$GCC $GCC_COMPILER_FLAGS -o \"$BUILD_DIR/expect_to_throw.obj\" -c \"$PROJECT_DIR/expect_to_throw.cpp\"
$GCC $GCC_COMPILER_FLAGS -o \"$BUILD_DIR/expect.obj\" -c \"$PROJECT_DIR/expect.cpp\"
$GCC $GCC_COMPILER_FLAGS -o \"$BUILD_DIR/printer.obj\" -c \"$PROJECT_DIR/printer.cpp\"
$GCC $GCC_COMPILER_FLAGS -o \"$BUILD_DIR/report.obj\" -c \"$PROJECT_DIR/report.cpp\"
$GCC $GCC_COMPILER_FLAGS -o \"$BUILD_DIR/reporter.obj\" -c \"$PROJECT_DIR/reporter.cpp\"
$GCC $GCC_COMPILER_FLAGS -o \"$BUILD_DIR/test_runner.obj\" -c \"$PROJECT_DIR/test_runner.cpp\"

$GCC $GCC_COMPILER_FLAGS \
     -o \"$BUILD_DIR/atest.obj\" \
     -c \"$PROJECT_DIR/atest.cpp\"

ar r \"$CMI_DIR/atest.lib\" \
     \"$BUILD_DIR/atest.obj\" \
     \"$BUILD_DIR/stringify.obj\" \
     \"$BUILD_DIR/failed_assertion.obj\" \
     \"$BUILD_DIR/failure.obj\" \
     \"$BUILD_DIR/test.obj\" \
     \"$BUILD_DIR/test_suite.obj\" \
     \"$BUILD_DIR/global_tests.obj\" \
     \"$BUILD_DIR/matcher_base.obj\" \
     \"$BUILD_DIR/matcher.obj\" \
     \"$BUILD_DIR/expect_base.obj\" \
     \"$BUILD_DIR/expect_to_match.obj\" \
     \"$BUILD_DIR/expect_to_throw.obj\" \
     \"$BUILD_DIR/expect.obj\" \
     \"$BUILD_DIR/report.obj\" \
     \"$BUILD_DIR/reporter.obj\" \
     \"$BUILD_DIR/printer.obj\" \
     \"$BUILD_DIR/test_runner.obj\"

$GCC $GCC_COMPILER_FLAGS \
     -o \"$BIN_DIR/atest_test\" \
     \"$PROJECT_DIR/test/main.cpp\" \
     \"$CMI_DIR/atest.lib\" \
     \"$CMI_DIR/astl.lib\"
"

MSVC_BUILD="
if not exist \"$BUILD_DIR/test\" mkdir \"$BUILD_DIR/test\" >nul

cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/atest-stringify.ifc\" /Fo\"$BUILD_DIR/atest-stringify.obj\" /c /TP \"$PROJECT_DIR/stringify.cpp\"
cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/atest-failed_assertion.ifc\" /Fo\"$BUILD_DIR/atest-failed_assertion.obj\" /c /TP \"$PROJECT_DIR/failed_assertion.cpp\"
cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/atest-failure.ifc\" /Fo\"$BUILD_DIR/atest-failure.obj\" /c /TP \"$PROJECT_DIR/failure.cpp\"
cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/atest-test.ifc\" /Fo\"$BUILD_DIR/atest-test.obj\" /c /TP \"$PROJECT_DIR/test.cpp\"
cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/atest-test_suite.ifc\" /Fo\"$BUILD_DIR/atest-test_suite.obj\" /c /TP \"$PROJECT_DIR/test_suite.cpp\"
cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/atest-global_tests.ifc\" /Fo\"$BUILD_DIR/atest-global_tests.obj\" /c /TP \"$PROJECT_DIR/global_tests.cpp\"
cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/atest-matcher_base.ifc\" /Fo\"$BUILD_DIR/atest-matcher_base.obj\" /c /TP \"$PROJECT_DIR/matcher_base.cpp\"
cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/atest-matcher.ifc\" /Fo\"$BUILD_DIR/atest-matcher.obj\" /c /TP \"$PROJECT_DIR/matcher.cpp\"
cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/atest-expect_base.ifc\" /Fo\"$BUILD_DIR/atest-expect_base.obj\" /c /TP \"$PROJECT_DIR/expect_base.cpp\"
cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/atest-expect_to_match.ifc\" /Fo\"$BUILD_DIR/atest-expect_to_match.obj\" /c /TP \"$PROJECT_DIR/expect_to_match.cpp\"
cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/atest-expect_to_throw.ifc\" /Fo\"$BUILD_DIR/atest-expect_to_throw.obj\" /c /TP \"$PROJECT_DIR/expect_to_throw.cpp\"
cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/atest-expect.ifc\" /Fo\"$BUILD_DIR/atest-expect.obj\" /c /TP \"$PROJECT_DIR/expect.cpp\"
cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/atest-report.ifc\" /Fo\"$BUILD_DIR/atest-report.obj\" /c /TP \"$PROJECT_DIR/report.cpp\"
cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/atest-reporter.ifc\" /Fo\"$BUILD_DIR/atest-reporter.obj\" /c /TP \"$PROJECT_DIR/reporter.cpp\"
cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/atest-printer.ifc\" /Fo\"$BUILD_DIR/atest-printer.obj\" /c /TP \"$PROJECT_DIR/printer.cpp\"
cl.exe $MSVC_COMPILER_FLAGS /internalPartition /ifcSearchDir \"$BUILD_DIR\" /ifcOutput\"$BUILD_DIR/atest-test_runner.ifc\" /Fo\"$BUILD_DIR/atest-test_runner.obj\" /c /TP \"$PROJECT_DIR/test_runner.cpp\"

cl.exe $MSVC_COMPILER_FLAGS ^
       /interface ^
       /ifcSearchDir \"$BUILD_DIR\" ^
       /ifcOutput\"$IFC_DIR/atest.ifc\" ^
       /Fo\"$BUILD_DIR/atest.obj\" ^
       /c \"$PROJECT_DIR/atest.cpp\"

lib.exe /NOLOGO ^
        /OUT:\"$IFC_DIR/atest.lib\" ^
        \"$BUILD_DIR/atest.obj\" ^
        \"$BUILD_DIR/atest-stringify.obj\" ^
        \"$BUILD_DIR/atest-failed_assertion.obj\" ^
        \"$BUILD_DIR/atest-failure.obj\" ^
        \"$BUILD_DIR/atest-test.obj\" ^
        \"$BUILD_DIR/atest-test_suite.obj\" ^
        \"$BUILD_DIR/atest-global_tests.obj\" ^
        \"$BUILD_DIR/atest-matcher_base.obj\" ^
        \"$BUILD_DIR/atest-matcher.obj\" ^
        \"$BUILD_DIR/atest-expect_base.obj\" ^
        \"$BUILD_DIR/atest-expect_to_match.obj\" ^
        \"$BUILD_DIR/atest-expect_to_throw.obj\" ^
        \"$BUILD_DIR/atest-expect.obj\" ^
        \"$BUILD_DIR/atest-report.obj\" ^
        \"$BUILD_DIR/atest-reporter.obj\" ^
        \"$BUILD_DIR/atest-printer.obj\" ^
        \"$BUILD_DIR/atest-test_runner.obj\"

cl.exe $MSVC_COMPILER_FLAGS ^
       /ifcSearchDir \"$BUILD_DIR\" ^
       /Fo\"$BUILD_DIR/test/main.obj\" ^
       /c \"$PROJECT_DIR/test/main.cpp\"

link.exe /NOLOGO ^
         /OUT:\"$BIN_DIR/atest_test.exe\" ^
         \"$BUILD_DIR/test/main.obj\" ^
         \"$IFC_DIR/atest.lib\" ^
         \"$IFC_DIR/astl.lib\"
"

sh/build_astl.sh $1
build "atest"
