source "sh/build_common.sh" 

set_build_properties "atest.test" "${1}" "${2}"

function build_clang() {
    ${clang} ${clangCompilerLinkerFlags}                       \
             -fprebuilt-module-path=${buildRoot}/atest         \
             -o "${binDir}/${project}${executableExtension}"   \
             "${projectDir}/assert_test.cpp"                   \
             "${projectDir}/throwing_test_test.cpp"            \
             "${projectDir}/throwing_suites_test.cpp"          \
             "${projectDir}/expect_to_be_test.cpp"             \
             "${projectDir}/expect_to_be_string_test.cpp"      \
             "${projectDir}/expect_to_contain_test.cpp"        \
             "${projectDir}/expect_to_match_test.cpp"          \
             "${projectDir}/expect_to_throw_test.cpp"          \
             "${projectDir}/test_runner_test.cpp"              \
             "${projectDir}/test_filter_test_test.cpp"         \
             "${projectDir}/test_filter_suite_test.cpp"        \
             "${projectDir}/test_filter_filter_test_test.cpp"  \
             "${projectDir}/test_filter_filter_suite_test.cpp" \
             "${projectDir}/main.cpp"                          \
             "${buildRoot}/atest/atest.obj"                    \
             "${buildRoot}/acommandline/acommandline.obj"      \
             "${buildRoot}/astl/astl.obj"
}

function build_gcc() {
    ${gcc} ${gccCompilerFlags}                               \
           -o "${binDir}/${project}"                         \
           "${projectDir}/assert_test.cpp"                   \
           "${projectDir}/throwing_test_test.cpp"            \
           "${projectDir}/throwing_suites_test.cpp"          \
           "${projectDir}/expect_to_be_test.cpp"             \
           "${projectDir}/expect_to_be_string_test.cpp"      \
           "${projectDir}/expect_to_contain_test.cpp"        \
           "${projectDir}/expect_to_match_test.cpp"          \
           "${projectDir}/expect_to_throw_test.cpp"          \
           "${projectDir}/test_runner_test.cpp"              \
           "${projectDir}/test_filter_test_test.cpp"         \
           "${projectDir}/test_filter_suite_test.cpp"        \
           "${projectDir}/test_filter_filter_test_test.cpp"  \
           "${projectDir}/test_filter_filter_suite_test.cpp" \
           "${projectDir}/main.cpp"                          \
           "${buildRoot}/atest/atest.lib"                    \
           "${buildRoot}/acommandline/acommandline.lib"      \
           "${buildRoot}/astl/astl.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags}                                ^
       /ifcSearchDir \"${buildRoot}/atest\"                ^
       /Fo\"$buildDir/\"                                   ^
       /Fe\"${binDir}/${project}.exe\"                     ^
       \"${projectDir}/assert_test.cpp\"                   ^
       \"${projectDir}/throwing_test_test.cpp\"            ^
       \"${projectDir}/throwing_suites_test.cpp\"          ^
       \"${projectDir}/expect_to_be_test.cpp\"             ^
       \"${projectDir}/expect_to_be_string_test.cpp\"      ^
       \"${projectDir}/expect_to_contain_test.cpp\"        ^
       \"${projectDir}/expect_to_match_test.cpp\"          ^
       \"${projectDir}/expect_to_throw_test.cpp\"          ^
       \"${projectDir}/test_runner_test.cpp\"              ^
       \"${projectDir}/test_filter_test_test.cpp\"         ^
       \"${projectDir}/test_filter_suite_test.cpp\"        ^
       \"${projectDir}/test_filter_filter_test_test.cpp\"  ^
       \"${projectDir}/test_filter_filter_suite_test.cpp\" ^
       \"${projectDir}/main.cpp\"                          ^
       \"${buildRoot}/atest/atest.lib\"                    ^
       \"${buildRoot}/acommandline/acommandline.lib\"      ^
       \"${buildRoot}/astl/astl.lib\" || exit 1
"

build "atest"
