source "sh/build_common.sh" "${1}"

project="abuild.cache_test"
projectDir="projects/abuild/cache/test"
buildDir="${buildRoot}/abuild/cache/test"

function build_clang() {
    $clang $clangCompilerLinkerFlags \
           -fprebuilt-module-path=${buildRoot}/atest \
           -fprebuilt-module-path=${buildRoot}/acommandline \
           -fprebuilt-module-path=${buildRoot}/abuild/cache \
           -fprebuilt-module-path=${buildRoot}/abuild/cpptokenizer \
           -o "${binDir}/abuild.cache_test${executableExtension}" \
           "${projectDir}/file_test.cpp" \
           "${projectDir}/main.cpp" \
           "${projectDir}/source_file_base_test.cpp" \
           "${buildRoot}/abuild/cache/abuild.cache.obj" \
           "${buildRoot}/atest/atest.obj" \
           "${buildRoot}/astl/astl.obj" \
           "${buildRoot}/acommandline/acommandline.obj"
}

function build_gcc() {
    $gcc $gccCompilerFlags \
         -o "${binDir}/abuild.cache_test" \
         "${projectDir}/file_test.cpp" \
         "${projectDir}/main.cpp" \
         "${projectDir}/source_file_base_test.cpp" \
         "${buildRoot}/abuild/cache/abuild.cache.lib" \
         "${buildRoot}/atest/atest.lib" \
         "${buildRoot}/astl/astl.lib" \
         "${buildRoot}/acommandline/acommandline.lib"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} ^
       /ifcSearchDir \"${buildRoot}/atest\" ^
       /ifcSearchDir \"${buildRoot}/acommandline\" ^
       /ifcSearchDir \"${buildRoot}/abuild/cache\" ^
       /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" ^
       /Fo\"$buildDir/\" ^
       /Fe\"${binDir}/abuild.cache_test.exe\" ^
       \"${projectDir}/file_test.cpp\" ^
       \"${projectDir}/main.cpp\" ^
       \"${projectDir}/source_file_base_test.cpp\" ^
       \"${buildRoot}/atest/atest.lib\" ^
       \"${buildRoot}/astl/astl.lib\" ^
       \"${buildRoot}/acommandline/acommandline.lib\" ^
       \"${buildRoot}/abuild/cache/abuild.cache.lib\" || exit 1
"

sh/build/abuild.cache.sh "${toolchain}"
sh/build/atest.sh "${toolchain}"
build
