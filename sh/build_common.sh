source "sh/common.sh"

toolchain="${1}"
buildRoot="build/${toolchain}"
binDir="${buildRoot}/bin"
libCppMsanRoot="${home}/libc++-msan"

clangCompilerFlagsCommon="-std=c++20 \
                          -Wall \
                          -Wextra \
                          -Werror \
                          -pedantic \
                          -Wno-missing-field-initializers \
                          -fmodules \
                          -fimplicit-module-maps \
                          -fprebuilt-module-path=${buildRoot}/astl \
                          -fmodule-map-file=projects/astl/module.modulemap \
                          -fmodule-map-file=projects/awinapi/module.modulemap \
                          -fmodule-map-file=projects/aprocess/module.modulemap \
                          -fmodule-map-file=projects/yamlcpp/module.modulemap"

if is_windows; then
    clangCompilerFlags="${clangCompilerFlagsCommon}"
else
    clangCompilerFlags="${clangCompilerFlagsCommon} \
                        -stdlib=libc++"
fi

if [[ "${ADDRESS_SANITIZER}" == "true" ]]; then
    clangCompilerFlags="${clangCompilerFlags} \
                        -g \
                        -fsanitize=address \
                        -fsanitize-address-use-after-return=always \
                        -fsanitize-address-use-after-scope \
                        -fno-omit-frame-pointer \
                        -fno-optimize-sibling-calls"
elif [[ "${CODE_COVERAGE}" == "true" ]]; then
    clangCompilerFlags="${clangCompilerFlags} \
                        -fprofile-instr-generate \
                        -fcoverage-mapping"
elif [[ "${MEMORY_SANITIZER}" == "true" ]]; then
    if [[ ! -d "${libCppMsanRoot}" ]]; then
        print_error "ERROR: libc++ with memory sanitizer not found. Build it with './adev.sh build libc++-msan'."
        exit 1
    fi

    clangCompilerFlags="${clangCompilerFlagsCommon} \
                        -g \
                        -Wno-error=unused-command-line-argument
                        -fsanitize=memory \
                        -fno-omit-frame-pointer \
                        -fno-optimize-sibling-calls \
                        -fsanitize-memory-track-origins \
                        -fsanitize-memory-use-after-dtor \
                        -nostdinc++ \
                        -I${libCppMsanRoot}/include/c++/v1"
elif [[ "${THREAD_SANITIZER}" == "true" ]]; then
    clangCompilerFlags="${clangCompilerFlags} \
                        -g \
                        -O1 \
                        -fsanitize=thread"
elif [[ "${UNDEFINED_SANITIZER}" == "true" ]]; then
    clangCompilerFlags="${clangCompilerFlags} \
                        -fsanitize=undefined"
else
    clangCompilerFlags="${clangCompilerFlags} \
                        -O3"
fi

if is_windows; then
    clangCompilerLinkerFlags="${clangCompilerFlags}"
else
    clangCompilerLinkerFlags="${clangCompilerFlags} \
                              -lpthread"
fi

if [[ "${MEMORY_SANITIZER}" == "true" ]]; then
    clangCompilerLinkerFlags="${clangCompilerLinkerFlags} \
                              -L${libCppMsanRoot}/lib \
                              -lc++ \
                              -lc++abi \
                              -Wl,-rpath,${libCppMsanRoot}/lib"
fi

gccCompilerFlags="-std=c++20 \
                  -Wall \
                  -Werror \
                  -pedantic-errors \
                  -fmodules-ts"

msvcCompilerFlags="/nologo \
                   /std:c++latest \
                   /EHsc \
                   /MT \
                   /O2 \
                   /W4 \
                   /WX \
                   /wd4005 \
                   /wd5105 \
                   /wd5106 \
                   /ifcSearchDir \"${buildRoot}/astl\" \
                   /headerUnit \"projects/astl/astl.hpp=${buildRoot}/astl/astl.hpp.ifc\" \
                   /headerUnit \"projects/awinapi/windows.hpp=${buildRoot}/awinapi/windows.hpp.ifc\" "

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
        echo "${satus}" > "${buildRoot}/${project}.done"
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

    msvcEnvScript="C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Auxiliary/Build/vcvars64.bat"
        
    if [[ -f "${msvcEnvScript}" ]]; then
        return
    fi

    msvcEnvScript="C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Auxiliary/Build/vcvars64.bat"
        
    if [[ -f "${msvcEnvScript}" ]]; then
        return
    fi

    msvcEnvScript="C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat"
        
    if [[ -f "${msvcEnvScript}" ]]; then
        return
    fi

    print_error "ERROR: Visual Studio 2022 environment script not found."
    exit 1
}
