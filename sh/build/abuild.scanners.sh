source "sh/build_common.sh" "${1}"

project="abuild.scanners"
projectDir="projects/abuild/scanners"
buildDir="${buildRoot}/abuild/scanners"

function build_clang() {
    $clang $clangCompilerFlags \
           -Xclang -emit-module-interface \
           -fprebuilt-module-path=${buildRoot}/abuild/cache \
           -fprebuilt-module-path=${buildRoot}/abuild/cpptokenizer \
           -fprebuilt-module-path=${buildRoot}/athreadpool \
           -fprebuilt-module-path=${buildRoot}/yamlcpp \
           -o "${buildDir}/abuild.scanners.pcm" \
           -c "${projectDir}/scanners.cpp"
    
    $clang $clangCompilerFlags \
           -fprebuilt-module-path=${buildRoot}/abuild/cache \
           -fprebuilt-module-path=${buildRoot}/abuild/cpptokenizer \
           -fprebuilt-module-path=${buildRoot}/athreadpool \
           -fprebuilt-module-path=${buildRoot}/yamlcpp \
           -o "${buildDir}/abuild.scanners.obj" \
           -c "${projectDir}/scanners.cpp"
}

function build_gcc() {
    $gcc $gccCompilerFlags -o "${buildDir}/project_scanner.obj" -c "${projectDir}/project_scanner.cpp"

    $gcc $gccCompilerFlags -o "${buildDir}/scanners.obj" -c "${projectDir}/scanners.cpp"

    ar r "${buildDir}/abuild.scanners.lib" \
         "${buildDir}/scanners.obj" \
         "${buildDir}/project_scanner.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcSearchDir \"${buildRoot}/abuild/cache\" /ifcSearchDir \"${buildRoot}/athreadpool\" /ifcOutput\"${buildDir}/abuild.scanners-project_scanner.ifc\" /Fo\"${buildDir}/abuild.scanners-project_scanner.obj\" /c /TP \"${projectDir}/project_scanner.cpp\" || exit 1

cl.exe ${msvcCompilerFlags} ^
       /interface ^
       /ifcSearchDir \"${buildDir}\" ^
       /ifcSearchDir \"${buildRoot}/abuild/cache\" ^
       /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" ^
       /ifcSearchDir \"${buildRoot}/athreadpool\" ^
       /ifcSearchDir \"${buildRoot}/yamlcpp\" ^
       /ifcOutput\"${buildDir}/abuild.scanners.ifc\" ^
       /Fo\"${buildDir}/abuild.scanners.obj\" ^
       /c \"${projectDir}/scanners.cpp\" || exit 1

lib.exe /NOLOGO ^
        /OUT:\"${buildDir}/abuild.scanners.lib\" ^
        \"${buildDir}/abuild.scanners.obj\" ^
        \"${buildDir}/abuild.scanners-project_scanner.obj\" || exit 1
"

sh/build/abuild.cache.sh "${toolchain}"
sh/build/athreadpool.sh "${toolchain}"
build
