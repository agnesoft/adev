source "sh/build_common.sh" "${1}"

project="abuild.cpptokenizer"
projectDir="projects/abuild/cpptokenizer"
buildDir="${buildRoot}/abuild/cpptokenizer"

function build_clang() {

    $clang $clangCompilerFlags -Xclang -emit-module-interface -o "${buildDir}/abuild.cpptokenizer.pcm" -c "${projectDir}/cpptokenizer.cpp"
    $clang $clangCompilerFlags -o "${buildDir}/abuild.cpptokenizer.obj" -c "${projectDir}/cpptokenizer.cpp"
}

function build_gcc() {
    $gcc $gccCompilerFlags -o "${buildDir}/condition.obj"              -c "${projectDir}/condition.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/token.obj"                  -c "${projectDir}/token.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/tokenizer_base.obj"         -c "${projectDir}/tokenizer_base.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/preprocessor_base.obj"      -c "${projectDir}/preprocessor_base.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/define_directive.obj"       -c "${projectDir}/define_directive.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/preprocessor_directive.obj" -c "${projectDir}/preprocessor_directive.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/tokenizer.obj"              -c "${projectDir}/tokenizer.cpp"

    $gcc $gccCompilerFlags -o "${buildDir}/cpptokenizer.obj" -c "${projectDir}/acommandline.cpp"

    ar r "${buildDir}/abuild.cpptokenizer.lib" \
         "${buildDir}/cpptokenizer.obj" \
         "${buildDir}/condition.obj" \
         "${buildDir}/token.obj" \
         "${buildDir}/tokenizer_base.obj" \
         "${buildDir}/preprocessor_base.obj" \
         "${buildDir}/define_directive.obj" \
         "${buildDir}/preprocessor_directive.obj" \
         "${buildDir}/tokenizer.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/abuild.cpptokenizer-condition.ifc\"              /Fo\"${buildDir}/abuild.cpptokenizer-condition.obj\"              /c /TP \"${projectDir}/condition.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/abuild.cpptokenizer-token.ifc\"                  /Fo\"${buildDir}/abuild.cpptokenizer-token.obj\"                  /c /TP \"${projectDir}/token.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/abuild.cpptokenizer-tokenizer_base.ifc\"         /Fo\"${buildDir}/abuild.cpptokenizer-tokenizer_base.obj\"         /c /TP \"${projectDir}/tokenizer_base.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/abuild.cpptokenizer-preprocessor_base.ifc\"      /Fo\"${buildDir}/abuild.cpptokenizer-preprocessor_base.obj\"      /c /TP \"${projectDir}/preprocessor_base.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/abuild.cpptokenizer-define_directive.ifc\"       /Fo\"${buildDir}/abuild.cpptokenizer-define_directive.obj\"       /c /TP \"${projectDir}/define_directive.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/abuild.cpptokenizer-preprocessor_directive.ifc\" /Fo\"${buildDir}/abuild.cpptokenizer-preprocessor_directive.obj\" /c /TP \"${projectDir}/preprocessor_directive.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/abuild.cpptokenizer-tokenizer.ifc\"              /Fo\"${buildDir}/abuild.cpptokenizer-tokenizer.obj\"              /c /TP \"${projectDir}/tokenizer.cpp\" || exit 1

cl.exe ${msvcCompilerFlags} ^
       /interface ^
       /ifcSearchDir \"${buildDir}\" ^
       /ifcOutput\"${buildDir}/abuild.cpptokenizer.ifc\" ^
       /Fo\"${buildDir}/abuild.cpptokenizer.obj\" ^
       /c \"${projectDir}/cpptokenizer.cpp\" || exit 1

lib.exe /NOLOGO ^
        /OUT:\"${buildDir}/abuild.cpptokenizer.lib\" ^
        \"${buildDir}/abuild.cpptokenizer.obj\" ^
        \"${buildDir}/abuild.cpptokenizer-condition.obj\" ^
        \"${buildDir}/abuild.cpptokenizer-token.obj\" ^
        \"${buildDir}/abuild.cpptokenizer-tokenizer_base.obj\" ^
        \"${buildDir}/abuild.cpptokenizer-preprocessor_base.obj\" ^
        \"${buildDir}/abuild.cpptokenizer-define_directive.obj\" ^
        \"${buildDir}/abuild.cpptokenizer-preprocessor_directive.obj\" ^
        \"${buildDir}/abuild.cpptokenizer-tokenizer.obj\" || exit 1
"

sh/build/astl.sh "${toolchain}"
build
