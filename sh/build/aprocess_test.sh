source "sh/build_common.sh" "${1}"

project="aprocess_test"
projectDir="projects/aprocess/test"
buildDir="${buildRoot}/aprocess/test"

function build_clang() {
    $clang $clangCompilerFlags -Xclang -emit-module-interface -o "${buildDir}/aprocesstest.pcm" -c "${projectDir}/aprocesstest.cpp"
    $clang $clangCompilerFlags -o "${buildDir}/aprocesstest.obj" -c "${projectDir}/aprocesstest.cpp"

    $clang $clangCompilerLinkerFlags \
           -fprebuilt-module-path=${buildRoot}/atest \
           -fprebuilt-module-path=${buildRoot}/aprocess \
           -fprebuilt-module-path=${buildRoot}/aprocess/test \
           -o "${binDir}/aprocess_test$executableSuffix" \
           "${projectDir}/examples.cpp" \
           "${projectDir}/exit_code.cpp" \
           "${projectDir}/main.cpp" \
           "${buildRoot}/aprocess/aprocess.obj" \
           "${buildRoot}/acommandline/acommandline.obj" \
           "${buildRoot}/atest/atest.obj" \
           "${buildRoot}/astl/astl.obj" \
           "${buildRoot}/astl/aprocesstest.obj"
}

function build_gcc() {
    $gcc $gccCompilerFlags -o "${buildDir}/set_path_unix.obj"    -c "${projectDir}/set_path_unix.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/set_path_windows.obj" -c "${projectDir}/set_path_windows.cpp"

    $gcc $gccCompilerFlags -o "${buildDir}/aprocesstest.obj"     -c "${projectDir}/aprocesstest.cpp"

    ar r "${buildDir}/aprocesstest.lib" \
         "${buildDir}/aprocesstest.obj" \
         "${buildDir}/set_path_unix.obj" \
         "${buildDir}/set_path_windows.obj"

    $gcc $gccCompilerFlags \
         -o "${binDir}/aprocess_test" \
         "${projectDir}/examples.cpp" \
         "${projectDir}/exit_code.cpp" \
         "${projectDir}/main.cpp" \
         "${buildRoot}/aprocess/aprocess.lib" \
         "${buildRoot}/acommandline/acommandline.obj" \
         "${buildRoot}/atest/atest.lib" \
         "${buildRoot}/astl/astl.obj" \
         "${buildRoot}/astl/aprocesstest.lib"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/aprocesstest-set_path_unix.ifc\"    /Fo\"${buildDir}/aprocesstest-set_path_unix.obj\"    /c /TP \"${projectDir}/set_path_unix.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/aprocesstest-set_path_windows.ifc\" /Fo\"${buildDir}/aprocesstest-set_path_windows.obj\" /c /TP \"${projectDir}/set_path_windows.cpp\" || exit 1

cl.exe ${msvcCompilerFlags} ^
       /interface ^
       /ifcSearchDir \"${buildDir}\" ^
       /ifcOutput\"${buildDir}/aprocesstest.ifc\" ^
       /Fo\"${buildDir}/aprocesstest.obj\" ^
       /c \"${projectDir}/aprocesstest.cpp\" || exit 1

lib.exe /NOLOGO ^
        /OUT:\"${buildDir}/aprocesstest.lib\" ^
        \"${buildDir}/aprocesstest.obj\" ^
        \"${buildDir}/aprocesstest-set_path_unix.obj\" ^
        \"${buildDir}/aprocesstest-set_path_windows.obj\" || exit 1

cl.exe ${msvcCompilerFlags} ^
       /ifcSearchDir \"${buildRoot}/atest\" ^
       /ifcSearchDir \"${buildRoot}/aprocess\" ^
       /ifcSearchDir \"${buildRoot}/awinapi\" ^
       /ifcSearchDir \"${buildDir}\" ^
       /Fo\"$buildDir/\" ^
       /Fe\"${binDir}/aprocess_test.exe\" ^
       \"${projectDir}/examples.cpp\" \
       \"${projectDir}/exit_code.cpp\" \
       \"${projectDir}/main.cpp\" \
       \"${buildRoot}/atest/atest.lib\" ^
       \"${buildRoot}/astl/astl.lib\" ^
       \"${buildRoot}/awinapi/awinapi.lib\" ^
       \"${buildRoot}/acommandline/acommandline.lib\" ^
       \"${buildRoot}/aprocess/aprocess.lib\" ^
       \"${buildRoot}/aprocess/test/aprocesstest.lib\" || exit 1
"

sh/build/aprocess.sh "${toolchain}"
sh/build/atest.sh "${toolchain}"
sh/build/aprocesstestapp.sh "${toolchain}"
build
