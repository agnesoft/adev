source "sh/build_common.sh" "${1}"

project="aprocess_test"
projectDir="projects/aprocess/test"
buildDir="${buildRoot}/aprocess/test"

function build_clang() {
    $clang $clangCompilerLinkerFlags \
           -fprebuilt-module-path=${buildRoot}/atest \
           -fprebuilt-module-path=${buildRoot}/aprocess \
           -fprebuilt-module-path=${buildRoot}/awinapi \
           -o "${binDir}/aprocess_test${executableExtension}" \
           "${projectDir}/arguments_test.cpp" \
           "${projectDir}/command_test.cpp" \
           "${projectDir}/detached_test.cpp" \
           "${projectDir}/environment_test.cpp" \
           "${projectDir}/examples.cpp" \
           "${projectDir}/exit_code_test.cpp" \
           "${projectDir}/input_test.cpp" \
           "${projectDir}/is_process_running.cpp" \
           "${projectDir}/kill_test.cpp" \
           "${projectDir}/output_test.cpp" \
           "${projectDir}/main.cpp" \
           "${projectDir}/terminate_test.cpp" \
           "${projectDir}/wait_test.cpp" \
           "${projectDir}/working_directory_test.cpp" \
           "${buildRoot}/aprocess/aprocess.obj" \
           "${buildRoot}/acommandline/acommandline.obj" \
           "${buildRoot}/atest/atest.obj" \
           "${buildRoot}/awinapi/awinapi.obj" \
           "${buildRoot}/astl/astl.obj"
}

function build_gcc() {
    $gcc $gccCompilerFlags \
         -o "${binDir}/aprocess_test" \
         "${projectDir}/arguments_test.cpp" \
         "${projectDir}/command_test.cpp" \
         "${projectDir}/detached_test.cpp" \
         "${projectDir}/environment_test.cpp" \
         "${projectDir}/examples.cpp" \
         "${projectDir}/exit_code_test.cpp" \
         "${projectDir}/input_test.cpp" \
         "${projectDir}/is_process_running.cpp" \
         "${projectDir}/kill_test.cpp" \
         "${projectDir}/output_test.cpp" \
         "${projectDir}/main.cpp" \
         "${projectDir}/terminate_test.cpp" \
         "${projectDir}/wait_test.cpp" \
         "${projectDir}/working_directory_test.cpp" \
         "${buildRoot}/aprocess/aprocess.lib" \
         "${buildRoot}/acommandline/acommandline.obj" \
         "${buildRoot}/atest/atest.lib" \
         "${buildRoot}/awinapi/awinapi.obj" \
         "${buildRoot}/astl/astl.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} ^
       /ifcSearchDir \"${buildRoot}/atest\" ^
       /ifcSearchDir \"${buildRoot}/aprocess\" ^
       /ifcSearchDir \"${buildRoot}/awinapi\" ^
       /ifcSearchDir \"${buildDir}\" ^
       /Fo\"$buildDir/\" ^
       /Fe\"${binDir}/aprocess_test.exe\" ^
       \"${projectDir}/arguments_test.cpp\" ^
       \"${projectDir}/command_test.cpp\" ^
       \"${projectDir}/detached_test.cpp\" ^
       \"${projectDir}/environment_test.cpp\" ^
       \"${projectDir}/examples.cpp\" ^
       \"${projectDir}/exit_code_test.cpp\" ^
       \"${projectDir}/input_test.cpp\" ^
       \"${projectDir}/is_process_running.cpp\" ^
       \"${projectDir}/kill_test.cpp\" ^
       \"${projectDir}/output_test.cpp\" ^
       \"${projectDir}/main.cpp\" ^
       \"${projectDir}/terminate_test.cpp\" ^
       \"${projectDir}/wait_test.cpp\" ^
       \"${projectDir}/working_directory_test.cpp\" ^
       \"${buildRoot}/atest/atest.lib\" ^
       \"${buildRoot}/astl/astl.lib\" ^
       \"${buildRoot}/awinapi/awinapi.lib\" ^
       \"${buildRoot}/acommandline/acommandline.lib\" ^
       \"${buildRoot}/aprocess/aprocess.lib\" || exit 1
"

sh/build/aprocess.sh "${toolchain}"
sh/build/atest.sh "${toolchain}"
sh/build/aprocesstestapp.sh "${toolchain}"
build
