source "sh/build_common.sh" "${1}"

project="astl_test"
projectDir="projects/astl/test"
buildDir="${buildRoot}/astl/test"

function build_clang() {
    $clang $clangCompilerLinkerFlags \
           -fprebuilt-module-path=${buildRoot}/atest \
           -o "${binDir}/astl_test${executableExtension}" \
           "${projectDir}/main.cpp" \
           "${projectDir}/source_location_test.cpp" \
           "${buildRoot}/atest/atest.obj" \
           "${buildRoot}/acommandline/acommandline.obj" \
           "${buildRoot}/astl/astl.obj"
}

function build_gcc() {
    $gcc $gccCompilerFlags \
         -o "${binDir}/astl_test" \
         "${projectDir}/main.cpp" \
         "${projectDir}/source_location_test.cpp" \
         "${buildRoot}/atest/atest.lib" \
         "${buildRoot}/acommandline/acommandline.lib" \
         "${buildRoot}/astl/astl.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} ^
       /ifcSearchDir \"${buildRoot}/atest\" ^
       /Fo\"$buildDir/\" ^
       /Fe\"${binDir}/astl_test.exe\" ^
       \"${projectDir}/main.cpp\" \
       \"${projectDir}/source_location_test.cpp\" \
       \"${buildRoot}/atest/atest.lib\" ^
       \"${buildRoot}/acommandline/acommandline.lib\" ^
       \"${buildRoot}/astl/astl.lib\" || exit 1
"

sh/build/astl.sh "${toolchain}"
sh/build/atest.sh "${toolchain}"
build
