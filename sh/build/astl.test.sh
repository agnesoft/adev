source "sh/build_common.sh" 

set_build_properties "astl.test" "${1}" "${2}"

function build_clang() {
    ${clang} ${clangCompilerLinkerFlags}                     \
             -fprebuilt-module-path=${buildRoot}/atest       \
             -o "${binDir}/${project}${executableExtension}" \
             "${projectDir}/main.cpp"                        \
             "${projectDir}/source_location_test.cpp"        \
             "${buildRoot}/atest/atest.obj"                  \
             "${buildRoot}/acommandline/acommandline.obj"    \
             "${buildRoot}/astl/astl.obj"
}

function build_gcc() {
    ${gcc} ${gccCompilerFlags}                          \
           -o "${binDir}/${project}"                    \
           "${projectDir}/main.cpp"                     \
           "${projectDir}/source_location_test.cpp"     \
           "${buildRoot}/atest/atest.lib"               \
           "${buildRoot}/acommandline/acommandline.lib" \
           "${buildRoot}/astl/astl.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags}                           ^
       /ifcSearchDir \"${buildRoot}/atest\"           ^
       /Fo\"$buildDir/\"                              ^
       /Fe\"${binDir}/${project}.exe\"                ^
       \"${projectDir}/main.cpp\"                     ^
       \"${projectDir}/source_location_test.cpp\"     ^
       \"${buildRoot}/atest/atest.lib\"               ^
       \"${buildRoot}/acommandline/acommandline.lib\" ^
       \"${buildRoot}/astl/astl.lib\" || exit 1
"

build "atest"
