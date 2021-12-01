source "sh/build_common.sh" "${1}"

project="atest_test"
projectDir="projects/atest/test"
buildDir="${buildRoot}/atest/test"

function build_clang() {
    $clang $clangCompilerLinkerFlags \
           -fprebuilt-module-path=${buildRoot}/atest \
           -o "${binDir}/atest_test${executableExtension}" \
           "${projectDir}/assert_test.cpp" \
           "${projectDir}/throwing_test_test.cpp" \
           "${projectDir}/throwing_suites_test.cpp" \
           "${projectDir}/expect_to_be_test.cpp" \
           "${projectDir}/expect_to_be_string_test.cpp" \
           "${projectDir}/expect_to_contain_test.cpp" \
           "${projectDir}/expect_to_match_test.cpp" \
           "${projectDir}/expect_to_throw_test.cpp" \
           "${projectDir}/test_runner_test.cpp" \
           "${projectDir}/test_filter_test_test.cpp" \
           "${projectDir}/test_filter_suite_test.cpp" \
           "${projectDir}/test_filter_filter_test_test.cpp" \
           "${projectDir}/test_filter_filter_suite_test.cpp" \
           "${projectDir}/main.cpp" \
           "${buildRoot}/atest/atest.obj" \
           "${buildRoot}/acommandline/acommandline.obj" \
           "${buildRoot}/astl/astl.obj"
}

function build_gcc() {
    $gcc $gccCompilerFlags \
         -o "${binDir}/atest_test" \
         "${projectDir}/assert_test.cpp" \
         "${projectDir}/throwing_test_test.cpp" \
         "${projectDir}/throwing_suites_test.cpp" \
         "${projectDir}/expect_to_be_test.cpp" \
         "${projectDir}/expect_to_be_string_test.cpp" \
         "${projectDir}/expect_to_contain_test.cpp" \
         "${projectDir}/expect_to_match_test.cpp" \
         "${projectDir}/expect_to_throw_test.cpp" \
         "${projectDir}/test_runner_test.cpp" \
         "${projectDir}/test_filter_test_test.cpp" \
         "${projectDir}/test_filter_suite_test.cpp" \
         "${projectDir}/test_filter_filter_test_test.cpp" \
         "${projectDir}/test_filter_filter_suite_test.cpp" \
         "${projectDir}/main.cpp" \
         "${buildRoot}/atest/atest.lib" \
         "${buildRoot}/acommandline/acommandline.lib" \
         "${buildRoot}/astl/astl.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} ^
       /ifcSearchDir \"${buildRoot}/atest\" ^
       /Fo\"$buildDir/\" ^
       /Fe\"${binDir}/atest_test.exe\" ^
       \"${projectDir}/assert_test.cpp\" ^
       \"${projectDir}/throwing_test_test.cpp\" ^
       \"${projectDir}/throwing_suites_test.cpp\" ^
       \"${projectDir}/expect_to_be_test.cpp\" ^
       \"${projectDir}/expect_to_be_string_test.cpp\" ^
       \"${projectDir}/expect_to_contain_test.cpp\" ^
       \"${projectDir}/expect_to_match_test.cpp\" ^
       \"${projectDir}/expect_to_throw_test.cpp\" ^
       \"${projectDir}/test_runner_test.cpp\" ^
       \"${projectDir}/test_filter_test_test.cpp\" ^
       \"${projectDir}/test_filter_suite_test.cpp\" ^
       \"${projectDir}/test_filter_filter_test_test.cpp\" ^
       \"${projectDir}/test_filter_filter_suite_test.cpp\" ^
       \"${projectDir}/main.cpp\" ^
       \"${buildRoot}/atest/atest.lib\" ^
       \"${buildRoot}/acommandline/acommandline.lib\" ^
       \"${buildRoot}/astl/astl.lib\" || exit 1
"

sh/build/atest.sh "${toolchain}"
build
