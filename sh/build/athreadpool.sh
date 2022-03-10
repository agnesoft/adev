source "sh/build_common.sh" "${1}"

project="athreadpool"
projectDir="projects/${project}"
buildDir="${buildRoot}/${project}"

function build_clang() {
    $clang $clangCompilerFlags \
           -Xclang \
           -emit-module-interface \
           -fprebuilt-module-path=${buildRoot}/athreadpool \
           -o "${buildDir}/athreadpool.pcm" \
           -c "${projectDir}/athreadpool.cpp"
    
    $clang $clangCompilerFlags \
           -fprebuilt-module-path=${buildRoot}/athreadpool \
           -o "${buildDir}/athreadpool.obj" \
           -c "${projectDir}/athreadpool.cpp"
}

function build_gcc() {
    $gcc $gccCompilerFlags -o "${buildDir}/job.obj"         -c "${projectDir}/job.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/jobs.obj"        -c "${projectDir}/jobs.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/queue.obj"       -c "${projectDir}/queue.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/monitor.obj"     -c "${projectDir}/monitor.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/threadpool.obj" -c "${projectDir}/threadpool.cpp"

    $gcc $gccCompilerFlags -o "${buildDir}/athreadpool.obj" -c "${projectDir}/athreadpool.cpp"

    ar r "${buildDir}/atest.lib" \
         "${buildDir}/athreadpool.obj" \
         "${buildDir}/job.obj" \
         "${buildDir}/jobs.obj" \
         "${buildDir}/queue.obj" \
         "${buildDir}/monitor.obj" \
         "${buildDir}/threadpool.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/athreadpool-job.ifc\"         /Fo\"${buildDir}/athreadpool-job.obj\"         /c /TP \"${projectDir}/job.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/athreadpool-jobs.ifc\"        /Fo\"${buildDir}/athreadpool-jobs.obj\"        /c /TP \"${projectDir}/jobs.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/athreadpool-queue.ifc\"       /Fo\"${buildDir}/athreadpool-queue.obj\"       /c /TP \"${projectDir}/queue.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/athreadpool-monitor.ifc\"     /Fo\"${buildDir}/athreadpool-monitor.obj\"     /c /TP \"${projectDir}/monitor.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/athreadpool-threadpool.ifc\" /Fo\"${buildDir}/athreadpool-threadpool.obj\" /c /TP \"${projectDir}/threadpool.cpp\" || exit 1

cl.exe ${msvcCompilerFlags} ^
       /interface ^
       /ifcSearchDir \"${buildDir}\" ^
       /ifcOutput\"${buildDir}/athreadpool.ifc\" ^
       /Fo\"${buildDir}/athreadpool.obj\" ^
       /c \"${projectDir}/athreadpool.cpp\" || exit 1

lib.exe /NOLOGO ^
        /OUT:\"${buildDir}/athreadpool.lib\" ^
        \"${buildDir}/athreadpool.obj\" ^
        \"${buildDir}/athreadpool-job.obj\" ^
        \"${buildDir}/athreadpool-jobs.obj\" ^
        \"${buildDir}/athreadpool-queue.obj\" ^
        \"${buildDir}/athreadpool-monitor.obj\" ^
        \"${buildDir}/athreadpool-threadpool.obj\" || exit 1
"

sh/build/astl.sh "${toolchain}"
build
