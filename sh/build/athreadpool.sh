source "sh/build_common.sh" 

set_build_properties "athreadpool" "${1}" "${2}"

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
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/job.obj"         -c "${projectDir}/job.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/jobs.obj"        -c "${projectDir}/jobs.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/queue.obj"       -c "${projectDir}/queue.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/monitor.obj"     -c "${projectDir}/monitor.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/threadpool.obj" -c "${projectDir}/threadpool.cpp"

    ${gcc} ${gccCompilerFlags} -o "${buildDir}/${project}.obj" -c "${projectDir}/${project}.cpp"

    ar r "${buildDir}/${project}.lib" \
         "${buildDir}/${project}.obj" \
         "${buildDir}/job.obj"        \
         "${buildDir}/jobs.obj"       \
         "${buildDir}/queue.obj"      \
         "${buildDir}/monitor.obj"    \
         "${buildDir}/threadpool.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-job.ifc\"         /Fo\"${buildDir}/${project}-job.obj\"         /c /TP \"${projectDir}/job.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-jobs.ifc\"        /Fo\"${buildDir}/${project}-jobs.obj\"        /c /TP \"${projectDir}/jobs.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-queue.ifc\"       /Fo\"${buildDir}/${project}-queue.obj\"       /c /TP \"${projectDir}/queue.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-monitor.ifc\"     /Fo\"${buildDir}/${project}-monitor.obj\"     /c /TP \"${projectDir}/monitor.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-threadpool.ifc\" /Fo\"${buildDir}/${project}-threadpool.obj\" /c /TP \"${projectDir}/threadpool.cpp\" || exit 1

cl.exe ${msvcCompilerFlags}                     ^
       /interface                               ^
       /ifcSearchDir \"${buildDir}\"            ^
       /ifcOutput\"${buildDir}/${project}.ifc\" ^
       /Fo\"${buildDir}/${project}.obj\"        ^
       /c \"${projectDir}/${project}.cpp\" || exit 1

lib.exe /NOLOGO                                ^
        /OUT:\"${buildDir}/${project}.lib\"    ^
        \"${buildDir}/${project}.obj\"         ^
        \"${buildDir}/${project}-job.obj\"     ^
        \"${buildDir}/${project}-jobs.obj\"    ^
        \"${buildDir}/${project}-queue.obj\"   ^
        \"${buildDir}/${project}-monitor.obj\" ^
        \"${buildDir}/${project}-threadpool.obj\" || exit 1
"

build "astl"
