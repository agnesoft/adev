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
if not exist \"${buildDir}/windows\" mkdir \"${buildDir}/windows\"
if not exist \"${buildDir}/unix\" mkdir \"${buildDir}/unix\"

cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/aprocess-environment_variable.ifc\" /Fo\"${buildDir}/aprocess-environment_variable.obj\" /c /TP \"${projectDir}/environment_variable.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/aprocess-process_setup.ifc\"        /Fo\"${buildDir}/aprocess-process_setup.obj\"        /c /TP \"${projectDir}/process_setup.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/aprocess-windows_h.ifc\"            /Fo\"${buildDir}/aprocess-windows_h.obj\"            /c /TP \"${projectDir}/windows/windows_h.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/aprocess-handle.ifc\"               /Fo\"${buildDir}/aprocess-handle.obj\"               /c /TP \"${projectDir}/windows/handle.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/aprocess-process_info.ifc\"         /Fo\"${buildDir}/aprocess-process_info.obj\"         /c /TP \"${projectDir}/windows/process_info.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/aprocess-security_attributes.ifc\"  /Fo\"${buildDir}/aprocess-security_attributes.obj\"  /c /TP \"${projectDir}/windows/security_attributes.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/aprocess-windows_pipe.ifc\"         /Fo\"${buildDir}/aprocess-windows_pipe.obj\"         /c /TP \"${projectDir}/windows/windows_pipe.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/aprocess-startup_info.ifc\"         /Fo\"${buildDir}/aprocess-startup_info.obj\"         /c /TP \"${projectDir}/windows/startup_info.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/aprocess-async_reader.ifc\"         /Fo\"${buildDir}/aprocess-async_reader.obj\"         /c /TP \"${projectDir}/windows/async_reader.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/aprocess-windows_process.ifc\"      /Fo\"${buildDir}/aprocess-windows_process.obj\"      /c /TP \"${projectDir}/windows/windows_process.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/aprocess-process.ifc\"              /Fo\"${buildDir}/aprocess-process.obj\"              /c /TP \"${projectDir}/process.cpp\" || exit 1

cl.exe ${msvcCompilerFlags} ^
       /interface ^
       /ifcSearchDir \"${buildDir}\" ^
       /ifcOutput\"${buildDir}/aprocess.ifc\" ^
       /Fo\"${buildDir}/aprocess.obj\" ^
       /c \"${projectDir}/aprocess.cpp\" || exit 1

lib.exe /NOLOGO ^
        /OUT:\"${buildDir}/aprocess.lib\" ^
        \"${buildDir}/aprocess.obj\" ^
        \"${buildDir}/aprocess-environment_variable.obj\" ^
        \"${buildDir}/aprocess-process_setup.obj\" ^
        \"${buildDir}/aprocess-windows_h.obj\" ^
        \"${buildDir}/aprocess-handle.obj\" ^
        \"${buildDir}/aprocess-process_info.obj\" ^
        \"${buildDir}/aprocess-security_attributes.obj\" ^
        \"${buildDir}/aprocess-startup_info.obj\" ^
        \"${buildDir}/aprocess-async_reader.obj\" ^
        \"${buildDir}/aprocess-windows_pipe.obj\" ^
        \"${buildDir}/aprocess-windows_process.obj\" ^
        \"${buildDir}/aprocess-process.obj\" || exit 1
"

sh/build/astl.sh "${toolchain}"
build
