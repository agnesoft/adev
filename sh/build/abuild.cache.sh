source "sh/build_common.sh" "${1}"

project="abuild.cache"
projectDir="projects/abuild/cache"
buildDir="${buildRoot}/abuild/cache"

function build_clang() {
    $clang $clangCompilerFlags \
           -Xclang -emit-module-interface \
           -fprebuilt-module-path=${buildRoot}/abuild/cpptokenizer \
           -fprebuilt-module-path=${buildRoot}/yamlcpp \
           -o "${buildDir}/abuild.cache.pcm" \
           -c "${projectDir}/cache.cpp"
    
    $clang $clangCompilerFlags \
           -fprebuilt-module-path=${buildRoot}/abuild/cpptokenizer \
           -fprebuilt-module-path=${buildRoot}/yamlcpp \
           -o "${buildDir}/abuild.cache.obj" \
           -c "${projectDir}/cache.cpp"
}

function build_gcc() {
    $gcc $gccCompilerFlags -o "${buildDir}/file.obj"              -c "${projectDir}/file.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/source_file.obj"       -c "${projectDir}/source_file.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/header_file.obj"       -c "${projectDir}/header_file.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/project.obj"           -c "${projectDir}/project.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/settings.obj"          -c "${projectDir}/settings.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/cache_data.obj"        -c "${projectDir}/cache_data.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/cache_index.obj"       -c "${projectDir}/cache_index.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/cache_impl.obj"        -c "${projectDir}/cache_impl.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/cache_reader.obj"      -c "${projectDir}/cache_reader.cpp"
    $gcc $gccCompilerFlags -o "${buildDir}/cache_writer.obj"      -c "${projectDir}/cache_writer.cpp"

    $gcc $gccCompilerFlags -o "${buildDir}/cache.obj" -c "${projectDir}/cache.cpp"

    ar r "${buildDir}/abuild.cache.lib" \
         "${buildDir}/cache.obj" \
         "${buildDir}/file.obj" \
         "${buildDir}/source_file.obj" \
         "${buildDir}/header_file.obj" \
         "${buildDir}/project_file.obj" \
         "${buildDir}/settings.obj" \
         "${buildDir}/cache_data.obj" \
         "${buildDir}/cache_index.obj" \
         "${buildDir}/cache_impl.obj" \
         "${buildDir}/cache_reader.obj" \
         "${buildDir}/cache_writer.obj"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-project.ifc\"          /Fo\"${buildDir}/abuild.cache-project.obj\"          /c /TP \"${projectDir}/project.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-file.ifc\"             /Fo\"${buildDir}/abuild.cache-file.obj\"             /c /TP \"${projectDir}/file.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-binary_file.ifc\"      /Fo\"${buildDir}/abuild.cache-binary_file.obj\"      /c /TP \"${projectDir}/binary_file.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-cpp_file.ifc\"         /Fo\"${buildDir}/abuild.cache-cpp_file.obj\"         /c /TP \"${projectDir}/cpp_file.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-source_file.ifc\"      /Fo\"${buildDir}/abuild.cache-source_file.obj\"      /c /TP \"${projectDir}/source_file.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-header_file.ifc\"      /Fo\"${buildDir}/abuild.cache-header_file.obj\"      /c /TP \"${projectDir}/header_file.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-executable.ifc\"       /Fo\"${buildDir}/abuild.cache-executable.obj\"       /c /TP \"${projectDir}/executable.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-dynamic_library.ifc\"  /Fo\"${buildDir}/abuild.cache-dynamic_library.obj\"  /c /TP \"${projectDir}/dynamic_library.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-static_library.ifc\"   /Fo\"${buildDir}/abuild.cache-static_library.obj\"   /c /TP \"${projectDir}/static_library.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-visibility.ifc\"       /Fo\"${buildDir}/abuild.cache-visibility.obj\"       /c /TP \"${projectDir}/visibility.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-module_partition.ifc\" /Fo\"${buildDir}/abuild.cache-module_partition.obj\" /c /TP \"${projectDir}/module_partition.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-module.ifc\"           /Fo\"${buildDir}/abuild.cache-module.obj\"           /c /TP \"${projectDir}/module.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-header.ifc\"           /Fo\"${buildDir}/abuild.cache-header.obj\"           /c /TP \"${projectDir}/header.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-header_unit.ifc\"      /Fo\"${buildDir}/abuild.cache-header_unit.obj\"      /c /TP \"${projectDir}/header_unit.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-settings.ifc\"         /Fo\"${buildDir}/abuild.cache-settings.obj\"         /c /TP \"${projectDir}/settings.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-abi.ifc\"              /Fo\"${buildDir}/abuild.cache-abi.obj\"              /c /TP \"${projectDir}/abi.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-flag.ifc\"             /Fo\"${buildDir}/abuild.cache-flag.obj\"             /c /TP \"${projectDir}/flag.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-toolchain.ifc\"        /Fo\"${buildDir}/abuild.cache-toolchain.obj\"        /c /TP \"${projectDir}/toolchain.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-translation_unit.ifc\" /Fo\"${buildDir}/abuild.cache-translation_unit.obj\" /c /TP \"${projectDir}/translation_unit.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-configuration.ifc\"    /Fo\"${buildDir}/abuild.cache-configuration.obj\"    /c /TP \"${projectDir}/configuration.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-cache_data.ifc\"       /Fo\"${buildDir}/abuild.cache-cache_data.obj\"       /c /TP \"${projectDir}/cache_data.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-cache_index.ifc\"      /Fo\"${buildDir}/abuild.cache-cache_index.obj\"      /c /TP \"${projectDir}/cache_index.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcOutput\"${buildDir}/abuild.cache-cache_impl.ifc\"       /Fo\"${buildDir}/abuild.cache-cache_impl.obj\"       /c /TP \"${projectDir}/cache_impl.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcSearchDir \"${buildRoot}/yamlcpp\" /ifcOutput\"${buildDir}/abuild.cache-cache_reader.ifc\" /Fo\"${buildDir}/abuild.cache-cache_reader.obj\" /c /TP \"${projectDir}/cache_reader.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /internalPartition /ifcSearchDir \"${buildDir}\" /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" /ifcSearchDir \"${buildRoot}/yamlcpp\" /ifcOutput\"${buildDir}/abuild.cache-cache_writer.ifc\" /Fo\"${buildDir}/abuild.cache-cache_writer.obj\" /c /TP \"${projectDir}/cache_writer.cpp\" || exit 1

cl.exe ${msvcCompilerFlags} ^
       /interface ^
       /ifcSearchDir \"${buildDir}\" ^
       /ifcSearchDir \"${buildRoot}/abuild/cpptokenizer\" ^
       /ifcSearchDir \"${buildRoot}/yamlcpp\" ^
       /ifcOutput\"${buildDir}/abuild.cache.ifc\" ^
       /Fo\"${buildDir}/abuild.cache.obj\" ^
       /c \"${projectDir}/cache.cpp\" || exit 1

lib.exe /NOLOGO ^
        /OUT:\"${buildDir}/abuild.cache.lib\" ^
        \"${buildDir}/abuild.cache.obj\" ^
        \"${buildDir}/abuild.cache-project.obj\" ^
        \"${buildDir}/abuild.cache-file.obj\" ^
        \"${buildDir}/abuild.cache-binary_file.obj\" ^
        \"${buildDir}/abuild.cache-cpp_file.obj\" ^
        \"${buildDir}/abuild.cache-source_file.obj\" ^
        \"${buildDir}/abuild.cache-header_file.obj\" ^
        \"${buildDir}/abuild.cache-executable.obj\" ^
        \"${buildDir}/abuild.cache-dynamic_libary.obj\" ^
        \"${buildDir}/abuild.cache-static_library.obj\" ^
        \"${buildDir}/abuild.cache-visibility.obj\" ^
        \"${buildDir}/abuild.cache-module_partition.obj\" ^
        \"${buildDir}/abuild.cache-module.obj\" ^
        \"${buildDir}/abuild.cache-header.obj\" ^
        \"${buildDir}/abuild.cache-header_unit.obj\" ^
        \"${buildDir}/abuild.cache-settings.obj\" ^
        \"${buildDir}/abuild.cache-abi.obj\" ^
        \"${buildDir}/abuild.cache-flag.obj\" ^
        \"${buildDir}/abuild.cache-toolchain.obj\" ^
        \"${buildDir}/abuild.cache-translation_unit.obj\" ^
        \"${buildDir}/abuild.cache-configuration.obj\" ^
        \"${buildDir}/abuild.cache-cache_data.obj\" ^
        \"${buildDir}/abuild.cache-cache_index.obj\" ^
        \"${buildDir}/abuild.cache-cache_impl.obj\" ^
        \"${buildDir}/abuild.cache-cache_reader.obj\" ^
        \"${buildDir}/abuild.cache-cache_writer.obj\" || exit 1
"

sh/build/abuild.cpptokenizer.sh "${toolchain}"
sh/build/yamlcpp.sh "${toolchain}"
build
