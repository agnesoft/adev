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
           -fprebuilt-module-path=${buildRoot}/abuild/test_utilities \
           -fprebuilt-module-path=${buildRoot}/yamlcpp \
           -o "${binDir}/abuild.cache_test${executableExtension}" \
           "${projectDir}/cache_test.cpp" \
           "${projectDir}/defines_test.cpp" \
           "${projectDir}/file_lookup_test.cpp" \
           "${projectDir}/flag_test.cpp" \
           "${projectDir}/header_file_test.cpp" \
           "${projectDir}/header_test.cpp" \
           "${projectDir}/header_unit_test.cpp" \
           "${projectDir}/main.cpp" \
           "${projectDir}/module_partition_test.cpp" \
           "${projectDir}/module_test.cpp" \
           "${projectDir}/project_test.cpp" \
           "${projectDir}/settings_test.cpp" \
           "${projectDir}/source_file_test.cpp" \
           "${projectDir}/tokens_test.cpp" \
           "${projectDir}/toolchain_test.cpp" \
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
         -o "${binDir}/abuild.cache_test" \
         "${projectDir}/cache_test.cpp" \
         "${projectDir}/defines_test.cpp" \
         "${projectDir}/file_lookup_test.cpp" \
         "${projectDir}/flag_test.cpp" \
         "${projectDir}/header_file_test.cpp" \
         "${projectDir}/header_test.cpp" \
         "${projectDir}/header_unit_test.cpp" \
         "${projectDir}/main.cpp" \
         "${projectDir}/module_partition_test.cpp" \
         "${projectDir}/module_test.cpp" \
         "${projectDir}/project_test.cpp" \
         "${projectDir}/settings_test.cpp" \
         "${projectDir}/source_file_test.cpp" 
         "${projectDir}/tokens_test.cpp" \
         "${projectDir}/toolchain_test.cpp" \
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
       /ifcSearchDir \"${buildRoot}/abuild/test_utilities\" ^
       /ifcSearchDir \"${buildRoot}/yamlcpp\" ^
       /Fo\"$buildDir/\" ^
       /Fe\"${binDir}/abuild.cache_test.exe\" ^
       \"${projectDir}/cache_test.cpp\" ^
       \"${projectDir}/defines_test.cpp\" ^
       \"${projectDir}/file_lookup_test.cpp\" ^
       \"${projectDir}/flag_test.cpp\" ^
       \"${projectDir}/header_file_test.cpp\" ^
       \"${projectDir}/header_test.cpp\" ^
       \"${projectDir}/header_unit_test.cpp\" ^
       \"${projectDir}/main.cpp\" ^
       \"${projectDir}/module_partition_test.cpp\" ^
       \"${projectDir}/module_test.cpp\" ^
       \"${projectDir}/project_test.cpp\" ^
       \"${projectDir}/settings_test.cpp\" ^
       \"${projectDir}/source_file_test.cpp\" ^
       \"${projectDir}/tokens_test.cpp\" ^
       \"${projectDir}/toolchain_test.cpp\" ^
       \"${buildRoot}/atest/atest.lib\" ^
       \"${buildRoot}/astl/astl.lib\" ^
       \"${buildRoot}/acommandline/acommandline.lib\" ^
       \"${buildRoot}/abuild/cache/abuild.cache.lib\" ^
       \"${buildRoot}/abuild/test_utilities/abuild.test_utilities.lib\" ^
       \"${buildRoot}/abuild/cpptokenizer/abuild.cpptokenizer.lib\" ^
       \"${buildRoot}/yamlcpp/yamlcpp.lib\" || exit 1
"

sh/build/abuild.cache.sh "${toolchain}"
sh/build/abuild.test_utilities.sh "${toolchain}"
sh/build/atest.sh "${toolchain}"
build
