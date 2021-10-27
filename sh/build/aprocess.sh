source "sh/build_common.sh" "${1}"

project="aprocess"
projectDir="projects/${project}"
buildDir="${buildRoot}/${project}"

function build_clang() {

    $clang $clangCompilerFlags -Xclang -emit-module-interface -o "${buildDir}/aprocess.pcm" -c "${projectDir}/aprocess.cpp"
    $clang $clangCompilerFlags -o "${buildDir}/aprocess.obj" -c "${projectDir}/aprocess.cpp"
}

function build_gcc() {
    $gcc $gccCompilerFlags -o "${buildDir}/aprocess.obj" -c "${projectDir}/aprocess.cpp"

    ar r "${buildDir}/aprocess.lib" \
         "${buildDir}/aprocess.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} ^
       /interface ^
       /ifcSearchDir \"${buildDir}\" ^
       /ifcOutput\"${buildDir}/aprocess.ifc\" ^
       /Fo\"${buildDir}/aprocess.obj\" ^
       /c \"${projectDir}/aprocess.cpp\" || exit 1

lib.exe /NOLOGO ^
        /OUT:\"${buildDir}/aprocess.lib\" ^
        \"${buildDir}/aprocess.obj\" || exit 1
"

sh/build/astl.sh "${toolchain}"
build
