source "sh/build_common.sh" "${1}"

project="awinapi"
projectDir="projects/${project}"
buildDir="${buildRoot}/${project}"

function build_clang() {
    $clang $clangCompilerFlags -Xclang -emit-module-interface -o "${buildDir}/awinapi.pcm" -c "${projectDir}/awinapi.cpp"
    $clang $clangCompilerFlags                                -o "${buildDir}/awinapi.obj" -c "${projectDir}/awinapi.cpp"
}

function build_gcc() {
    $gcc $gccCompilerFlags -x c++-header "${projectDir}/windows.hpp"
    $gcc $gccCompilerFlags -o "${buildDir}/awinapi.obj" -c "${projectDir}/awinapi.cpp"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} /exportHeader /ifcOutput \"${buildDir}/windows.hpp.ifc\" /Fo\"${buildDir}/windows.hpp.obj\" /c /TP \"${projectDir}/windows.hpp\" || exit 1
cl.exe ${msvcCompilerFlags} /interface    /ifcOutput \"${buildDir}/awinapi.ifc\"     /Fo\"${buildDir}/awinapi.obj\"     /c /TP \"${projectDir}/awinapi.cpp\" || exit 1
lib.exe /NOLOGO /OUT:\"${buildDir}/awinapi.lib\" \"${buildDir}/windows.hpp.obj\" \"${buildDir}/awinapi.obj\" || exit 1
"

sh/build/astl.sh "${toolchain}"
build
