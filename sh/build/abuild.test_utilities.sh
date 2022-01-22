source "sh/build_common.sh" "${1}"

project="abuild.test_utilities"
projectDir="projects/abuild/test_utilities"
buildDir="${buildRoot}/abuild/test_utilities"

function build_clang() {

    $clang $clangCompilerFlags -Xclang -emit-module-interface -o "${buildDir}/abuild.test_file.pcm" -c "${projectDir}/test_file.cpp"
    $clang $clangCompilerFlags -o "${buildDir}/abuild.test_utilities.obj" -c "${projectDir}/test_utilities.cpp"
}

function build_gcc() {
    $gcc $gccCompilerFlags -o "${buildDir}/test_file.obj" -c "${projectDir}/test_file.cpp"

    $gcc $gccCompilerFlags -o "${buildDir}/test_utilities.obj" -c "${projectDir}/test_utilities.cpp"

    ar r "${buildDir}/test_file.obj" \
         "${buildDir}/test_utilities.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/abuild.test_utilities-test_file.ifc\" /Fo\"${buildDir}/abuild.test_utilities-test_file.obj\" /c /TP \"${projectDir}/test_file.cpp\" || exit 1

cl.exe ${msvcCompilerFlags} ^
       /interface ^
       /ifcSearchDir \"${buildDir}\" ^
       /ifcOutput\"${buildDir}/abuild.test_utilities.ifc\" ^
       /Fo\"${buildDir}/abuild.test_utilities.obj\" ^
       /c \"${projectDir}/test_utilities.cpp\" || exit 1

lib.exe /NOLOGO ^
        /OUT:\"${buildDir}/abuild.test_utilities.lib\" ^
        \"${buildDir}/abuild.test_utilities.obj\" ^
        \"${buildDir}/abuild.test_utilities-test_file.obj\" || exit 1
"

sh/build/astl.sh "${toolchain}"
build
