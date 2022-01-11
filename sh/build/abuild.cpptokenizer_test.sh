source "sh/build_common.sh" "${1}"

project="abuild.cpptokenizer_test"
projectDir="projects/abuild/cpptokenizer/test"
buildDir="${buildRoot}/abuild/cpptokenizer/test"

function build_clang() {
    $clang $clangCompilerLinkerFlags \
           -fprebuilt-module-path=${buildRoot}/atest \
           -fprebuilt-module-path=${buildRoot}/acommandline \
           -fprebuilt-module-path=${buildRoot}/abuild/cpptokenizer \
           -o "${binDir}/abuild.cpptokenizer_test${executableExtension}" \
           "${projectDir}/define_test.cpp" \
           "${projectDir}/if_bracket_test.cpp" \
           "${projectDir}/if_defined_test.cpp" \
           "${projectDir}/if_else_test.cpp" \
           "${projectDir}/if_has_include_test.cpp" \
           "${projectDir}/if_test.cpp" \
           "${projectDir}/ifdef_test.cpp" \
           "${projectDir}/import_include_external_test.cpp" \
           "${projectDir}/import_include_local_test.cpp" \
           "${projectDir}/import_module_test.cpp" \
           "${projectDir}/include_external_test.cpp" \
           "${projectDir}/include_local_test.cpp" \
           "${projectDir}/main.cpp" \
           "${projectDir}/module_test.cpp" \
           "${projectDir}/module_partition_test.cpp" \
           "${projectDir}/tokenizer_test.cpp" \
           "${projectDir}/undef_test.cpp" \
           "${buildRoot}/abuild/cpptokenizer/abuild.cpptokenizer.obj" \
           "${buildRoot}/atest/atest.obj" \
           "${buildRoot}/astl/astl.obj" \
           "${buildRoot}/acommandline/acommandline.obj"
}

function build_gcc() {
    $gcc $gccCompilerFlags \
         -o "${binDir}/acommandline_test" \
         "${projectDir}/define_test.cpp" \
         "${projectDir}/if_bracket_test.cpp" \
         "${projectDir}/if_defined_test.cpp" \
         "${projectDir}/if_else_test.cpp" \
         "${projectDir}/if_has_include_test.cpp" \
         "${projectDir}/if_test.cpp" \
         "${projectDir}/ifdef_test.cpp" \
         "${projectDir}/import_include_external_test.cpp" \
         "${projectDir}/import_include_local_test.cpp" \
         "${projectDir}/import_module_test.cpp" \
         "${projectDir}/include_external_test.cpp" \
         "${projectDir}/include_local_test.cpp" \
         "${projectDir}/main.cpp" \
         "${projectDir}/module_test.cpp" \
         "${projectDir}/module_partition_test.cpp" \
         "${projectDir}/tokenizer_test.cpp" \
         "${projectDir}/undef_test.cpp" \
         "${buildRoot}/abuild/cpptokenizer/abuild.cpptokenizer.lib" \
         "${buildRoot}/atest/atest.lib" \
         "${buildRoot}/astl/astl.lib" \
         "${buildRoot}/acommandline/acommandline.lib"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} ^
       /ifcSearchDir \"${buildRoot}/atest\" ^
       /ifcSearchDir \"${buildRoot}/acommandline\" ^
       /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" ^
       /Fo\"$buildDir/\" ^
       /Fe\"${binDir}/abuild.cpptokenizer_test.exe\" ^
       \"${projectDir}/define_test.cpp\" ^
       \"${projectDir}/if_bracket_test.cpp\" ^
       \"${projectDir}/if_defined_test.cpp\" ^
       \"${projectDir}/if_else_test.cpp\" ^
       \"${projectDir}/if_has_include_test.cpp\" ^
       \"${projectDir}/if_test.cpp\" ^
       \"${projectDir}/import_include_external_test.cpp\" ^
       \"${projectDir}/import_include_local_test.cpp\" ^
       \"${projectDir}/import_module_test.cpp\" ^
       \"${projectDir}/ifdef_test.cpp\" ^
       \"${projectDir}/include_external_test.cpp\" ^
       \"${projectDir}/include_local_test.cpp\" ^
       \"${projectDir}/main.cpp\" ^
       \"${projectDir}/module_test.cpp\" ^
       \"${projectDir}/module_partition_test.cpp\" ^
       \"${projectDir}/tokenizer_test.cpp\" ^
       \"${projectDir}/undef_test.cpp\" ^
       \"${buildRoot}/atest/atest.lib\" ^
       \"${buildRoot}/astl/astl.lib\" ^
       \"${buildRoot}/acommandline/acommandline.lib\" ^
       \"${buildRoot}/abuild/cpptokenizer/abuild.cpptokenizer.lib\" || exit 1
"

sh/build/abuild.cpptokenizer.sh "${toolchain}"
sh/build/atest.sh "${toolchain}"
build
