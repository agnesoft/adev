source "sh/build_common.sh" 

set_build_properties "abuild.cpptokenizer" "${1}" "${2}"

function build_clang() {
    ${clang} ${clangCompilerFlags}           \
             -Xclang -emit-module-interface  \
             -o "${buildDir}/${project}.pcm" \
             -c "${projectDir}/${project}.cpp"

    ${clang} ${clangCompilerFlags}           \
             -o "${buildDir}/${project}.obj" \
             -c "${projectDir}/${project}.cpp"
}

function build_gcc() {
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/if_token.obj"                      -c "${projectDir}/if_token.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/token.obj"                         -c "${projectDir}/token.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/token_operators.obj"               -c "${projectDir}/token_operators.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/tokenizer_common.obj"              -c "${projectDir}/tokenizer_common.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/preprocessor_tokenizer_common.obj" -c "${projectDir}/preprocessor_tokenizer_common.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/preprocessor_if_tokenizer.obj"     -c "${projectDir}/preprocessor_if_tokenizer.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/preprocessor_tokenizer.obj"        -c "${projectDir}/preprocessor_tokenizer.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/tokenizer.obj"                     -c "${projectDir}/tokenizer.cpp"

    ${gcc} ${gccCompilerFlags} -o "${buildDir}/${project}.obj" -c "${projectDir}/${project}.cpp"

    ar r "${buildDir}/${project}.lib"                    \
         "${buildDir}/${project}.obj"                    \
         "${buildDir}/if_token.obj"                      \
         "${buildDir}/token.obj"                         \
         "${buildDir}/token_operators.obj"               \
         "${buildDir}/tokenizer_common.obj"              \
         "${buildDir}/preprocessor_tokenizer_common.obj" \
         "${buildDir}/preprocessor_if_tokenizer.obj"     \
         "${buildDir}/preprocessor_tokenizer.obj"        \
         "${buildDir}/tokenizer.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-if_token.ifc\"                      /Fo\"${buildDir}/${project}-if_token.obj\"                      /c /TP \"${projectDir}/if_token.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-token.ifc\"                         /Fo\"${buildDir}/${project}-token.obj\"                         /c /TP \"${projectDir}/token.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-token_operators.ifc\"               /Fo\"${buildDir}/${project}-token_operators.obj\"               /c /TP \"${projectDir}/token_operators.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-tokenizer_common.ifc\"              /Fo\"${buildDir}/${project}-tokenizer_common.obj\"              /c /TP \"${projectDir}/tokenizer_common.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-preprocessor_tokenizer_common.ifc\" /Fo\"${buildDir}/${project}-preprocessor_tokenizer_common.obj\" /c /TP \"${projectDir}/preprocessor_tokenizer_common.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-preprocessor_if_tokenizer.ifc\"     /Fo\"${buildDir}/${project}-preprocessor_if_tokenizer.obj\"     /c /TP \"${projectDir}/preprocessor_if_tokenizer.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-preprocessor_tokenizer.ifc\"        /Fo\"${buildDir}/${project}-preprocessor_tokenizer.obj\"        /c /TP \"${projectDir}/preprocessor_tokenizer.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-tokenizer.ifc\"                     /Fo\"${buildDir}/${project}-tokenizer.obj\"                     /c /TP \"${projectDir}/tokenizer.cpp\" || exit 1

cl.exe ${msvcCompilerFlags}                     ^
       /interface                               ^
       /ifcSearchDir \"${buildDir}\"            ^
       /ifcOutput\"${buildDir}/${project}.ifc\" ^
       /Fo\"${buildDir}/${project}.obj\"        ^
       /c \"${projectDir}/${project}.cpp\" || exit 1

lib.exe /NOLOGO ^
        /OUT:\"${buildDir}/${project}.lib\"                          ^
        \"${buildDir}/${project}.obj\"                               ^
        \"${buildDir}/${project}-if_token.obj\"                      ^
        \"${buildDir}/${project}-token.obj\"                         ^
        \"${buildDir}/${project}-token_operators.obj\"               ^
        \"${buildDir}/${project}-tokenizer_common.obj\"              ^
        \"${buildDir}/${project}-preprocessor_tokenizer_common.obj\" ^
        \"${buildDir}/${project}-preprocessor_if_tokenizer.obj\"     ^
        \"${buildDir}/${project}-preprocessor_tokenizer.obj\"        ^
        \"${buildDir}/${project}-tokenizer.obj\" || exit 1
"

build "astl"
