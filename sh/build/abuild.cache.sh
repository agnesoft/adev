source "sh/build_common.sh" "${1}"

project="abuild.cache"
projectDir="projects/abuild/cache"
buildDir="${buildRoot}/abuild/cache"

function build_clang() {

    $clang $clangCompilerFlags -Xclang -emit-module-interface -o "${buildDir}/abuild.cache.pcm" -c "${projectDir}/cache.cpp"
    $clang $clangCompilerFlags -o "${buildDir}/abuild.cache.obj" -c "${projectDir}/cache.cpp"
}

function build_gcc() {
    $gcc $gccCompilerFlags -o "${buildDir}/file.obj" -c "${projectDir}/file.cpp"

    $gcc $gccCompilerFlags -o "${buildDir}/cache.obj" -c "${projectDir}/cache.cpp"

    ar r "${buildDir}/file.obj" \
         "${buildDir}/cache.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/abuild.cache-file.ifc\" /Fo\"${buildDir}/abuild.cache-file.obj\" /c /TP \"${projectDir}/file.cpp\" || exit 1


cl.exe ${msvcCompilerFlags} ^
       /interface ^
       /ifcSearchDir \"${buildDir}\" ^
       /ifcOutput\"${buildDir}/abuild.cache.ifc\" ^
       /Fo\"${buildDir}/abuild.cache.obj\" ^
       /c \"${projectDir}/cache.cpp\" || exit 1

lib.exe /NOLOGO ^
        /OUT:\"${buildDir}/abuild.cache.lib\" ^
        \"${buildDir}/abuild.cache.obj\" ^
        \"${buildDir}/abuild.cache-file.obj\" || exit 1
"

sh/build/astl.sh "${toolchain}"
build
