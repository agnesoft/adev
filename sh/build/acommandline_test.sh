source "sh/build_common.sh" "${1}"

project="acommandline_test"
projectDir="projects/acommandline/test"
buildDir="${buildRoot}/acommandline/test"

function build_clang() {
    $clang $clangCompilerLinkerFlags \
           -fprebuilt-module-path=${buildRoot}/atest \
           -fprebuilt-module-path=${buildRoot}/acommandline \
           -o "${binDir}/acommandline_test$executableSuffix" \
           "${projectDir}/application_name_test.cpp" \
           "${projectDir}/command_line_test.cpp" \
           "${projectDir}/command_test.cpp" \
           "${projectDir}/examples.cpp" \
           "${projectDir}/default_value_test.cpp" \
           "${projectDir}/help_test.cpp" \
           "${projectDir}/long_name_test.cpp" \
           "${projectDir}/main.cpp" \
           "${projectDir}/parse_test.cpp" \
           "${projectDir}/positional_test.cpp" \
           "${projectDir}/repeated_test.cpp" \
           "${projectDir}/required_test.cpp" \
           "${projectDir}/short_name_test.cpp" \
           "${buildRoot}/acommandline/acommandline.obj" \
           "${buildRoot}/atest/atest.obj" \
           "${buildRoot}/astl/astl.obj"
}

function build_gcc() {
    $gcc $gccCompilerFlags \
         -o "${binDir}/acommandline_test" \
         "${projectDir}/application_name_test.cpp" \
         "${projectDir}/command_line_test.cpp" \
         "${projectDir}/command_test.cpp" \
         "${projectDir}/examples.cpp" \
         "${projectDir}/default_value_test.cpp" \
         "${projectDir}/help_test.cpp" \
         "${projectDir}/long_name_test.cpp" \
         "${projectDir}/main.cpp" \
         "${projectDir}/parse_test.cpp" \
         "${projectDir}/positional_test.cpp" \
         "${projectDir}/repeated_test.cpp" \
         "${projectDir}/required_test.cpp" \
         "${projectDir}/short_name_test.cpp" \
         "${buildRoot}/acommandline/acommandline.lib" \
         "${buildRoot}/atest/atest.lib" \
         "${buildRoot}/astl/astl.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} ^
       /ifcSearchDir \"${buildRoot}/atest\" ^
       /ifcSearchDir \"${buildRoot}/acommandline\" ^
       /Fo\"$buildDir/\" ^
       /Fe\"${binDir}/acommandline_test.exe\" ^
       \"${projectDir}/application_name_test.cpp\" \
       \"${projectDir}/command_line_test.cpp\" \
       \"${projectDir}/command_test.cpp\" \
       \"${projectDir}/examples.cpp\" \
       \"${projectDir}/default_value_test.cpp\" \
       \"${projectDir}/help_test.cpp\" \
       \"${projectDir}/long_name_test.cpp\" \
       \"${projectDir}/main.cpp\" \
       \"${projectDir}/parse_test.cpp\" \
       \"${projectDir}/positional_test.cpp\" \
       \"${projectDir}/repeated_test.cpp\" \
       \"${projectDir}/required_test.cpp\" \
       \"${projectDir}/short_name_test.cpp\" \
       \"${buildRoot}/atest/atest.lib\" ^
       \"${buildRoot}/astl/astl.lib\" ^
       \"${buildRoot}/acommandline/acommandline.lib\" || exit 1
"

sh/build/acommandline.sh "${toolchain}"
sh/build/atest.sh "${toolchain}"
build
