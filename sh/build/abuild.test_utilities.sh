source "sh/build_common.sh" "${1}"

project="abuild.test_utilities"
projectDir="projects/abuild/test_utilities"
buildDir="${buildRoot}/abuild/test_utilities"

function build_clang() {

    $clang $clangCompilerFlags \
           -Xclang -emit-module-interface \
           -fprebuilt-module-path=${buildRoot}/abuild/cpptokenizer \
           -o "${buildDir}/abuild.test_utilities.pcm" \
           -c "${projectDir}/test_utilities.cpp"
    
    $clang $clangCompilerFlags \
           -fprebuilt-module-path=${buildRoot}/abuild/cpptokenizer \
           -o "${buildDir}/abuild.test_utilities.obj" \
           -c "${projectDir}/test_utilities.cpp"
}

function build_gcc() {
    $gcc $gccCompilerFlags -o "${buildDir}/test_file.obj" -c "${projectDir}/test_file.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/token_operators.obj" -c "${projectDir}/token_operators.cpp"

    $gcc $gccCompilerFlags -o "${buildDir}/test_utilities.obj" -c "${projectDir}/test_utilities.cpp"

    ar r "${buildDir}/test_file.obj" \
         "${buildDir}/test_utilities.obj" \
         "${buildDir}/token_operators.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" ^ /ifcOutput\"${buildDir}/abuild.test_utilities-test_file.ifc\" /Fo\"${buildDir}/abuild.test_utilities-test_file.obj\" /c /TP \"${projectDir}/test_file.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" ^ /ifcOutput\"${buildDir}/abuild.test_utilities-token_operators.ifc\" /Fo\"${buildDir}/abuild.test_utilities-token_operators.obj\" /c /TP \"${projectDir}/token_operators.cpp\" || exit 1

cl.exe ${msvcCompilerFlags} ^
       /interface ^
       /ifcSearchDir \"${buildDir}\" ^
       /ifcOutput\"${buildDir}/abuild.test_utilities.ifc\" ^
       /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" ^
       /Fo\"${buildDir}/abuild.test_utilities.obj\" ^
       /c \"${projectDir}/test_utilities.cpp\" || exit 1

lib.exe /NOLOGO ^
        /OUT:\"${buildDir}/abuild.test_utilities.lib\" ^
        \"${buildDir}/abuild.test_utilities.obj\" ^
        \"${buildDir}/abuild.test_utilities-test_file.obj\" ^
        \"${buildDir}/abuild.test_utilities-token_operators.obj\" || exit 1
"

sh/build/abuild.cpptokenizer.sh "${toolchain}"
build
