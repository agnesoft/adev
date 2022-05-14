source "sh/build_common.sh" 

set_build_properties "abuild.test_utilities" "${1}" "${2}"

function build_clang() {
    ${clang} ${clangCompilerFlags}                                     \
             -Xclang -emit-module-interface                            \
             -fprebuilt-module-path="${buildRoot}/abuild.cpptokenizer" \
             -o "${buildDir}/${project}.pcm"                           \
             -c "${projectDir}/${project}.cpp"
    
    ${clang} ${clangCompilerFlags}                                     \
             -fprebuilt-module-path="${buildRoot}/abuild.cpptokenizer" \
             -o "${buildDir}/${project}.obj"                           \
             -c "${projectDir}/${project}.cpp"
}

function build_gcc() {
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/test_file.obj" -c "${projectDir}/test_file.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/token_operators.obj" -c "${projectDir}/token_operators.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/test_project.obj" -c "${projectDir}/test_project.cpp"

    ${gcc} ${gccCompilerFlags} -o "${buildDir}/${project}.obj" -c "${projectDir}/${project}.cpp"

    ar r "${buildDir}/test_file.obj"       \
         "${buildDir}/${project}.obj"      \
         "${buildDir}/token_operators.obj" \
         "${buildDir}/test_project.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild.cpptokenizer\" /ifcOutput\"${buildDir}/${project}-test_file.ifc\" /Fo\"${buildDir}/${project}-test_file.obj\" /c /TP \"${projectDir}/test_file.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild.cpptokenizer\" /ifcOutput\"${buildDir}/${project}-token_operators.ifc\" /Fo\"${buildDir}/${project}-token_operators.obj\" /c /TP \"${projectDir}/token_operators.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild.cpptokenizer\" /ifcOutput\"${buildDir}/${project}-test_project.ifc\" /Fo\"${buildDir}/${project}-test_project.obj\" /c /TP \"${projectDir}/test_project.cpp\" || exit 1

cl.exe ${msvcCompilerFlags}                               ^
       /interface                                         ^
       /ifcSearchDir \"${buildDir}\"                      ^
       /ifcOutput\"${buildDir}/${project}.ifc\"           ^
       /ifcSearchDir \"${buildRoot}/abuild.cpptokenizer\" ^
       /Fo\"${buildDir}/${project}.obj\"                  ^
       /c \"${projectDir}/${project}.cpp\" || exit 1

lib.exe /NOLOGO ^
        /OUT:\"${buildDir}/${project}.lib\"         ^
        \"${buildDir}/${project}.obj\"              ^
        \"${buildDir}/${project}-test_file.obj\"    ^
        \"${buildDir}/${project}-test_project.obj\" ^
        \"${buildDir}/${project}-token_operators.obj\" || exit 1
"

build "abuild.cpptokenizer"
