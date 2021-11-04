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
    $gcc $gccCompilerFlags -o "${buildDir}/buffer.obj" -c "${projectDir}/buffer.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/last_error_message.obj" -c "${projectDir}/last_error_message.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/handle.obj" -c "${projectDir}/handle.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/pipe.obj" -c "${projectDir}/pipe.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/awinapi.obj" -c "${projectDir}/awinapi.cpp"

    ar r "${buildDir}/awinapi.lib" \
         "${buildDir}/buffer.obj" \
         "${buildDir}/last_error_message.obj" \
         "${buildDir}/handle.obj" \
         "${buildDir}/pipe.obj" \
         "${buildDir}/awinapi.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} /exportHeader                                    /ifcOutput \"${buildDir}/windows.hpp.ifc\"               /Fo\"${buildDir}/windows.hpp.obj\"                /c /TP \"${projectDir}/windows.hpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/awinapi-buffer.ifc\"             /Fo\"${buildDir}/awinapi-buffer.obj\"             /c /TP \"${projectDir}/buffer.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/awinapi-last_error_message.ifc\" /Fo\"${buildDir}/awinapi-last_error_message.obj\" /c /TP \"${projectDir}/last_error_message.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/awinapi-handle.ifc\"             /Fo\"${buildDir}/awinapi-handle.obj\"             /c /TP \"${projectDir}/handle.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/awinapi-pipe.ifc\"             /Fo\"${buildDir}/awinapi-pipe.obj\"             /c /TP \"${projectDir}/pipe.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /interface         /ifcSearchDir \"${buildDir}\" /ifcOutput \"${buildDir}/awinapi.ifc\"                   /Fo\"${buildDir}/awinapi.obj\"                    /c /TP \"${projectDir}/awinapi.cpp\" || exit 1

lib.exe /NOLOGO /OUT:\"${buildDir}/awinapi.lib\" ^
                     \"${buildDir}/windows.hpp.obj\" ^
                     \"${buildDir}/awinapi-buffer.obj\" ^
                     \"${buildDir}/awinapi-last_error_message.obj\" ^
                     \"${buildDir}/awinapi-handle.obj\" ^
                     \"${buildDir}/awinapi-pipe.obj\" ^
                     \"${buildDir}/awinapi.obj\" || exit 1
"

sh/build/astl.sh "${toolchain}"
build
