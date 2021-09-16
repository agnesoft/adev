source "sh/common_build.sh" $1

project
projectDir="projects/$project"
buildDir="$buildRoot/$project"

clangBuild="
$clang $clangCompilerFlags \
       -Xclang -emit-module-interface \
       -o \"$buildDir/acommandline.pcm\" \
       -c \"$projectDir/acommandline.cpp\"

$clang $clangCompilerFlags \
       -o \"$buildDir/acommandline.obj\" \
       -c \"$projectDir/acommandline.cpp\"
"

gccBuild="
$gcc $gccCompilerFlags -o \"$buildDir/option.obj\"                   -c \"$projectDir/option.cpp\"
$gcc $gccCompilerFlags -o \"$buildDir/option_builder_base.obj\"      -c \"$projectDir/option_builder_base.cpp\"
$gcc $gccCompilerFlags -o \"$buildDir/option_builder_described.obj\" -c \"$projectDir/option_builder_described.cpp\"
$gcc $gccCompilerFlags -o \"$buildDir/option_builder_defined.obj\"   -c \"$projectDir/option_builder_defined.cpp\"
$gcc $gccCompilerFlags -o \"$buildDir/option_builder_value.obj\"     -c \"$projectDir/option_builder_value.cpp\"
$gcc $gccCompilerFlags -o \"$buildDir/option_builder_named.obj\"     -c \"$projectDir/option_builder_named.cpp\"
$gcc $gccCompilerFlags -o \"$buildDir/option_builder.obj\"           -c \"$projectDir/option_builder.cpp\"
$gcc $gccCompilerFlags -o \"$buildDir/option_matcher.obj\"           -c \"$projectDir/option_matcher.cpp\"
$gcc $gccCompilerFlags -o \"$buildDir/option_setter.obj\"            -c \"$projectDir/option_setter.cpp\"
$gcc $gccCompilerFlags -o \"$buildDir/option_help_line.obj\"         -c \"$projectDir/option_help_line.cpp\"
$gcc $gccCompilerFlags -o \"$buildDir/printer.obj\"                  -c \"$projectDir/printer.cpp\"
$gcc $gccCompilerFlags -o \"$buildDir/command_line.obj\"             -c \"$projectDir/command_line.cpp\"

$gcc $gccCompilerFlags \
     -o \"$buildDir/acommandline.obj\" \
     -c \"$projectDir/acommandline.cpp\"

ar r \"$buildDir/acommandline.lib\" \
     \"$buildDir/acommandline.obj\" \
     \"$buildDir/option.obj\" \
     \"$buildDir/option_builder_base.obj\" \
     \"$buildDir/option_builder_described.obj\" \
     \"$buildDir/option_builder_defined.obj\" \
     \"$buildDir/option_builder_value.obj\" \
     \"$buildDir/option_builder_named.obj\" \
     \"$buildDir/option_builder.obj\" \
     \"$buildDir/option_matcher.obj\" \
     \"$buildDir/option_setter.obj\" \
     \"$buildDir/option_help_line.obj\" \
     \"$buildDir/printer.obj\" \
     \"$buildDir/command_line.obj\"
"

