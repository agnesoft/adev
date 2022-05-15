source "sh/build_common.sh" 

set_build_properties "athreadpool.test" "${1}" "${2}"

function build_clang() {
    ${clang} ${clangCompilerLinkerFlags}                      \
           -fprebuilt-module-path=${buildRoot}/atest          \
           -fprebuilt-module-path=${buildRoot}/acommandline   \
           -fprebuilt-module-path=${buildRoot}/athreadpool    \
           -o "${binDir}/${project}${executableExtension}"    \
           "${projectDir}/main.cpp"                           \
           "${projectDir}/threadpool_test.cpp"                \
           "${buildRoot}/acommandline/acommandline.obj"       \
           "${buildRoot}/athreadpool/athreadpool.obj"         \
           "${buildRoot}/atest/atest.obj"                     \
           "${buildRoot}/astl/astl.obj"
}

function build_gcc() {
    ${gcc} ${gccCompilerFlags}                        \
         -o "${binDir}/${project}"                    \
         "${projectDir}/main.cpp"                     \
         "${projectDir}/threadpool_test.cpp"          \
         "${buildRoot}/acommandline/acommandline.lib" \
         "${buildRoot}/athreadpool/athreadpool.lib"   \
         "${buildRoot}/atest/atest.lib"               \
         "${buildRoot}/astl/astl.lib"
}

buildMSVC="
cl.exe ${msvcCompilerFlags}                         ^
       /ifcSearchDir \"${buildRoot}/atest\"         ^
       /ifcSearchDir \"${buildRoot}/acommandline\"  ^
       /ifcSearchDir \"${buildRoot}/athreadpool\"   ^
       /Fo\"$buildDir/\"                            ^
       /Fe\"${binDir}/${project}.exe\"              ^
       \"${projectDir}/main.cpp\"                   ^
       \"${projectDir}/threadpool_test.cpp\"        ^
       \"${buildRoot}/athreadpool/athreadpool.lib\" ^
       \"${buildRoot}/atest/atest.lib\"             ^
       \"${buildRoot}/astl/astl.lib\"               ^
       \"${buildRoot}/acommandline/acommandline.lib\" || exit 1
"

build "athreadpool atest"
