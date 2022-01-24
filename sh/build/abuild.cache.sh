source "sh/build_common.sh" "${1}"

project="abuild.cache"
projectDir="projects/abuild/cache"
buildDir="${buildRoot}/abuild/cache"

function build_clang() {
    $clang $clangCompilerFlags \
           -Xclang -emit-module-interface \
           -fprebuilt-module-path=${buildRoot}/abuild/cpptokenizer \
           -o "${buildDir}/abuild.cache.pcm" \
           -c "${projectDir}/cache.cpp"
    
    $clang $clangCompilerFlags \
           -fprebuilt-module-path=${buildRoot}/abuild/cpptokenizer \
           -o "${buildDir}/abuild.cache.obj" \
           -c "${projectDir}/cache.cpp"
}

function build_gcc() {
    $gcc $gccCompilerFlags -o "${buildDir}/file.obj"             -c "${projectDir}/file.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/source_file_base.obj" -c "${projectDir}/source_file_base.cpp"

    $gcc $gccCompilerFlags -o "${buildDir}/cache.obj" -c "${projectDir}/cache.cpp"

    ar r "${buildDir}/cache.obj" \
         "${buildDir}/file.obj" \
         "${buildDir}/source_file_base.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-file.ifc\"             /Fo\"${buildDir}/abuild.cache-file.obj\"             /c /TP \"${projectDir}/file.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-source_file_base.ifc\" /Fo\"${buildDir}/abuild.cache-source_file_base.obj\" /c /TP \"${projectDir}/source_file_base.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-source_file.ifc\"      /Fo\"${buildDir}/abuild.cache-source_file.obj\"      /c /TP \"${projectDir}/source_file.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-header_file.ifc\"      /Fo\"${buildDir}/abuild.cache-header_file.obj\"      /c /TP \"${projectDir}/header_file.cpp\" || exit 1

cl.exe ${msvcCompilerFlags} ^
       /interface ^
       /ifcSearchDir \"${buildDir}\" ^
       /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" ^
       /ifcOutput\"${buildDir}/abuild.cache.ifc\" ^
       /Fo\"${buildDir}/abuild.cache.obj\" ^
       /c \"${projectDir}/cache.cpp\" || exit 1

lib.exe /NOLOGO ^
        /OUT:\"${buildDir}/abuild.cache.lib\" ^
        \"${buildDir}/abuild.cache.obj\" ^
        \"${buildDir}/abuild.cache-file.obj\" ^
        \"${buildDir}/abuild.cache-source_file_base.obj\" ^
        \"${buildDir}/abuild.cache-source_file.obj\" ^
        \"${buildDir}/abuild.cache-header_file.obj\"  || exit 1
"

sh/build/astl.sh "${toolchain}"
sh/build/abuild.cpptokenizer.sh "${toolchain}"
build