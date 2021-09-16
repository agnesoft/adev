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
                    -fprebuilt-module-path=\"${buildRoot}/astl\" \
                    -fmodule-map-file=projects/astl/module.modulemap"

clangCompilerLinkerFlags="${clangCompilerFlags} \
                          -lpthread"

gccCompilerFlags="-std=c++20 \
                  -Wall \
                  -Werror \
                  -pedantic-errors \
                  -fmodules-ts"

msvcCompilerFlags="/nologo ^
                   /std:c++latest ^
                   /EHsc ^
                   /O2 ^
                   /W4 ^
                   /WX ^
                   /ifcSearchDir \"${buildRoot}/astl\" ^
                   /headerUnit \"projects/astl/astl.hpp=${buildRoot}/astl/astl.hpp.ifc\""

function build() {
    if [[ "${buildRoot}/${project}.done" ]]; then
        exit $status
    fi
    
    echo "*** ${project} ***"

    if [[ "${toolchain}" == "clang" ]]; then
        build_clang "${clangBuild}"
    elif [[ "${toolchain}" == "gcc" ]]; then
        build_gcc "${gccBuild}"
    elif [[ "${toolchain}" == "msvc" ]]; then
        build_msvc "${msvcBuild}"
    else
        print_error "ERROR: Unknown toolchain '${toolchain}'"
        status=1
    fi

    if (( $status == 0 )); then
        touch "${buildRoot}/${project}.done"
    fi

    exit $status
}

function build_clang() {
    local buildScript="
mkdir -p \"${binDir}\"
mkdir -p \"${buildDir}\"
$1"

    "${buildScript}"
    status=$?
}

function build_gcc() {
    local buildScript="
mkdir -p \"${binDir}\"
mkdir -p \"${buildDir}\"
$1"

    "${buildScript}"
    status=$?
}

function build_msvc() {
    detect_msvc_env_script

    local buildScript="
@echo off
call \"${msvcEnvScript}\" >nul
if not exist \"${binDir}\" mkdir \"${binDir}\" >nul
if not exist \"${buildDir}\" mkdir \"${buildDir}\" >nul
$1"

    echo "${buildScript}" > build.bat
    cmd //c build.bat
    status=$?
    rm build.bat
}

function detect_msvc_env_script() {
    if ! [[ "${msvcEnvScript}" == "" ]]; then
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
