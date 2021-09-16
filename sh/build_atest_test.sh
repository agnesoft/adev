source "sh/common_build.sh" $1

project="atest_test"
projectDir="projects/atest/test"
buildDir="$buildRoot/atest/test"

clangBuild="
$clang $clangCompilerLinkerFlags \
       -fprebuilt-module-path=$buildRoot/atest \
       -o \"$binDir/atest_test$executableSuffix\" \
       \"$projectDir/assert_test.cpp\" \
       \"$projectDir/bad_test_suite.cpp\" \
       \"$projectDir/expect_to_be_test.cpp\" \
       \"$projectDir/expect_to_match_test.cpp\" \
       \"$projectDir/expect_to_throw_test.cpp\" \
       \"$projectDir/printer_test.cpp\" \
       \"$projectDir/main.cpp\" \
       \"$buildRoot/atest/atest.obj\"
"

gccBuild="
$gcc $gccCompilerFlags \
     -o \"$binDir/atest_test\" \
     \"$projectDir/assert_test.cpp\" \
     \"$projectDir/bad_test_suite.cpp\" \
     \"$projectDir/expect_to_be_test.cpp\" \
     \"$projectDir/expect_to_match_test.cpp\" \
     \"$projectDir/expect_to_throw_test.cpp\" \
     \"$projectDir/printer_test.cpp\" \
     \"$projectDir/main.cpp\" \
     \"$buildRoot/atest/atest.lib\" \
     \"$buildRoot/astl/astl.lib\"
"

msvcBuild="
cl.exe $msvcCompilerFlags ^
       /ifcSearchDir \"$buildRoot/atest\" ^
       /Fo\"$buildDir/\" ^
       /Fe\"$binDir/atest_test.exe\" ^
       \"$projectDir/assert_test.cpp\" ^
       \"$projectDir/bad_test_suite.cpp\" ^
       \"$projectDir/expect_to_be_test.cpp\" ^
       \"$projectDir/expect_to_match_test.cpp\" ^
       \"$projectDir/expect_to_throw_test.cpp\" ^
       \"$projectDir/printer_test.cpp\" ^
       \"$projectDir/main.cpp\" ^
       \"$buildRoot/atest/atest.lib\" ^
       \"$buildRoot/astl/astl.lib\"
"
sh/build_atest.sh $toolchain
build
