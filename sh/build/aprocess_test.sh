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
           "${projectDir}/examples.cpp" \
           "${projectDir}/exit_code.cpp" \
           "${projectDir}/main.cpp" \
           "${buildRoot}/aprocess/aprocess.obj" \
           "${buildRoot}/acommandline/acommandline.obj" \
           "${buildRoot}/atest/atest.obj" \
           "${buildRoot}/awinapi/awinapi.obj" \
           "${buildRoot}/astl/astl.obj"
}

function build_gcc() {
    $gcc $gccCompilerFlags \
         -o "${binDir}/aprocess_test" \
         "${projectDir}/examples.cpp" \
         "${projectDir}/exit_code.cpp" \
         "${projectDir}/main.cpp" \
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
       \"${projectDir}/examples.cpp\" ^
       \"${projectDir}/exit_code.cpp\" ^
       \"${projectDir}/main.cpp\" ^
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
