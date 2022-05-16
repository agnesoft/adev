source "sh/build_common.sh" 

set_build_properties "aprocesstestapp" "${1}" "${2}"

function build_clang() {
    ${clang} ${clangCompilerLinkerFlags}                      \
           -fprebuilt-module-path=${buildRoot}/aprocess       \
           -fprebuilt-module-path=${buildRoot}/acommandline   \
           -fprebuilt-module-path=${buildRoot}/awinapi        \
           -o "${binDir}/${project}${executableExtension}"    \
           "${projectDir}/main.cpp"                           \
           "${buildRoot}/aprocess/aprocess.obj"               \
           "${buildRoot}/acommandline/acommandline.obj"       \
           "${buildRoot}/awinapi/awinapi.obj"                 \
           "${buildRoot}/astl/astl.obj"
}

function build_gcc() {
    ${gcc} ${gccCompilerFlags}                          \
           -o "${binDir}/${project}"                    \
           "${projectDir}/main.cpp"                     \
           "${buildRoot}/aprocess/aprocess.lib"         \
           "${buildRoot}/acommandline/acommandline.obj" \
           "${buildRoot}/astl/astl.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags}                           ^
       /ifcSearchDir \"${buildRoot}/acommandline\"    ^
       /ifcSearchDir \"${buildRoot}/aprocess\"        ^
       /ifcSearchDir \"${buildRoot}/awinapi\"         ^
       /Fo\"$buildDir/\"                              ^
       /Fe\"${binDir}/${project}.exe\"                ^
       \"${projectDir}/main.cpp\"                     ^
       \"${buildRoot}/astl/astl.lib\"                 ^
       \"${buildRoot}/acommandline/acommandline.lib\" ^
       \"${buildRoot}/aprocess/aprocess.lib\"         ^
       \"${buildRoot}/awinapi/awinapi.lib\" || exit 1
"

build "acommandline aprocess"
