source "sh/common_build.sh" $1

project="atest_test"
projectDir="projects/atest/test"
buildDir="${buildRoot}/atest/test"

function build_clang() {
    $clang $clangCompilerLinkerFlags \
           -fprebuilt-module-path=${buildRoot}/atest \
           -o "${binDir}/atest_test$executableSuffix" \
           "${projectDir}/assert_test.cpp" \
           "${projectDir}/bad_test_suite.cpp" \
           "${projectDir}/expect_to_be_test.cpp" \
           "${projectDir}/expect_to_match_test.cpp" \
           "${projectDir}/expect_to_throw_test.cpp" \
           "${projectDir}/printer_test.cpp" \
           "${projectDir}/main.cpp" \
           "${buildRoot}/atest/atest.obj" \
           "${buildRoot}/astl/astl.obj"
}

function build_gcc() {
    $gcc $gccCompilerFlags \
         -o "${binDir}/atest_test" \
         "${projectDir}/assert_test.cpp" \
         "${projectDir}/bad_test_suite.cpp" \
         "${projectDir}/expect_to_be_test.cpp" \
         "${projectDir}/expect_to_match_test.cpp" \
         "${projectDir}/expect_to_throw_test.cpp" \
         "${projectDir}/printer_test.cpp" \
         "${projectDir}/main.cpp" \
         "${buildRoot}/atest/atest.lib" \
         "${buildRoot}/astl/astl.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} ^
       /ifcSearchDir \"${buildRoot}/atest\" ^
       /Fo\"$buildDir/\" ^
       /Fe\"${binDir}/atest_test.exe\" ^
       \"${projectDir}/assert_test.cpp\" ^
       \"${projectDir}/bad_test_suite.cpp\" ^
       \"${projectDir}/expect_to_be_test.cpp\" ^
       \"${projectDir}/expect_to_match_test.cpp\" ^
       \"${projectDir}/expect_to_throw_test.cpp\" ^
       \"${projectDir}/printer_test.cpp\" ^
       \"${projectDir}/main.cpp\" ^
       \"${buildRoot}/atest/atest.lib\" ^
       \"${buildRoot}/astl/astl.lib\" || exit 1
"

sh/build_atest.sh $toolchain
build
