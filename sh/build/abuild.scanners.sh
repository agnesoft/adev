source "sh/build_common.sh" 

set_build_properties "abuild.scanners" "${1}" "${2}"

function build_clang() {
    ${clang} ${clangCompilerFlags}                                     \
             -Xclang -emit-module-interface                            \
             -fprebuilt-module-path="${buildRoot}/abuild.cache"        \
             -fprebuilt-module-path="${buildRoot}/abuild.cpptokenizer" \
             -fprebuilt-module-path="${buildRoot}/athreadpool"         \
             -fprebuilt-module-path="${buildRoot}/yamlcpp"             \
             -o "${buildDir}/${project}.pcm"                           \
             -c "${projectDir}/${project}.cpp"
    
    ${clang} ${clangCompilerFlags}                                     \
             -fprebuilt-module-path="${buildRoot}/abuild.cache"        \
             -fprebuilt-module-path="${buildRoot}/abuild.cpptokenizer" \
             -fprebuilt-module-path="${buildRoot}/athreadpool"         \
             -fprebuilt-module-path="${buildRoot}/yamlcpp"             \
             -o "${buildDir}/${project}.obj"                           \
             -c "${projectDir}/${project}.cpp"
}

function build_gcc() {
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/project_scanner.obj" -c "${projectDir}/project_scanner.cpp"

    ${gcc} ${gccCompilerFlags} -o "${buildDir}/${project}.obj" -c "${projectDir}/${project}.cpp"

    ar r "${buildDir}/${project}.lib" \
         "${buildDir}/${project}.obj" \
         "${buildDir}/project_scanner.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild.cpptokenizer\" /ifcSearchDir \"${buildRoot}/abuild.cache\" /ifcSearchDir \"${buildRoot}/athreadpool\" /ifcOutput\"${buildDir}/${project}-project_scanner.ifc\" /Fo\"${buildDir}/${project}-project_scanner.obj\" /c /TP \"${projectDir}/project_scanner.cpp\" || exit 1

cl.exe ${msvcCompilerFlags}                               ^
       /interface                                         ^
       /ifcSearchDir \"${buildDir}\"                      ^
       /ifcSearchDir \"${buildRoot}/abuild.cache\"        ^
       /ifcSearchDir \"${buildRoot}/abuild.cpptokenizer\" ^
       /ifcSearchDir \"${buildRoot}/athreadpool\"         ^
       /ifcSearchDir \"${buildRoot}/yamlcpp\"             ^
       /ifcOutput\"${buildDir}/${project}.ifc\"           ^
       /Fo\"${buildDir}/${project}.obj\"                  ^
       /c \"${projectDir}/${project}.cpp\" || exit 1

lib.exe /NOLOGO                             ^
        /OUT:\"${buildDir}/${project}.lib\" ^
        \"${buildDir}/${project}.obj\"      ^
        \"${buildDir}/${project}-project_scanner.obj\" || exit 1
"

build "abuild.cache athreadpool"
