source "sh/build_common.sh" 

set_build_properties "awinapi.test" "${1}" "${2}"

function build_clang() {
    ${clang} ${clangCompilerLinkerFlags} \
             -fprebuilt-module-path="${buildRoot}/atest"     \
             -fprebuilt-module-path="${buildRoot}/awinapi"   \
             -o "${binDir}/${project}${executableExtension}" \
             "${projectDir}/main.cpp"                        \
             "${projectDir}/buffer_test.cpp"                 \
             "${projectDir}/handle_test.cpp"                 \
             "${projectDir}/last_error_message_test.cpp"     \
             "${projectDir}/pipe_test.cpp"                   \
             "${buildRoot}/awinapi/awinapi.obj"              \
             "${buildRoot}/acommandline/acommandline.obj"    \
             "${buildRoot}/atest/atest.obj"                  \
             "${buildRoot}/astl/astl.obj"
}

function build_gcc() {
    ${gcc} ${gccCompilerFlags}                          \
           -o "${binDir}/${project}"                    \
           "${projectDir}/main.cpp"                     \
           "${projectDir}/buffer_test.cpp"              \
           "${projectDir}/handle_test.cpp"              \
           "${projectDir}/last_error_message_test.cpp"  \
           "${projectDir}/pipe_test.cpp"                \
           "${buildRoot}/awinapi/awinapi.lib"           \
           "${buildRoot}/acommandline/acommandline.obj" \
           "${buildRoot}/atest/atest.lib"               \
           "${buildRoot}/astl/astl.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags}                           ^
       /ifcSearchDir \"${buildRoot}/atest\"           ^
       /ifcSearchDir \"${buildRoot}/awinapi\"         ^
       /Fo\"$buildDir/\"                              ^
       /Fe\"${binDir}/${project}.exe\"                ^
       \"${projectDir}/main.cpp\"                     ^
       \"${projectDir}/buffer_test.cpp\"              ^
       \"${projectDir}/handle_test.cpp\"              ^
       \"${projectDir}/last_error_message_test.cpp\"  ^
       \"${projectDir}/pipe_test.cpp\"                ^
       \"${buildRoot}/atest/atest.lib\"               ^
       \"${buildRoot}/astl/astl.lib\"                 ^
       \"${buildRoot}/acommandline/acommandline.lib\" ^
       \"${buildRoot}/awinapi/awinapi.lib\" || exit 1
"

build "awinapi atest"
