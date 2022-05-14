source "sh/build_common.sh" 

set_build_properties "yamlcpp" "${1}" "${2}"

function build_clang() {
    ${clang} ${clangCompilerFlags}           \
             -Xclang -emit-module-interface  \
             -I"${projectDir}/include"       \
             -o "${buildDir}/${project}.pcm" \
             -c "${projectDir}/${project}.cpp"

    ${clang} ${clangCompilerFlags}           \
             -I"${projectDir}/include"       \
             -o "${buildDir}/${project}.obj" \
             -c "${projectDir}/${project}.cpp"

    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/binary.o"          -c "${projectDir}/src/binary.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/convert.o"         -c "${projectDir}/src/convert.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/depthguard.o"      -c "${projectDir}/src/depthguard.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/directives.o"      -c "${projectDir}/src/directives.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/emit.o"            -c "${projectDir}/src/emit.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/emitfromevents.o"  -c "${projectDir}/src/emitfromevents.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/emitter.o"         -c "${projectDir}/src/emitter.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/emitterstate.o"    -c "${projectDir}/src/emitterstate.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/emitterutils.o"    -c "${projectDir}/src/emitterutils.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/exceptions.o"      -c "${projectDir}/src/exceptions.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/exp.o"             -c "${projectDir}/src/exp.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/memory.o"          -c "${projectDir}/src/memory.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/node.o"            -c "${projectDir}/src/node.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/node_data.o"       -c "${projectDir}/src/node_data.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/nodebuilder.o"     -c "${projectDir}/src/nodebuilder.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/nodeevents.o"      -c "${projectDir}/src/nodeevents.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/null.o"            -c "${projectDir}/src/null.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/ostream_wrapper.o" -c "${projectDir}/src/ostream_wrapper.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/parse.o"           -c "${projectDir}/src/parse.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/parser.o"          -c "${projectDir}/src/parser.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/regex_yaml.o"      -c "${projectDir}/src/regex_yaml.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/scanner.o"         -c "${projectDir}/src/scanner.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/scanscalar.o"      -c "${projectDir}/src/scanscalar.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/scantag.o"         -c "${projectDir}/src/scantag.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/scantoken.o"       -c "${projectDir}/src/scantoken.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/simplekey.o"       -c "${projectDir}/src/simplekey.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/singledocparser.o" -c "${projectDir}/src/singledocparser.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/stream.o"          -c "${projectDir}/src/stream.cpp"
    ${clang} ${clangCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/tag.o"             -c "${projectDir}/src/tag.cpp"

    ${ar} r "${buildDir}/${project}.lib"    \
            "${buildDir}/${project}.obj"    \
            "${buildDir}/binary.o"          \
            "${buildDir}/convert.o"         \
            "${buildDir}/depthguard.o"      \
            "${buildDir}/directives.o"      \
            "${buildDir}/emit.o"            \
            "${buildDir}/emitfromevents.o"  \
            "${buildDir}/emitter.o"         \
            "${buildDir}/emitterstate.o"    \
            "${buildDir}/emitterutils.o"    \
            "${buildDir}/exceptions.o"      \
            "${buildDir}/exp.o"             \
            "${buildDir}/memory.o"          \
            "${buildDir}/node.o"            \
            "${buildDir}/node_data.o"       \
            "${buildDir}/nodebuilder.o"     \
            "${buildDir}/nodeevents.o"      \
            "${buildDir}/null.o"            \
            "${buildDir}/ostream_wrapper.o" \
            "${buildDir}/parse.o"           \
            "${buildDir}/parser.o"          \
            "${buildDir}/regex_yaml.o"      \
            "${buildDir}/scanner.o"         \
            "${buildDir}/scanscalar.o"      \
            "${buildDir}/scantag.o"         \
            "${buildDir}/scantoken.o"       \
            "${buildDir}/simplekey.o"       \
            "${buildDir}/singledocparser.o" \
            "${buildDir}/stream.o"          \
            "${buildDir}/tag.o"
}

function build_gcc() {
    ${gcc} ${gccCompilerFlags} -x c++-header "${projectDir}/${project}.hpp"
    ${gcc} ${gccCompilerFlags} -o "${buildDir}/${project}.obj" -c "${projectDir}/${project}.cpp"

    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/binary.o"          -c "${projectDir}/src/binary.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/convert.o"         -c "${projectDir}/src/convert.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/depthguard.o"      -c "${projectDir}/src/depthguard.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/directives.o"      -c "${projectDir}/src/directives.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/emit.o"            -c "${projectDir}/src/emit.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/emitfromevents.o"  -c "${projectDir}/src/emitfromevents.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/emitter.o"         -c "${projectDir}/src/emitter.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/emitterstate.o"    -c "${projectDir}/src/emitterstate.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/emitterutils.o"    -c "${projectDir}/src/emitterutils.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/exceptions.o"      -c "${projectDir}/src/exceptions.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/exp.o"             -c "${projectDir}/src/exp.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/memory.o"          -c "${projectDir}/src/memory.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/node.o"            -c "${projectDir}/src/node.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/node_data.o"       -c "${projectDir}/src/node_data.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/nodebuilder.o"     -c "${projectDir}/src/nodebuilder.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/nodeevents.o"      -c "${projectDir}/src/nodeevents.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/null.o"            -c "${projectDir}/src/null.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/ostream_wrapper.o" -c "${projectDir}/src/ostream_wrapper.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/parse.o"           -c "${projectDir}/src/parse.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/parser.o"          -c "${projectDir}/src/parser.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/regex_yaml.o"      -c "${projectDir}/src/regex_yaml.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/scanner.o"         -c "${projectDir}/src/scanner.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/scanscalar.o"      -c "${projectDir}/src/scanscalar.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/scantag.o"         -c "${projectDir}/src/scantag.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/scantoken.o"       -c "${projectDir}/src/scantoken.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/simplekey.o"       -c "${projectDir}/src/simplekey.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/singledocparser.o" -c "${projectDir}/src/singledocparser.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/stream.o"          -c "${projectDir}/src/stream.cpp"
    ${gcc} ${gccCompilerFlags} -I"${projectDir}/include" -o "${buildDir}/tag.o"             -c "${projectDir}/src/tag.cpp"

    ar r "${buildDir}/${project}.lib"    \
         "${buildDir}/${project}.obj"    \
         "${buildDir}/binary.o"          \
         "${buildDir}/convert.o"         \
         "${buildDir}/depthguard.o"      \
         "${buildDir}/directives.o"      \
         "${buildDir}/emit.o"            \
         "${buildDir}/emitfromevents.o"  \
         "${buildDir}/emitter.o"         \
         "${buildDir}/emitterstate.o"    \
         "${buildDir}/emitterutils.o"    \
         "${buildDir}/exceptions.o"      \
         "${buildDir}/exp.o"             \
         "${buildDir}/memory.o"          \
         "${buildDir}/node.o"            \
         "${buildDir}/node_data.o"       \
         "${buildDir}/nodebuilder.o"     \
         "${buildDir}/nodeevents.o"      \
         "${buildDir}/null.o"            \
         "${buildDir}/ostream_wrapper.o" \
         "${buildDir}/parse.o"           \
         "${buildDir}/parser.o"          \
         "${buildDir}/regex_yaml.o"      \
         "${buildDir}/scanner.o"         \
         "${buildDir}/scanscalar.o"      \
         "${buildDir}/scantag.o"         \
         "${buildDir}/scantoken.o"       \
         "${buildDir}/simplekey.o"       \
         "${buildDir}/singledocparser.o" \
         "${buildDir}/stream.o"          \
         "${buildDir}/tag.o"
}

buildMSVC="
cl.exe ${msvcCompilerFlags} /exportHeader                                 ^
                            /I\"${projectDir}/include\"                   ^
                            /ifcOutput \"${buildDir}/${project}.hpp.ifc\" ^
                            /Fo\"${buildDir}/${project}.hpp.obj\"         ^
                            /c /TP \"${projectDir}/${project}.hpp\" || exit 1

cl.exe ${msvcCompilerFlags}                     ^
       /interface                               ^
       /ifcSearchDir \"${buildDir}\"            ^
       /ifcOutput\"${buildDir}/${project}.ifc\" ^
       /Fo\"${buildDir}/${project}.obj\"        ^
       /c \"${projectDir}/${project}.cpp\" || exit 1

cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/binary.obj\"          /c \"${projectDir}/src/binary.cpp\" /wd4244 || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/convert.obj\"         /c \"${projectDir}/src/convert.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/depthguard.obj\"      /c \"${projectDir}/src/depthguard.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/directives.obj\"      /c \"${projectDir}/src/directives.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/emit.obj\"            /c \"${projectDir}/src/emit.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/emitfromevents.obj\"  /c \"${projectDir}/src/emitfromevents.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/emitter.obj\"         /c \"${projectDir}/src/emitter.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/emitterstate.obj\"    /c \"${projectDir}/src/emitterstate.cpp\" /wd4702 || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/emitterutils.obj\"    /c \"${projectDir}/src/emitterutils.cpp\" /wd4244 || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/exceptions.obj\"      /c \"${projectDir}/src/exceptions.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/exp.obj\"             /c \"${projectDir}/src/exp.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/memory.obj\"          /c \"${projectDir}/src/memory.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/node.obj\"            /c \"${projectDir}/src/node.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/node_data.obj\"       /c \"${projectDir}/src/node_data.cpp\" /wd4702 || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/nodebuilder.obj\"     /c \"${projectDir}/src/nodebuilder.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/nodeevents.obj\"      /c \"${projectDir}/src/nodeevents.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/null.obj\"            /c \"${projectDir}/src/null.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/ostream_wrapper.obj\" /c \"${projectDir}/src/ostream_wrapper.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/parse.obj\"           /c \"${projectDir}/src/parse.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/parser.obj\"          /c \"${projectDir}/src/parser.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/regex_yaml.obj\"      /c \"${projectDir}/src/regex_yaml.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/scanner.obj\"         /c \"${projectDir}/src/scanner.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/scanscalar.obj\"      /c \"${projectDir}/src/scanscalar.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/scantag.obj\"         /c \"${projectDir}/src/scantag.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/scantoken.obj\"       /c \"${projectDir}/src/scantoken.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/simplekey.obj\"       /c \"${projectDir}/src/simplekey.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/singledocparser.obj\" /c \"${projectDir}/src/singledocparser.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/stream.obj\"          /c \"${projectDir}/src/stream.cpp\" || exit 1
cl.exe ${msvcCompilerFlags} /I\"${projectDir}/include\" /Fo\"${buildDir}/tag.obj\"             /c \"${projectDir}/src/tag.cpp\" || exit 1

lib.exe /NOLOGO                             ^
        /OUT:\"${buildDir}/${project}.lib\" ^
        \"${buildDir}/${project}.hpp.obj\"  ^
        \"${buildDir}/${project}.obj\"      ^
        \"${buildDir}/binary.obj\"          ^
        \"${buildDir}/convert.obj\"         ^
        \"${buildDir}/depthguard.obj\"      ^
        \"${buildDir}/directives.obj\"      ^
        \"${buildDir}/emit.obj\"            ^
        \"${buildDir}/emitfromevents.obj\"  ^
        \"${buildDir}/emitter.obj\"         ^
        \"${buildDir}/emitterstate.obj\"    ^
        \"${buildDir}/emitterutils.obj\"    ^
        \"${buildDir}/exceptions.obj\"      ^
        \"${buildDir}/exp.obj\"             ^
        \"${buildDir}/memory.obj\"          ^
        \"${buildDir}/node.obj\"            ^
        \"${buildDir}/node_data.obj\"       ^
        \"${buildDir}/nodebuilder.obj\"     ^
        \"${buildDir}/nodeevents.obj\"      ^
        \"${buildDir}/null.obj\"            ^
        \"${buildDir}/ostream_wrapper.obj\" ^
        \"${buildDir}/parse.obj\"           ^
        \"${buildDir}/parser.obj\"          ^
        \"${buildDir}/regex_yaml.obj\"      ^
        \"${buildDir}/scanner.obj\"         ^
        \"${buildDir}/scanscalar.obj\"      ^
        \"${buildDir}/scantag.obj\"         ^
        \"${buildDir}/scantoken.obj\"       ^
        \"${buildDir}/simplekey.obj\"       ^
        \"${buildDir}/singledocparser.obj\" ^
        \"${buildDir}/stream.obj\"          ^
        \"${buildDir}/tag.obj\" || exit 1
"

build
