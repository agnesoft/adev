source "sh/common.sh"

status=0
toolchain=$1
buildRoot="build/${toolchain}"
binDir="${buildRoot}/bin"

if is_windows; then
    executableSuffix=".exe"
    clang="clang++"
else
    clang="clang++-12"
    gcc="g++-11"
fi

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

clangCompilerLinkerFlags="${clangCompilerFlags} \
                          -lpthread"

gccCompilerFlags="-std=c++20 \
                  -Wall -Werror \
                  -pedantic-errors \
                  -fmodules-ts"

msvcCompilerFlags="/nologo \
                   /std:c++latest \
                   /EHsc \
                   /O2 \
                   /W4 \
                   /WX \
                   /ifcSearchDir \"${buildRoot}/astl\" \
                   /headerUnit \"projects/astl/astl.hpp=${buildRoot}/astl/astl.hpp.ifc\""

function build() {
    if [[ -f "${buildRoot}/${project}.done" ]]; then
        exit $status
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
    if [[ "${msvcEnvScript}" == "" ]]; then
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

        print_error "ERROR: Visual Studio environment script not found."
        exit 1
    fi
}