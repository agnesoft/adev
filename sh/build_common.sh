source "sh/common.sh"

toolchain="${1}"
buildRoot="build/${toolchain}"
binDir="${buildRoot}/bin"

clangCompilerFlags="-std=c++20 \
                    -Wall \
                    -Wextra \
                    -Werror \
                    -pedantic \
                    -Wno-missing-field-initializers \
                    -fmodules \
                    -fimplicit-module-maps \
                    -stdlib=libc++ \
                    -fprebuilt-module-path=${buildRoot}/astl \
                    -fmodule-map-file=projects/astl/module.modulemap"

if [[ "${CODE_COVERAGE}" == "true" ]]; then
    clangCompilerFlags="${clangCompilerFlags} \
                        -fprofile-instr-generate \
                        -fcoverage-mapping"
elif [[ "${ADDRESS_SANITIZER}" == "true" ]]; then
    clangCompilerFlags="${clangCompilerFlags} \
                        -g \
                        -fsanitize=address \
                        -fsanitize-address-use-after-return=always \
                        -fsanitize-address-use-after-scope \
                        -fno-omit-frame-pointer \
                        -fno-optimize-sibling-calls"
else
    clangCompilerFlags="${clangCompilerFlags} \
                        -O3"
fi

clangCompilerLinkerFlags="${clangCompilerFlags} \
                          -lpthread"

gccCompilerFlags="-std=c++20 \
                  -Wall \
                  -Werror \
                  -pedantic-errors \
                  -fmodules-ts"

msvcCompilerFlags="/nologo \
                   /std:c++latest \
                   /EHsc \
                   /O2 \
                   /W4 \
                   /WX \
                   /ifcSearchDir \"${buildRoot}/astl\" \
                   /headerUnit \"projects/astl/astl.hpp=${buildRoot}/astl/astl.hpp.ifc\" "

function build() {
    if [[ -f "${buildRoot}/${project}.done" ]]; then
        exit 0
    fi

    echo "*** ${project} ***"

    mkdir -p "${binDir}"
    mkdir -p "${buildDir}"

    if [[ "${toolchain}" == "clang" ]]; then
        set -e
        build_clang
        status=$?
    elif [[ "${toolchain}" == "gcc" ]]; then
        set -e
        build_gcc
        status=$?
    elif [[ "${toolchain}" == "msvc" ]]; then
        build_msvc
    else
        print_error "ERROR: Unknown toolchain '${toolchain}'"
        status=1
    fi

    if (( $status == 0 )); then
        touch "${buildRoot}/${project}.done"
    fi

    exit $status
}

function build_msvc() {
    detect_msvc_env_script

    local buildScript="
@echo off
call \"${msvcEnvScript}\" >nul
${buildMSVC}"

    echo "${buildScript}" > build.bat
    cmd //c build.bat
    status=$?
    rm build.bat
}

function detect_msvc_env_script() {
    if [[ "${msvcEnvScript}" != "" ]]; then
        return
    fi
    
    msvcEnvScript="C:/Program Files (x86)/Microsoft Visual Studio/2019/Enterprise/VC/Auxiliary/Build/vcvars64.bat"
        
    if [[ -f "${msvcEnvScript}" ]]; then
        return
    fi

    msvcEnvScript="C:/Program Files (x86)/Microsoft Visual Studio/2019/Professional/VC/Auxiliary/Build/vcvars64.bat"
    
    if [[ -f "${msvcEnvScript}" ]]; then
        return
    fi

    msvcEnvScript="C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build/vcvars64.bat"
    
    if [[ -f "${msvcEnvScript}" ]]; then
        return
    fi

    print_error "ERROR: Visual Studio 2019 environment script not found."
    exit 1
}