msvcBuild="
cl.exe $msvcCompilerFlags /internalPartition /ifcSearchDir \"$buildDir\" /ifcOutput\"$buildDir/acommandline-option.ifc\"                   /Fo\"$buildDir/acommandline-option.obj\"                   /c /TP \"$projectDir/option.cpp\"
cl.exe $msvcCompilerFlags /internalPartition /ifcSearchDir \"$buildDir\" /ifcOutput\"$buildDir/acommandline-option_builder_base.ifc\"      /Fo\"$buildDir/acommandline-option_builder_base.obj\"      /c /TP \"$projectDir/option_builder_base.cpp\"
cl.exe $msvcCompilerFlags /internalPartition /ifcSearchDir \"$buildDir\" /ifcOutput\"$buildDir/acommandline-option_builder_described.ifc\" /Fo\"$buildDir/acommandline-option_builder_described.obj\" /c /TP \"$projectDir/option_builder_described.cpp\"
cl.exe $msvcCompilerFlags /internalPartition /ifcSearchDir \"$buildDir\" /ifcOutput\"$buildDir/acommandline-option_builder_defined.ifc\"   /Fo\"$buildDir/acommandline-option_builder_defined.obj\"   /c /TP \"$projectDir/option_builder_defined.cpp\"
cl.exe $msvcCompilerFlags /internalPartition /ifcSearchDir \"$buildDir\" /ifcOutput\"$buildDir/acommandline-option_builder_value.ifc\"     /Fo\"$buildDir/acommandline-option_builder_value.obj\"     /c /TP \"$projectDir/option_builder_value.cpp\"
cl.exe $msvcCompilerFlags /internalPartition /ifcSearchDir \"$buildDir\" /ifcOutput\"$buildDir/acommandline-option_builder_named.ifc\"     /Fo\"$buildDir/acommandline-option_builder_named.obj\"     /c /TP \"$projectDir/option_builder_named.cpp\"
cl.exe $msvcCompilerFlags /internalPartition /ifcSearchDir \"$buildDir\" /ifcOutput\"$buildDir/acommandline-option_builder.ifc\"           /Fo\"$buildDir/acommandline-option_builder.obj\"           /c /TP \"$projectDir/option_builder.cpp\"
cl.exe $msvcCompilerFlags /internalPartition /ifcSearchDir \"$buildDir\" /ifcOutput\"$buildDir/acommandline-option_matcher.ifc\"           /Fo\"$buildDir/acommandline-option_matcher.obj\"           /c /TP \"$projectDir/option_matcher.cpp\"
cl.exe $msvcCompilerFlags /internalPartition /ifcSearchDir \"$buildDir\" /ifcOutput\"$buildDir/acommandline-option_setter.ifc\"            /Fo\"$buildDir/acommandline-option_setter.obj\"            /c /TP \"$projectDir/option_setter.cpp\"
cl.exe $msvcCompilerFlags /internalPartition /ifcSearchDir \"$buildDir\" /ifcOutput\"$buildDir/acommandline-option_help_line.ifc\"         /Fo\"$buildDir/acommandline-option_help_line.obj\"         /c /TP \"$projectDir/option_help_line.cpp\"
cl.exe $msvcCompilerFlags /internalPartition /ifcSearchDir \"$buildDir\" /ifcOutput\"$buildDir/acommandline-printer.ifc\"                  /Fo\"$buildDir/acommandline-printer.obj\"                  /c /TP \"$projectDir/printer.cpp\"
cl.exe $msvcCompilerFlags /internalPartition /ifcSearchDir \"$buildDir\" /ifcOutput\"$buildDir/acommandline-command_line.ifc\"             /Fo\"$buildDir/acommandline-command_line.obj\"             /c /TP \"$projectDir/command_line.cpp\"

cl.exe $msvcCompilerFlags ^
       /interface ^
       /ifcSearchDir \"$buildDir\" ^
       /ifcOutput\"$buildDir/acommandline.ifc\" ^
       /Fo\"$buildDir/acommandline.obj\" ^
       /c \"$projectDir/acommandline.cpp\"

lib.exe /NOLOGO ^
        /OUT:\"$buildDir/acommandline.lib\" ^
        \"$buildDir/acommandline.obj\" ^
        \"$buildDir/acommandline-option.obj\" ^
        \"$buildDir/acommandline-option_builder_base.obj\" ^
        \"$buildDir/acommandline-option_builder_described.obj\" ^
        \"$buildDir/acommandline-option_builder_defined.obj\" ^
        \"$buildDir/acommandline-option_builder_value.obj\" ^
        \"$buildDir/acommandline-option_builder_named.obj\" ^
        \"$buildDir/acommandline-option_builder.obj\" ^
        \"$buildDir/acommandline-option_matcher.obj\" ^
        \"$buildDir/acommandline-option_setter.obj\" ^
        \"$buildDir/acommandline-option_help_line.obj\" ^
        \"$buildDir/acommandline-printer.obj\" ^
        \"$buildDir/acommandline-command_line.obj\"

"

sh/build_astl.sh $toolchain
build
