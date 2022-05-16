source "sh/build_common.sh" 

set_build_properties "astl" "${1}" "${2}"

function build_clang() {
    ${clang} ${clangCompilerFlags}           \
             -Xclang                         \
             -emit-module-interface          \
             -o "${buildDir}/${project}.pcm" \
             -c "${projectDir}/${project}.cpp"

    ${clang} ${clangCompilerFlags}           \
             -o "${buildDir}/${project}.obj" \
             -c "${projectDir}/${project}.cpp"
}

function build_gcc() {
    ${gcc} ${gccCompilerFlags} -x c++-header "${projectDir}/${project}.hpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/${project}.obj" -c "${projectDir}/${project}.cpp"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} /exportHeader /ifcOutput \"${buildDir}/${project}.hpp.ifc\" /Fo\"${buildDir}/${project}.hpp.obj\" /c /TP \"${projectDir}/${project}.hpp\" || exit 1
cl.exe ${msvcCompilerFlags} /interface    /ifcOutput \"${buildDir}/${project}.ifc\"     /Fo\"${buildDir}/${project}.obj\"     /c /TP \"${projectDir}/${project}.cpp\" || exit 1
lib.exe /NOLOGO /OUT:\"${buildDir}/${project}.lib\" \"${buildDir}/${project}.hpp.obj\" \"${buildDir}/${project}.obj\" || exit 1
"

build
