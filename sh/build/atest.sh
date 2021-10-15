source "sh/build_common.sh" "${1}"

project="atest"
projectDir="projects/${project}"
buildDir="${buildRoot}/${project}"

function build_clang() {
    $clang $clangCompilerFlags \
           -Xclang \
           -emit-module-interface \
           -fprebuilt-module-path=${buildRoot}/acommandline \
           -o "${buildDir}/atest.pcm" \
           -c "${projectDir}/atest.cpp"
    
    $clang $clangCompilerFlags \
           -fprebuilt-module-path=${buildRoot}/acommandline \
           -o "${buildDir}/atest.obj" \
           -c "${projectDir}/atest.cpp"
}

function build_gcc() {
    $gcc $gccCompilerFlags -o "${buildDir}/stringify.obj"        -c "${projectDir}/stringify.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/failed_assertion.obj" -c "${projectDir}/failed_assertion.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/failure.obj"          -c "${projectDir}/failure.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/test.obj"             -c "${projectDir}/test.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/test_suite.obj"       -c "${projectDir}/test_suite.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/test_context.obj"     -c "${projectDir}/test_context.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/matcher_base.obj"     -c "${projectDir}/matcher_base.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/matcher.obj"          -c "${projectDir}/matcher.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/matcher_contains.obj" -c "${projectDir}/matcher_contains.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/expect_base.obj"      -c "${projectDir}/expect_base.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/expect_to_match.obj"  -c "${projectDir}/expect_to_match.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/expect_to_throw.obj"  -c "${projectDir}/expect_to_throw.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/expect.obj"           -c "${projectDir}/expect.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/stats.obj"            -c "${projectDir}/stats.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/results.obj"          -c "${projectDir}/results.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/filters.obj"      -c "${projectDir}/filters.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/test_filter.obj"      -c "${projectDir}/test_filter.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/reporter.obj"         -c "${projectDir}/reporter.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/printer.obj"          -c "${projectDir}/printer.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/test_runner.obj"      -c "${projectDir}/test_runner.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/atest.obj"            -c "${projectDir}/atest.cpp"

    ar r "${buildDir}/atest.lib" \
         "${buildDir}/atest.obj" \
         "${buildDir}/stringify.obj" \
         "${buildDir}/failed_assertion.obj" \
         "${buildDir}/failure.obj" \
         "${buildDir}/test.obj" \
         "${buildDir}/test_suite.obj" \
         "${buildDir}/test_context.obj" \
         "${buildDir}/matcher_base.obj" \
         "${buildDir}/matcher.obj" \
         "${buildDir}/matcher_contains.obj" \
         "${buildDir}/expect_base.obj" \
         "${buildDir}/expect_to_match.obj" \
         "${buildDir}/expect_to_throw.obj" \
         "${buildDir}/expect.obj" \
         "${buildDir}/stats.obj" \
         "${buildDir}/results.obj" \
         "${buildDir}/reporter.obj" \
         "${buildDir}/printer.obj" \
         "${buildDir}/filters.obj" \
         "${buildDir}/test_filter.obj" \
         "${buildDir}/test_runner.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/atest-stringify.ifc\"        /Fo\"${buildDir}/atest-stringify.obj\"        /c /TP \"${projectDir}/stringify.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/atest-failed_assertion.ifc\" /Fo\"${buildDir}/atest-failed_assertion.obj\" /c /TP \"${projectDir}/failed_assertion.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/atest-failure.ifc\"          /Fo\"${buildDir}/atest-failure.obj\"          /c /TP \"${projectDir}/failure.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/atest-test.ifc\"             /Fo\"${buildDir}/atest-test.obj\"             /c /TP \"${projectDir}/test.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/atest-test_suite.ifc\"       /Fo\"${buildDir}/atest-test_suite.obj\"       /c /TP \"${projectDir}/test_suite.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/atest-test_context.ifc\"     /Fo\"${buildDir}/atest-test_context.obj\"     /c /TP \"${projectDir}/test_context.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/atest-matcher_base.ifc\"     /Fo\"${buildDir}/atest-matcher_base.obj\"     /c /TP \"${projectDir}/matcher_base.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/atest-matcher.ifc\"          /Fo\"${buildDir}/atest-matcher.obj\"          /c /TP \"${projectDir}/matcher.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/atest-matcher_contains.ifc\" /Fo\"${buildDir}/atest-matcher_contains.obj\" /c /TP \"${projectDir}/matcher_contains.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/atest-expect_base.ifc\"      /Fo\"${buildDir}/atest-expect_base.obj\"      /c /TP \"${projectDir}/expect_base.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/atest-expect_to_match.ifc\"  /Fo\"${buildDir}/atest-expect_to_match.obj\"  /c /TP \"${projectDir}/expect_to_match.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/atest-expect_to_throw.ifc\"  /Fo\"${buildDir}/atest-expect_to_throw.obj\"  /c /TP \"${projectDir}/expect_to_throw.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/atest-expect.ifc\"           /Fo\"${buildDir}/atest-expect.obj\"           /c /TP \"${projectDir}/expect.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/atest-stats.ifc\"            /Fo\"${buildDir}/atest-stats.obj\"            /c /TP \"${projectDir}/stats.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/atest-filters.ifc\"          /Fo\"${buildDir}/atest-filters.obj\"          /c /TP \"${projectDir}/filters.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/atest-test_filter.ifc\"      /Fo\"${buildDir}/atest-test_filter.obj\"      /c /TP \"${projectDir}/test_filter.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/atest-results.ifc\"          /Fo\"${buildDir}/atest-results.obj\"          /c /TP \"${projectDir}/results.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/atest-reporter.ifc\"         /Fo\"${buildDir}/atest-reporter.obj\"         /c /TP \"${projectDir}/reporter.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/atest-printer.ifc\"          /Fo\"${buildDir}/atest-printer.obj\"          /c /TP \"${projectDir}/printer.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/acommandline\" /ifcOutput\"${buildDir}/atest-test_runner.ifc\"      /Fo\"${buildDir}/atest-test_runner.obj\"      /c /TP \"${projectDir}/test_runner.cpp\" || exit 1

cl.exe ${msvcCompilerFlags} ^
       /interface ^
       /ifcSearchDir \"${buildDir}\" ^
       /ifcSearchDir \"${buildRoot}/acommandline\" ^
       /ifcOutput\"${buildDir}/atest.ifc\" ^
       /Fo\"${buildDir}/atest.obj\" ^
       /c \"${projectDir}/atest.cpp\" || exit 1

lib.exe /NOLOGO ^
        /OUT:\"${buildDir}/atest.lib\" ^
        \"${buildDir}/atest.obj\" ^
        \"${buildDir}/atest-stringify.obj\" ^
        \"${buildDir}/atest-failed_assertion.obj\" ^
        \"${buildDir}/atest-failure.obj\" ^
        \"${buildDir}/atest-test.obj\" ^
        \"${buildDir}/atest-test_suite.obj\" ^
        \"${buildDir}/atest-test_context.obj\" ^
        \"${buildDir}/atest-matcher_base.obj\" ^
        \"${buildDir}/atest-matcher.obj\" ^
        \"${buildDir}/atest-matcher_contains.obj\" ^
        \"${buildDir}/atest-expect_base.obj\" ^
        \"${buildDir}/atest-expect_to_match.obj\" ^
        \"${buildDir}/atest-expect_to_throw.obj\" ^
        \"${buildDir}/atest-expect.obj\" ^
        \"${buildDir}/atest-stats.obj\" ^
        \"${buildDir}/atest-results.obj\" ^
        \"${buildDir}/atest-reporter.obj\" ^
        \"${buildDir}/atest-printer.obj\" ^
        \"${buildDir}/atest-filters.obj\" ^
        \"${buildDir}/atest-test_filter.obj\" ^
        \"${buildDir}/atest-test_runner.obj\" || exit 1
"

sh/build/acommandline.sh "${toolchain}"
build
