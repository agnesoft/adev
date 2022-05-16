source "sh/build_common.sh" 

set_build_properties "acommandline" "${1}" "${2}"

function build_clang() {
    ${clang} ${clangCompilerFlags}           \
             -Xclang -emit-module-interface  \
             -o "${buildDir}/${project}.pcm" \
             -c "${projectDir}/${project}.cpp"

    ${clang} ${clangCompilerFlags}           \
             -o "${buildDir}/${project}.obj" \
             -c "${projectDir}/${project}.cpp"
}

function build_gcc() {
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/option.obj"                   -c "${projectDir}/option.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/option_builder_base.obj"      -c "${projectDir}/option_builder_base.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/option_builder_described.obj" -c "${projectDir}/option_builder_described.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/option_builder_defined.obj"   -c "${projectDir}/option_builder_defined.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/option_builder_value.obj"     -c "${projectDir}/option_builder_value.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/option_builder_named.obj"     -c "${projectDir}/option_builder_named.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/option_builder.obj"           -c "${projectDir}/option_builder.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/option_matcher.obj"           -c "${projectDir}/option_matcher.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/option_setter.obj"            -c "${projectDir}/option_setter.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/option_help_line.obj"         -c "${projectDir}/option_help_line.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/printer.obj"                  -c "${projectDir}/printer.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/command_line.obj"             -c "${projectDir}/command_line.cpp"

    ${gcc} ${gccCompilerFlags} -o "${buildDir}/${project}.obj"             -c "${projectDir}/${project}.cpp"

    ar r "${buildDir}/${project}.lib"               \
         "${buildDir}/${project}.obj"               \
         "${buildDir}/option.obj"                   \
         "${buildDir}/option_builder_base.obj"      \
         "${buildDir}/option_builder_described.obj" \
         "${buildDir}/option_builder_defined.obj"   \
         "${buildDir}/option_builder_value.obj"     \
         "${buildDir}/option_builder_named.obj"     \
         "${buildDir}/option_builder.obj"           \
         "${buildDir}/option_matcher.obj"           \
         "${buildDir}/option_setter.obj"            \
         "${buildDir}/option_help_line.obj"         \
         "${buildDir}/printer.obj"                  \
         "${buildDir}/command_line.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-option.ifc\"                   /Fo\"${buildDir}/${project}-option.obj\"                   /c /TP \"${projectDir}/option.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-option_builder_base.ifc\"      /Fo\"${buildDir}/${project}-option_builder_base.obj\"      /c /TP \"${projectDir}/option_builder_base.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-option_builder_described.ifc\" /Fo\"${buildDir}/${project}-option_builder_described.obj\" /c /TP \"${projectDir}/option_builder_described.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-option_builder_defined.ifc\"   /Fo\"${buildDir}/${project}-option_builder_defined.obj\"   /c /TP \"${projectDir}/option_builder_defined.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-option_builder_value.ifc\"     /Fo\"${buildDir}/${project}-option_builder_value.obj\"     /c /TP \"${projectDir}/option_builder_value.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-option_builder_named.ifc\"     /Fo\"${buildDir}/${project}-option_builder_named.obj\"     /c /TP \"${projectDir}/option_builder_named.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-option_builder.ifc\"           /Fo\"${buildDir}/${project}-option_builder.obj\"           /c /TP \"${projectDir}/option_builder.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-option_matcher.ifc\"           /Fo\"${buildDir}/${project}-option_matcher.obj\"           /c /TP \"${projectDir}/option_matcher.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-option_setter.ifc\"            /Fo\"${buildDir}/${project}-option_setter.obj\"            /c /TP \"${projectDir}/option_setter.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-option_help_line.ifc\"         /Fo\"${buildDir}/${project}-option_help_line.obj\"         /c /TP \"${projectDir}/option_help_line.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-printer.ifc\"                  /Fo\"${buildDir}/${project}-printer.obj\"                  /c /TP \"${projectDir}/printer.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcOutput\"${buildDir}/${project}-command_line.ifc\"             /Fo\"${buildDir}/${project}-command_line.obj\"             /c /TP \"${projectDir}/command_line.cpp\" || exit 1

cl.exe ${msvcCompilerFlags}                     ^
       /interface                               ^
       /ifcSearchDir \"${buildDir}\"            ^
       /ifcOutput\"${buildDir}/${project}.ifc\" ^
       /Fo\"${buildDir}/${project}.obj\"        ^
       /c \"${projectDir}/${project}.cpp\" || exit 1

lib.exe /NOLOGO                                                 ^
        /OUT:\"${buildDir}/${project}.lib\"                     ^
        \"${buildDir}/${project}.obj\"                          ^
        \"${buildDir}/${project}-option.obj\"                   ^
        \"${buildDir}/${project}-option_builder_base.obj\"      ^
        \"${buildDir}/${project}-option_builder_described.obj\" ^
        \"${buildDir}/${project}-option_builder_defined.obj\"   ^
        \"${buildDir}/${project}-option_builder_value.obj\"     ^
        \"${buildDir}/${project}-option_builder_named.obj\"     ^
        \"${buildDir}/${project}-option_builder.obj\"           ^
        \"${buildDir}/${project}-option_matcher.obj\"           ^
        \"${buildDir}/${project}-option_setter.obj\"            ^
        \"${buildDir}/${project}-option_help_line.obj\"         ^
        \"${buildDir}/${project}-printer.obj\"                  ^
        \"${buildDir}/${project}-command_line.obj\" || exit 1
"

build "astl"
