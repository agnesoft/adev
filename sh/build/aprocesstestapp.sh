source "sh/build_common.sh" "${1}"

project="aprocesstestapp"
projectDir="projects/aprocess/test/aprocesstestapp"
buildDir="${buildRoot}/aprocess/test/aprocesstestapp"

function build_clang() {
    $clang $clangCompilerLinkerFlags \
           -fprebuilt-module-path=${buildRoot}/aprocess \
           -fprebuilt-module-path=${buildRoot}/acommandline \
           -o "${binDir}/aprocesstestapp${executableExtension}" \
           "${projectDir}/main.cpp" \
           "${buildRoot}/aprocess/aprocess.obj" \
           "${buildRoot}/acommandline/acommandline.obj" \
           "${buildRoot}/astl/astl.obj"
}

function build_gcc() {
    $gcc $gccCompilerFlags \
         -o "${binDir}/aprocesstestapp" \
         "${projectDir}/main.cpp" \
         "${buildRoot}/aprocess/aprocess.lib" \
         "${buildRoot}/acommandline/acommandline.obj" \
         "${buildRoot}/astl/astl.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} ^
       /ifcSearchDir \"${buildRoot}/acommandline\" ^
       /ifcSearchDir \"${buildRoot}/aprocess\" ^
       /Fo\"$buildDir/\" ^
       /Fe\"${binDir}/aprocesstestapp.exe\" ^
       \"${projectDir}/main.cpp\" \
       \"${buildRoot}/astl/astl.lib\" ^
       \"${buildRoot}/acommandline/acommandline.lib\" ^
       \"${buildRoot}/aprocess/aprocess.lib\" || exit 1
"

sh/build/acommandline.sh "${toolchain}"
sh/build/aprocess.sh "${toolchain}"
build
