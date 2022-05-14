source "sh/build_common.sh" 

set_build_properties "aprocess" "${1}" "${2}"

function build_clang() {
    ${clang} ${clangCompilerFlags}                         \
             -Xclang -emit-module-interface                \
             -fprebuilt-module-path="${buildRoot}/awinapi" \
             -o "${buildDir}/${project}.pcm"               \
             -c "${projectDir}/${project}.cpp"
    
    ${clang} ${clangCompilerFlags}                         \
             -fprebuilt-module-path="${buildRoot}/awinapi" \
             -o "${buildDir}/${project}.obj"               \
             -c "${projectDir}/${project}.cpp"
}

function build_gcc() {
    ${gcc} ${gccCompilerFlags} -x c++-system-header "fcntl.h"
    ${gcc} ${gccCompilerFlags} -x c++-system-header "unistd.h"
    ${gcc} ${gccCompilerFlags} -x c++-system-header "wait.h"

    ${gcc} ${gccCompilerFlags} -o "${buildDir}/environment_variable.obj" -c "${projectDir}/environment_variable.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/process_setup.obj"        -c "${projectDir}/process_setup.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/pipe.obj"                 -c "${projectDir}/pipe.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/unix_process.obj"         -c "${projectDir}/unix_process.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/windows_process.obj"      -c "${projectDir}/windows_process.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/${project}.obj"           -c "${projectDir}/${project}.cpp"

    ar r "${buildDir}/${project}.lib"           \
         "${buildDir}/environment_variable.obj" \
         "${buildDir}/process_setup.obj"        \
         "${buildDir}/pipe.obj"                 \
         "${buildDir}/unix_process.obj"         \
         "${buildDir}/windows_process.obj"      \
         "${buildDir}/${project}.obj"
}

buildMSVC="
if not exist \"${buildDir}/windows\" mkdir \"${buildDir}/windows\"
if not exist \"${buildDir}/unix\" mkdir \"${buildDir}/unix\"

cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/awinapi\" /ifcOutput\"${buildDir}/${project}-environment_variable.ifc\" /Fo\"${buildDir}/${project}-environment_variable.obj\" /c /TP \"${projectDir}/environment_variable.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/awinapi\" /ifcOutput\"${buildDir}/${project}-process_setup.ifc\"        /Fo\"${buildDir}/${project}-process_setup.obj\"        /c /TP \"${projectDir}/process_setup.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/awinapi\" /ifcOutput\"${buildDir}/${project}-pipe.ifc\"                 /Fo\"${buildDir}/${project}-pipe.obj\"                 /c /TP \"${projectDir}/pipe.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/awinapi\" /ifcOutput\"${buildDir}/${project}-unix_process.ifc\"         /Fo\"${buildDir}/${project}-unix_process.obj\"         /c /TP \"${projectDir}/unix_process.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/awinapi\" /ifcOutput\"${buildDir}/${project}-windows_process.ifc\"      /Fo\"${buildDir}/${project}-windows_process.obj\"      /c /TP \"${projectDir}/windows_process.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/awinapi\" /ifcOutput\"${buildDir}/${project}-process.ifc\"              /Fo\"${buildDir}/${project}-process.obj\"              /c /TP \"${projectDir}/process.cpp\" || exit 1

cl.exe ${msvcCompilerFlags}                     ^
       /interface                               ^
       /ifcSearchDir \"${buildDir}\"            ^
       /ifcSearchDir \"${buildRoot}/awinapi\"   ^
       /ifcOutput\"${buildDir}/${project}.ifc\" ^
       /Fo\"${buildDir}/${project}.obj\"        ^
       /c \"${projectDir}/${project}.cpp\" || exit 1

lib.exe /NOLOGO                                             ^
        /OUT:\"${buildDir}/${project}.lib\"                 ^
        \"${buildDir}/${project}.obj\"                      ^
        \"${buildDir}/${project}-environment_variable.obj\" ^
        \"${buildDir}/${project}-process_setup.obj\"        ^
        \"${buildDir}/${project}-pipe.obj\"                 ^
        \"${buildDir}/${project}-unix_process.obj\"         ^
        \"${buildDir}/${project}-windows_process.obj\"      ^
        \"${buildDir}/${project}-process.obj\" || exit 1
"

build "awinapi"
