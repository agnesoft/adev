source "sh/build_common.sh" 

set_build_properties "awinapi" "${1}" "${2}"

function build_clang() {
    ${clang} ${clangCompilerFlags}           \
             -Xclang -emit-module-interface  \
             -o "${buildDir}/${project}.pcm" \
             -c "${projectDir}/${project}.cpp"

    ${clang} ${clangCompilerFlags}           \
             -o "${buildDir}/${project}.obj" \
             -c "${projectDir}/${project}.cpp"
}

function build_gcc() {
    ${gcc} ${gccCompilerFlags} -x c++-header "${projectDir}/windows.hpp"

    ${gcc} ${gccCompilerFlags} -o "${buildDir}/buffer.obj" -c "${projectDir}/buffer.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/last_error_message.obj" -c "${projectDir}/last_error_message.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/handle.obj" -c "${projectDir}/handle.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/pipe.obj" -c "${projectDir}/pipe.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/${project}.obj" -c "${projectDir}/${project}.cpp"

    ar r "${buildDir}/${project}.lib"         \
         "${buildDir}/buffer.obj"             \
         "${buildDir}/last_error_message.obj" \
         "${buildDir}/handle.obj"             \
         "${buildDir}/pipe.obj"               \
         "${buildDir}/${project}.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} /exportHeader                                    /ifcOutput \"${buildDir}/windows.hpp.ifc\"               /Fo\"${buildDir}/windows.hpp.obj\"                /c /TP \"${projectDir}/windows.hpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-buffer.ifc\"             /Fo\"${buildDir}/${project}-buffer.obj\"             /c /TP \"${projectDir}/buffer.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-last_error_message.ifc\" /Fo\"${buildDir}/${project}-last_error_message.obj\" /c /TP \"${projectDir}/last_error_message.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-handle.ifc\"             /Fo\"${buildDir}/${project}-handle.obj\"             /c /TP \"${projectDir}/handle.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-pipe.ifc\"             /Fo\"${buildDir}/${project}-pipe.obj\"             /c /TP \"${projectDir}/pipe.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /interface         /ifcSearchDir \"${buildDir}\" /ifcOutput \"${buildDir}/${project}.ifc\"                   /Fo\"${buildDir}/${project}.obj\"                    /c /TP \"${projectDir}/${project}.cpp\" || exit 1

lib.exe /NOLOGO                                           ^
        /OUT:\"${buildDir}/${project}.lib\"               ^
        \"${buildDir}/windows.hpp.obj\"                   ^
        \"${buildDir}/${project}-buffer.obj\"             ^
        \"${buildDir}/${project}-last_error_message.obj\" ^
        \"${buildDir}/${project}-handle.obj\"             ^
        \"${buildDir}/${project}-pipe.obj\"               ^
        \"${buildDir}/${project}.obj\" || exit 1
"

build "astl"
