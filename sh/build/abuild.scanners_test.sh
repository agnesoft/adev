source "sh/build_common.sh" "${1}"

project="abuild.scanners_test"
projectDir="projects/abuild/scanners/test"
buildDir="${buildRoot}/abuild/scanners/test"

function build_clang() {
    $clang $clangCompilerLinkerFlags \
           -fprebuilt-module-path=${buildRoot}/atest \
           -fprebuilt-module-path=${buildRoot}/acommandline \
           -fprebuilt-module-path=${buildRoot}/abuild/scanners \
           -fprebuilt-module-path=${buildRoot}/abuild/cache \
           -fprebuilt-module-path=${buildRoot}/abuild/cpptokenizer \
           -fprebuilt-module-path=${buildRoot}/abuild/test_utilities \
           -fprebuilt-module-path=${buildRoot}/yamlcpp \
           -o "${binDir}/abuild.scanners_test${executableExtension}" \
           "${projectDir}/main.cpp" \
           "${projectDir}/project_scanner_test.cpp" \
           "${buildRoot}/abuild/scanners/abuild.scanners.obj" \
           "${buildRoot}/abuild/cache/abuild.cache.obj" \
           "${buildRoot}/abuild/test_utilities/abuild.test_utilities.obj" \
           "${buildRoot}/abuild/cpptokenizer/abuild.cpptokenizer.obj" \
           "${buildRoot}/atest/atest.obj" \
           "${buildRoot}/astl/astl.obj" \
           "${buildRoot}/acommandline/acommandline.obj" \
           "${buildRoot}/yamlcpp/yamlcpp.lib"
}

function build_gcc() {
    $gcc $gccCompilerFlags \
         -o "${binDir}/abuild.scanners_test" \
         "${projectDir}/main.cpp" \
         "${projectDir}/project_scanner_test.cpp" \
         "${buildRoot}/abuild/scanners/abuild.scanners.lib" \
         "${buildRoot}/abuild/cache/abuild.cache.lib" \
         "${buildRoot}/abuild/test_utilities/abuild.test_utilities.lib" \
         "${buildRoot}/abuild/cpptokenizer/abuild.cpptokenizer.lib" \
         "${buildRoot}/atest/atest.lib" \
         "${buildRoot}/astl/astl.lib" \
         "${buildRoot}/acommandline/acommandline.lib" \
         "${buildRoot}/yamlcpp/yamlcpp.lib"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} ^
       /ifcSearchDir \"${buildRoot}/atest\" ^
       /ifcSearchDir \"${buildRoot}/acommandline\" ^
       /ifcSearchDir \"${buildRoot}/abuild/cache\" ^
       /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" ^
       /ifcSearchDir \"${buildRoot}/abuild/scanners\" ^
       /ifcSearchDir \"${buildRoot}/abuild/test_utilities\" ^
       /ifcSearchDir \"${buildRoot}/athreadpool\" ^
       /ifcSearchDir \"${buildRoot}/yamlcpp\" ^
       /Fo\"$buildDir/\" ^
       /Fe\"${binDir}/abuild.scanners_test.exe\" ^
       \"${projectDir}/main.cpp\" ^
       \"${projectDir}/project_scanner_test.cpp\" ^
       \"${buildRoot}/atest/atest.lib\" ^
       \"${buildRoot}/astl/astl.lib\" ^
       \"${buildRoot}/acommandline/acommandline.lib\" ^
       \"${buildRoot}/abuild/cache/abuild.cache.lib\" ^
       \"${buildRoot}/abuild/scanners/abuild.scanners.lib\" ^
       \"${buildRoot}/abuild/test_utilities/abuild.test_utilities.lib\" ^
       \"${buildRoot}/abuild/cpptokenizer/abuild.cpptokenizer.lib\" ^
       \"${buildRoot}/yamlcpp/yamlcpp.lib\" ^
       \"${buildRoot}/athreadpool/athreadpool.lib\" || exit 1
"

sh/build/abuild.scanners.sh "${toolchain}"
sh/build/abuild.test_utilities.sh "${toolchain}"
sh/build/atest.sh "${toolchain}"
build
