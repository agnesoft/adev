source "sh/common_build.sh" $1

project="astl"
projectDir="projects/${project}"
buildDir="${buildRoot}/${project}"

function build_clang() {
     $clang $clangCompilerFlags -Xclang -emit-module-interface -o "${buildDir}/astl.pcm" -c "${projectDir}/astl.cpp"
     $clang $clangCompilerFlags -o "${buildDir}/astl.obj" -c "${projectDir}/astl.cpp"
}

function build_gcc() {
     $gcc $gccCompilerFlags -x c++-header "${projectDir}/astl.hpp"
     $gcc $gccCompilerFlags -o "${buildDir}/astl.obj" -c "${projectDir}/astl.cpp"
}

msvcBuild="
cl.exe ${msvcCompilerFlags} ^
       /exportHeader ^
       /ifcOutput \"${buildDir}/astl.hpp.ifc\" ^
       /Fo\"${buildDir}/astl.hpp.obj\" ^
       /c ^
       /TP ^
       \"${projectDir}/astl.hpp\"

cl.exe ${msvcCompilerFlags} ^
       /interface ^
       /ifcOutput \"${buildDir}/astl.ifc\" ^
       /Fo\"${buildDir}/astl.obj\" ^
       /c ^
       /TP ^
       \"${projectDir}/astl.cpp\"

lib.exe /NOLOGO ^
        /OUT:\"${buildDir}/astl.lib\" ^
        \"${buildDir}/astl.hpp.obj\" ^
        \"${buildDir}/astl.obj\"
"

build
