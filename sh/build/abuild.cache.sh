source "sh/build_common.sh" 

set_build_properties "abuild.cache" "${1}" "${2}"

function build_clang() {
    ${clang} ${clangCompilerFlags}                                     \
             -Xclang -emit-module-interface                            \
             -fprebuilt-module-path=${buildRoot}/abuild.cpptokenizer   \
             -fprebuilt-module-path=${buildRoot}/yamlcpp               \
             -o "${buildDir}/${project}.pcm"                           \
             -c "${projectDir}/${project}.cpp"
    
    ${clang} ${clangCompilerFlags}                                     \
             -fprebuilt-module-path=${buildRoot}/abuild.cpptokenizer   \
             -fprebuilt-module-path=${buildRoot}/yamlcpp               \
             -o "${buildDir}/${project}.obj"                           \
             -c "${projectDir}/${project}.cpp"
}

function build_gcc() {
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/file.obj"             -c "${projectDir}/file.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/project.obj"          -c "${projectDir}/project.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/header.obj"           -c "${projectDir}/header.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/cpp_file.obj"         -c "${projectDir}/cpp_file.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/source_file.obj"      -c "${projectDir}/source_file.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/header_file.obj"      -c "${projectDir}/header_file.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/module_partition.obj" -c "${projectDir}/module_partition.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/module_.obj"          -c "${projectDir}/module_.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/header_unit.obj"      -c "${projectDir}/header_unit.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/settings.obj"         -c "${projectDir}/settings.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/abi.obj"              -c "${projectDir}/abi.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/flag.obj"             -c "${projectDir}/flag.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/toolchain.obj"        -c "${projectDir}/toolchain.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/cache_data.obj"       -c "${projectDir}/cache_data.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/cache_index.obj"      -c "${projectDir}/cache_index.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/cache.obj"            -c "${projectDir}/cache.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/cache_reader.obj"     -c "${projectDir}/cache_reader.cpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/cache_writer.obj"     -c "${projectDir}/cache_writer.cpp"

    ${gcc} ${gccCompilerFlags} -o "${buildDir}/${project}.obj" -c "${projectDir}/${project}.cpp"

    ar r "${buildDir}/${project}.lib"       \
         "${buildDir}/${project}.obj"       \
         "${buildDir}/project.obj"          \
         "${buildDir}/header.obj"           \
         "${buildDir}/file.obj"             \
         "${buildDir}/cpp_file.obj"         \
         "${buildDir}/source_file.obj"      \
         "${buildDir}/header_file.obj"      \
         "${buildDir}/module_partition.obj" \
         "${buildDir}/module_.obj"          \
         "${buildDir}/header_unit.obj"      \
         "${buildDir}/settings.obj"         \
         "${buildDir}/abi.obj"              \
         "${buildDir}/flag.obj"             \
         "${buildDir}/toolchain.obj"        \
         "${buildDir}/cache_data.obj"       \
         "${buildDir}/cache_index.obj"      \
         "${buildDir}/cache.obj"            \
         "${buildDir}/cache_reader.obj"     \
         "${buildDir}/cache_writer.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild.cpptokenizer\" /ifcOutput\"${buildDir}/${project}-file.ifc\"                /Fo\"${buildDir}/${project}-file.obj\"                /c /TP \"${projectDir}/file.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild.cpptokenizer\" /ifcOutput\"${buildDir}/${project}-project.ifc\"             /Fo\"${buildDir}/${project}-project.obj\"             /c /TP \"${projectDir}/project.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild.cpptokenizer\" /ifcOutput\"${buildDir}/${project}-header.ifc\"              /Fo\"${buildDir}/${project}-header.obj\"              /c /TP \"${projectDir}/header.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild.cpptokenizer\" /ifcOutput\"${buildDir}/${project}-cpp_file.ifc\"            /Fo\"${buildDir}/${project}-cpp_file.obj\"            /c /TP \"${projectDir}/cpp_file.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild.cpptokenizer\" /ifcOutput\"${buildDir}/${project}-source_file.ifc\"         /Fo\"${buildDir}/${project}-source_file.obj\"         /c /TP \"${projectDir}/source_file.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild.cpptokenizer\" /ifcOutput\"${buildDir}/${project}-header_file.ifc\"         /Fo\"${buildDir}/${project}-header_file.obj\"         /c /TP \"${projectDir}/header_file.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild.cpptokenizer\" /ifcOutput\"${buildDir}/${project}-module_partition.ifc\"    /Fo\"${buildDir}/${project}-module_partition.obj\"    /c /TP \"${projectDir}/module_partition.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild.cpptokenizer\" /ifcOutput\"${buildDir}/${project}-module_.ifc\"             /Fo\"${buildDir}/${project}-module_.obj\"             /c /TP \"${projectDir}/module.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild.cpptokenizer\" /ifcOutput\"${buildDir}/${project}-header_unit.ifc\"         /Fo\"${buildDir}/${project}-header_unit.obj\"         /c /TP \"${projectDir}/header_unit.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild.cpptokenizer\" /ifcOutput\"${buildDir}/${project}-settings.ifc\"            /Fo\"${buildDir}/${project}-settings.obj\"            /c /TP \"${projectDir}/settings.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild.cpptokenizer\" /ifcOutput\"${buildDir}/${project}-abi.ifc\"                 /Fo\"${buildDir}/${project}-abi.obj\"                 /c /TP \"${projectDir}/abi.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild.cpptokenizer\" /ifcOutput\"${buildDir}/${project}-flag.ifc\"                /Fo\"${buildDir}/${project}-flag.obj\"                /c /TP \"${projectDir}/flag.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild.cpptokenizer\" /ifcOutput\"${buildDir}/${project}-toolchain.ifc\"           /Fo\"${buildDir}/${project}-toolchain.obj\"           /c /TP \"${projectDir}/toolchain.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild.cpptokenizer\" /ifcOutput\"${buildDir}/${project}-cache_data.ifc\"          /Fo\"${buildDir}/${project}-cache_data.obj\"          /c /TP \"${projectDir}/cache_data.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild.cpptokenizer\" /ifcOutput\"${buildDir}/${project}-cache_index.ifc\"         /Fo\"${buildDir}/${project}-cache_index.obj\"         /c /TP \"${projectDir}/cache_index.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild.cpptokenizer\" /ifcOutput\"${buildDir}/${project}-cache.ifc\"               /Fo\"${buildDir}/${project}-cache.obj\"               /c /TP \"${projectDir}/cache.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild.cpptokenizer\" /ifcSearchDir \"${buildRoot}/yamlcpp\" /ifcOutput\"${buildDir}/${project}-cache_reader.ifc\"        /Fo\"${buildDir}/${project}-cache_reader.obj\"        /c /TP \"${projectDir}/cache_reader.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild.cpptokenizer\" /ifcSearchDir \"${buildRoot}/yamlcpp\" /ifcOutput\"${buildDir}/${project}-cache_writer.ifc\"        /Fo\"${buildDir}/${project}-cache_writer.obj\"        /c /TP \"${projectDir}/cache_writer.cpp\" || exit 1

cl.exe ${msvcCompilerFlags}                               ^
       /interface                                         ^
       /ifcSearchDir \"${buildDir}\"                      ^
       /ifcSearchDir \"${buildRoot}/abuild.cpptokenizer\" ^
       /ifcSearchDir \"${buildRoot}/yamlcpp\"             ^
       /ifcOutput\"${buildDir}/${project}.ifc\"           ^
       /Fo\"${buildDir}/${project}.obj\"                  ^
       /c \"${projectDir}/${project}.cpp\" || exit 1

lib.exe /NOLOGO                                         ^
        /OUT:\"${buildDir}/${project}.lib\"             ^
        \"${buildDir}/${project}.obj\"                  ^
        \"${buildDir}/${project}-project.obj\"          ^
        \"${buildDir}/${project}-file.obj\"             ^
        \"${buildDir}/${project}-cpp_file.obj\"         ^
        \"${buildDir}/${project}-source_file.obj\"      ^
        \"${buildDir}/${project}-header_file.obj\"      ^
        \"${buildDir}/${project}-module_partition.obj\" ^
        \"${buildDir}/${project}-module_.obj\"          ^
        \"${buildDir}/${project}-header.obj\"           ^
        \"${buildDir}/${project}-header_unit.obj\"      ^
        \"${buildDir}/${project}-settings.obj\"         ^
        \"${buildDir}/${project}-abi.obj\"              ^
        \"${buildDir}/${project}-flag.obj\"             ^
        \"${buildDir}/${project}-toolchain.obj\"        ^
        \"${buildDir}/${project}-cache_data.obj\"       ^
        \"${buildDir}/${project}-cache_index.obj\"      ^
        \"${buildDir}/${project}-cache.obj\"            ^
        \"${buildDir}/${project}-cache_reader.obj\"     ^
        \"${buildDir}/${project}-cache_writer.obj\" || exit 1
"

build "abuild.cpptokenizer yamlcpp"
