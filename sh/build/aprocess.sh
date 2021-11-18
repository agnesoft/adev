source "sh/build_common.sh" "${1}"

project="aprocess"
projectDir="projects/${project}"
buildDir="${buildRoot}/${project}"

function build_clang() {
    $clang $clangCompilerFlags \
           -Xclang -emit-module-interface \
           -fprebuilt-module-path=${buildRoot}/awinapi \
           -o "${buildDir}/aprocess.pcm" \
           -c "${projectDir}/aprocess.cpp"
    
    $clang $clangCompilerFlags \
           -fprebuilt-module-path=${buildRoot}/awinapi \
           -o "${buildDir}/aprocess.obj" \
           -c "${projectDir}/aprocess.cpp"
}

function build_gcc() {
    $gcc $gccCompilerFlags -x c++-system-header "unistd.h"
    $gcc $gccCompilerFlags -x c++-system-header "wait.h"

    $gcc $gccCompilerFlags -o "${buildDir}/environment_variable.obj" -c "${projectDir}/environment_variable.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/process_setup.obj"        -c "${projectDir}/process_setup.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/async_reader.obj"         -c "${projectDir}/async_reader.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/pipe.obj"                 -c "${projectDir}/pipe.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/unix_process.obj"         -c "${projectDir}/unix_process.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/windows_process.obj"      -c "${projectDir}/windows_process.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/aprocess.obj"             -c "${projectDir}/aprocess.cpp"

    ar r "${buildDir}/aprocess.lib" \
         "${buildDir}/environment_variable.obj" \
         "${buildDir}/process_setup.obj" \
         "${buildDir}/async_reader.obj" \
         "${buildDir}/pipe.obj" \
         "${buildDir}/unix_process.obj" \
         "${buildDir}/windows_process.obj" \
         "${buildDir}/aprocess.obj"
}

buildMSVC="
if not exist \"${buildDir}/windows\" mkdir \"${buildDir}/windows\"
if not exist \"${buildDir}/unix\" mkdir \"${buildDir}/unix\"

cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/awinapi\" /ifcOutput\"${buildDir}/aprocess-environment_variable.ifc\" /Fo\"${buildDir}/aprocess-environment_variable.obj\" /c /TP \"${projectDir}/environment_variable.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/awinapi\" /ifcOutput\"${buildDir}/aprocess-process_setup.ifc\"        /Fo\"${buildDir}/aprocess-process_setup.obj\"        /c /TP \"${projectDir}/process_setup.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/awinapi\" /ifcOutput\"${buildDir}/aprocess-async_reader.ifc\"         /Fo\"${buildDir}/aprocess-async_reader.obj\"         /c /TP \"${projectDir}/async_reader.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/awinapi\" /ifcOutput\"${buildDir}/aprocess-pipe.ifc\"                 /Fo\"${buildDir}/aprocess-pipe.obj\"                 /c /TP \"${projectDir}/pipe.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/awinapi\" /ifcOutput\"${buildDir}/aprocess-unix_process.ifc\"         /Fo\"${buildDir}/aprocess-unix_process.obj\"         /c /TP \"${projectDir}/unix_process.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/awinapi\" /ifcOutput\"${buildDir}/aprocess-windows_process.ifc\"      /Fo\"${buildDir}/aprocess-windows_process.obj\"      /c /TP \"${projectDir}/windows_process.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/awinapi\" /ifcOutput\"${buildDir}/aprocess-process.ifc\"              /Fo\"${buildDir}/aprocess-process.obj\"              /c /TP \"${projectDir}/process.cpp\" || exit 1

cl.exe ${msvcCompilerFlags} ^
       /interface ^
       /ifcSearchDir \"${buildDir}\" ^
       /ifcSearchDir \"${buildRoot}/awinapi\" ^
       /ifcOutput\"${buildDir}/aprocess.ifc\" ^
       /Fo\"${buildDir}/aprocess.obj\" ^
       /c \"${projectDir}/aprocess.cpp\" || exit 1

lib.exe /NOLOGO ^
        /OUT:\"${buildDir}/aprocess.lib\" ^
        \"${buildDir}/aprocess.obj\" ^
        \"${buildDir}/aprocess-environment_variable.obj\" ^
        \"${buildDir}/aprocess-process_setup.obj\" ^
        \"${buildDir}/aprocess-async_reader.obj\" ^
        \"${buildDir}/aprocess-pipe.obj\" ^
        \"${buildDir}/aprocess-unix_process.obj\" ^
        \"${buildDir}/aprocess-windows_process.obj\" ^
        \"${buildDir}/aprocess-process.obj\" || exit 1
"

sh/build/astl.sh "${toolchain}"
sh/build/awinapi.sh "${toolchain}"
build
