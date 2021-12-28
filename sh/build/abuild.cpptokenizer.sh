source "sh/build_common.sh" "${1}"

project="abuild.cpptokenizer"
projectDir="projects/abuild/cpptokenizer"
buildDir="${buildRoot}/abuild/cpptokenizer"

function build_clang() {

    $clang $clangCompilerFlags -Xclang -emit-module-interface -o "${buildDir}/abuild.cpptokenizer.pcm" -c "${projectDir}/cpptokenizer.cpp"
    $clang $clangCompilerFlags -o "${buildDir}/abuild.cpptokenizer.obj" -c "${projectDir}/cpptokenizer.cpp"
}

function build_gcc() {
    $gcc $gccCompilerFlags -o "${buildDir}/if_token.obj"                      -c "${projectDir}/if_token.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/token.obj"                         -c "${projectDir}/token.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/tokenizer_common.obj"              -c "${projectDir}/tokenizer_common.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/preprocessor_tokenizer_common.obj" -c "${projectDir}/preprocessor_tokenizer_common.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/preprocessor_tokenizer.obj"        -c "${projectDir}/preprocessor_tokenizer.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/tokenizer.obj"                     -c "${projectDir}/tokenizer.cpp"

    $gcc $gccCompilerFlags -o "${buildDir}/cpptokenizer.obj" -c "${projectDir}/acommandline.cpp"

    ar r "${buildDir}/abuild.cpptokenizer.lib" \
         "${buildDir}/cpptokenizer.obj" \
         "${buildDir}/if_token.obj" \
         "${buildDir}/token.obj" \
         "${buildDir}/tokenizer_common.obj" \
         "${buildDir}/preprocessor_tokenizer_common.obj" \
         "${buildDir}/preprocessor_tokenizer.obj" \
         "${buildDir}/tokenizer.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/abuild.cpptokenizer-if_token.ifc\"                      /Fo\"${buildDir}/abuild.cpptokenizer-if_token.obj\"                      /c /TP \"${projectDir}/if_token.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/abuild.cpptokenizer-token.ifc\"                         /Fo\"${buildDir}/abuild.cpptokenizer-token.obj\"                         /c /TP \"${projectDir}/token.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/abuild.cpptokenizer-tokenizer_common.ifc\"              /Fo\"${buildDir}/abuild.cpptokenizer-tokenizer_common.obj\"              /c /TP \"${projectDir}/tokenizer_common.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/abuild.cpptokenizer-preprocessor_tokenizer_common.ifc\" /Fo\"${buildDir}/abuild.cpptokenizer-preprocessor_tokenizer_common.obj\" /c /TP \"${projectDir}/preprocessor_tokenizer_common.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/abuild.cpptokenizer-preprocessor_tokenizer.ifc\"        /Fo\"${buildDir}/abuild.cpptokenizer-preprocessor_tokenizer.obj\"        /c /TP \"${projectDir}/preprocessor_tokenizer.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/abuild.cpptokenizer-tokenizer.ifc\"                     /Fo\"${buildDir}/abuild.cpptokenizer-tokenizer.obj\"                     /c /TP \"${projectDir}/tokenizer.cpp\" || exit 1

cl.exe ${msvcCompilerFlags} ^
       /interface ^
       /ifcSearchDir \"${buildDir}\" ^
       /ifcOutput\"${buildDir}/abuild.cpptokenizer.ifc\" ^
       /Fo\"${buildDir}/abuild.cpptokenizer.obj\" ^
       /c \"${projectDir}/cpptokenizer.cpp\" || exit 1

lib.exe /NOLOGO ^
        /OUT:\"${buildDir}/abuild.cpptokenizer.lib\" ^
        \"${buildDir}/abuild.cpptokenizer.obj\" ^
        \"${buildDir}/abuild.cpptokenizer-if_token.obj\" ^
        \"${buildDir}/abuild.cpptokenizer-token.obj\" ^
        \"${buildDir}/abuild.cpptokenizer-tokenizer_common.obj\" ^
        \"${buildDir}/abuild.cpptokenizer-preprocessor_tokenizer_common.obj\" ^
        \"${buildDir}/abuild.cpptokenizer-preprocessor_tokenizer.obj\" ^
        \"${buildDir}/abuild.cpptokenizer-tokenizer.obj\" || exit 1
"

sh/build/astl.sh "${toolchain}"
build
