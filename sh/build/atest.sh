source "sh/build_common.sh" 

set_build_properties "atest" "${1}" "${2}"

function build_clang() {
    ${clang} ${clangCompilerFlags}                              \
             -Xclang                                            \
             -emit-module-interface                             \
             -fprebuilt-module-path=${buildRoot}/acommandline   \
             -o "${buildDir}/${project}.pcm"                    \
             -c "${projectDir}/${project}.cpp"

    ${clang} ${clangCompilerFlags}                              \
             -fprebuilt-module-path=${buildRoot}/acommandline   \
             -o "${buildDir}/${project}.obj"                    \
             -c "${projectDir}/${project}.cpp"
}

function build_gcc() {
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/stringify.obj"        -c "${projectDir}/stringify.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/failed_assertion.obj" -c "${projectDir}/failed_assertion.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/failure.obj"          -c "${projectDir}/failure.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/test.obj"             -c "${projectDir}/test.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/test_suite.obj"       -c "${projectDir}/test_suite.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/test_context.obj"     -c "${projectDir}/test_context.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/matcher_base.obj"     -c "${projectDir}/matcher_base.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/matcher.obj"          -c "${projectDir}/matcher.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/matcher_contains.obj" -c "${projectDir}/matcher_contains.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/expect_base.obj"      -c "${projectDir}/expect_base.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/expect_to_match.obj"  -c "${projectDir}/expect_to_match.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/expect_to_throw.obj"  -c "${projectDir}/expect_to_throw.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/expect.obj"           -c "${projectDir}/expect.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/stats.obj"            -c "${projectDir}/stats.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/results.obj"          -c "${projectDir}/results.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/filters.obj"          -c "${projectDir}/filters.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/test_filter.obj"      -c "${projectDir}/test_filter.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/selected_tests.obj"   -c "${projectDir}/selected_tests.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/reporter.obj"         -c "${projectDir}/reporter.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/printer.obj"          -c "${projectDir}/printer.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/test_runner.obj"      -c "${projectDir}/test_runner.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/${project}.obj"            -c "${projectDir}/${project}.cpp"

    ar r "${buildDir}/${project}.lib"       \
         "${buildDir}/${project}.obj"       \
         "${buildDir}/stringify.obj"        \
         "${buildDir}/failed_assertion.obj" \
         "${buildDir}/failure.obj"          \
         "${buildDir}/test.obj"             \
         "${buildDir}/test_suite.obj"       \
         "${buildDir}/test_context.obj"     \
         "${buildDir}/matcher_base.obj"     \
         "${buildDir}/matcher.obj"          \
         "${buildDir}/matcher_contains.obj" \
         "${buildDir}/expect_base.obj"      \
         "${buildDir}/expect_to_match.obj"  \
         "${buildDir}/expect_to_throw.obj"  \
         "${buildDir}/expect.obj"           \
         "${buildDir}/stats.obj"            \
         "${buildDir}/results.obj"          \
         "${buildDir}/selected_tests.obj"   \
         "${buildDir}/reporter.obj"         \
         "${buildDir}/printer.obj"          \
         "${buildDir}/filters.obj"          \
         "${buildDir}/test_filter.obj"      \
         "${buildDir}/test_runner.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/${project}-stringify.ifc\"        /Fo\"${buildDir}/${project}-stringify.obj\"        /c /TP \"${projectDir}/stringify.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/${project}-failed_assertion.ifc\" /Fo\"${buildDir}/${project}-failed_assertion.obj\" /c /TP \"${projectDir}/failed_assertion.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/${project}-failure.ifc\"          /Fo\"${buildDir}/${project}-failure.obj\"          /c /TP \"${projectDir}/failure.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/${project}-test.ifc\"             /Fo\"${buildDir}/${project}-test.obj\"             /c /TP \"${projectDir}/test.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/${project}-test_suite.ifc\"       /Fo\"${buildDir}/${project}-test_suite.obj\"       /c /TP \"${projectDir}/test_suite.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/${project}-test_context.ifc\"     /Fo\"${buildDir}/${project}-test_context.obj\"     /c /TP \"${projectDir}/test_context.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/${project}-matcher_base.ifc\"     /Fo\"${buildDir}/${project}-matcher_base.obj\"     /c /TP \"${projectDir}/matcher_base.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/${project}-matcher.ifc\"          /Fo\"${buildDir}/${project}-matcher.obj\"          /c /TP \"${projectDir}/matcher.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/${project}-matcher_contains.ifc\" /Fo\"${buildDir}/${project}-matcher_contains.obj\" /c /TP \"${projectDir}/matcher_contains.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/${project}-expect_base.ifc\"      /Fo\"${buildDir}/${project}-expect_base.obj\"      /c /TP \"${projectDir}/expect_base.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/${project}-expect_to_match.ifc\"  /Fo\"${buildDir}/${project}-expect_to_match.obj\"  /c /TP \"${projectDir}/expect_to_match.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/${project}-expect_to_throw.ifc\"  /Fo\"${buildDir}/${project}-expect_to_throw.obj\"  /c /TP \"${projectDir}/expect_to_throw.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/${project}-expect.ifc\"           /Fo\"${buildDir}/${project}-expect.obj\"           /c /TP \"${projectDir}/expect.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/${project}-stats.ifc\"            /Fo\"${buildDir}/${project}-stats.obj\"            /c /TP \"${projectDir}/stats.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/${project}-filters.ifc\"          /Fo\"${buildDir}/${project}-filters.obj\"          /c /TP \"${projectDir}/filters.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/${project}-test_filter.ifc\"      /Fo\"${buildDir}/${project}-test_filter.obj\"      /c /TP \"${projectDir}/test_filter.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/${project}-results.ifc\"          /Fo\"${buildDir}/${project}-results.obj\"          /c /TP \"${projectDir}/results.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/${project}-selected_tests.ifc\"   /Fo\"${buildDir}/${project}-selected_tests.obj\"   /c /TP \"${projectDir}/selected_tests.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/${project}-reporter.ifc\"         /Fo\"${buildDir}/${project}-reporter.obj\"         /c /TP \"${projectDir}/reporter.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/${project}-printer.ifc\"          /Fo\"${buildDir}/${project}-printer.obj\"          /c /TP \"${projectDir}/printer.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/${project}-test_runner.ifc\"      /Fo\"${buildDir}/${project}-test_runner.obj\"      /c /TP \"${projectDir}/test_runner.cpp\" || exit 1

cl.exe ${msvcCompilerFlags}                        ^
       /interface                                  ^
       /ifcSearchDir \"${buildDir}\"               ^
       /ifcSearchDir \"${buildRoot}/acommandline\" ^
       /ifcOutput\"${buildDir}/${project}.ifc\"    ^
       /Fo\"${buildDir}/${project}.obj\"           ^
       /c \"${projectDir}/${project}.cpp\" || exit 1

lib.exe /NOLOGO                                         ^
        /OUT:\"${buildDir}/${project}.lib\"             ^
        \"${buildDir}/${project}.obj\"                  ^
        \"${buildDir}/${project}-stringify.obj\"        ^
        \"${buildDir}/${project}-failed_assertion.obj\" ^
        \"${buildDir}/${project}-failure.obj\"          ^
        \"${buildDir}/${project}-test.obj\"             ^
        \"${buildDir}/${project}-test_suite.obj\"       ^
        \"${buildDir}/${project}-test_context.obj\"     ^
        \"${buildDir}/${project}-matcher_base.obj\"     ^
        \"${buildDir}/${project}-matcher.obj\"          ^
        \"${buildDir}/${project}-matcher_contains.obj\" ^
        \"${buildDir}/${project}-expect_base.obj\"      ^
        \"${buildDir}/${project}-expect_to_match.obj\"  ^
        \"${buildDir}/${project}-expect_to_throw.obj\"  ^
        \"${buildDir}/${project}-expect.obj\"           ^
        \"${buildDir}/${project}-stats.obj\"            ^
        \"${buildDir}/${project}-results.obj\"          ^
        \"${buildDir}/${project}-selected_tests.obj\"   ^
        \"${buildDir}/${project}-reporter.obj\"         ^
        \"${buildDir}/${project}-printer.obj\"          ^
        \"${buildDir}/${project}-filters.obj\"          ^
        \"${buildDir}/${project}-test_filter.obj\"      ^
        \"${buildDir}/${project}-test_runner.obj\" || exit 1
"

build "acommandline"
