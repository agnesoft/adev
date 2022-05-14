[ -n "$BUILD_COMMON_SH" ] && return || readonly BUILD_COMMON_SH=1

source "sh/common.sh"

function build_project() {
    local -r buildScript="sh/build/${1}.sh"

    if [[ -f "${buildScript}" ]]; then
        run_script "${buildScript}" "${toolchain}" "${configuration}"
    else
        print_error "ERROR: Project '${1}' does not exist."
        exit 1;
    fi
}

function build() {
    if [[ -f "${buildRoot}/${project}.done" ]]; then
        return
    fi

    build_dependencies "${1}"

    echo "*** ${project} | ${toolchain} | ${configuration} ***"

    mkdir -p "${binDir}"
    mkdir -p "${buildDir}"

    if [[ "${toolchain}" == "clang" ]]; then
        set -e
        build_clang
        readonly status=$?
    elif [[ "${toolchain}" == "gcc" ]]; then
        set -e
        build_gcc
        readonly status=$?
    elif [[ "${toolchain}" == "msvc" ]]; then
        build_msvc
    fi

    if (( $status == 0 )); then
        echo "${satus}" > "${buildRoot}/${project}.done"
    else
        print_error "ERROR: build of '${project}' failed"
        exit 1
    fi
}

function build_dependencies() {
    local -r dependencies="${1}"

    for dependency in ${dependencies[@]}; do
        build_project "${dependency}"
    done
}

function is_clang_configuration() {
    [[ "${configuration}" == "memory" ]] || [[ "${configuration}" == "coverage" ]]
}

function set_build_properties() {
    set_build_arguments "${1}" "${2}" "${3}"
    detect_msvc_env_script
    set_build_dirs
    set_build_flags
}

function set_build_dirs() {
    readonly buildRoot="build/${toolchain}/${configuration}"
    readonly binDir="${buildRoot}/bin"

    if [[ "${project}" == *.test ]]; then
        readonly projectDir="projects/${project:: -5}/test"
        readonly buildDir="${buildRoot}/${project:: -5}/test"
    else
        readonly projectDir="projects/${project}"
        readonly buildDir="${buildRoot}/${project}"
        
    fi
}

function set_build_arguments() {
    if [[ "${1}" != "" ]]; then
        set_first_argument "${1}" "${2}" "${3}"
    fi

    validate_toolchain
    validate_configuration
}

function set_first_argument() {
    if is_toolchain "${1}"; then
        readonly toolchain="${1}"
    elif is_configuration "${1}"; then
        readonly configuration="${1}"
    elif ! is_number $1; then
        readonly project="${1}"
    fi

    if [[ "${2}" != "" ]]; then
        set_second_argument "${2}" "${3}"
    fi
}

function set_second_argument() {
    if is_toolchain "${1}"; then
        readonly toolchain="${1}"
    elif is_configuration "${1}"; then
        readonly configuration="${1}"
    elif ! is_number $1; then
        print_error "ERROR: second build argument '${1}' is not a toolchain or configuration"
        exit 1
    fi

    if [[ "${2}" != "" ]]; then
        set_third_argument "${2}"
    fi
}

function set_third_argument() {
    if is_configuration "${1}"; then
        readonly configuration="${1}"
    elif ! is_number $1; then
        print_error "ERROR: third build argument '${1}' is not a configuration"
        exit 1
    fi
}

function validate_toolchain() {
    if ! is_linux && [[ "${toolchain}" == "gcc" ]]; then
        print_error "ERROR: 'gcc' toolchain is only available on Linux"
        exit 1
    elif ! is_windows && [[ "${toolchain}" == "msvc" ]]; then
        print_error "ERROR: 'msvc' toolchain is only available on Windows"
        exit 1
    fi

    if [[ "${toolchain}" == "" ]]; then
        if is_clang_configuration; then
            readonly toolchain="clang"
        elif is_windows; then
            readonly toolchain="msvc"
        elif is_linux; then
            readonly toolchain="gcc"
        else
            readonly toolchain="clang"
        fi
    fi
}

function validate_configuration() {
    if [[ "${configuration}" == "memory" ]]; then
        if [[ ! -d "${libCppMsanRoot}" ]]; then
            print_error "ERROR: libc++ with memory sanitizer not found. Build it with './build.sh libc++-msan'."
            exit 1
        fi
    elif [[ "${configuration}" == "" ]]; then
        readonly configuration="release"
    fi
}

function set_clang_common_flags() {
    readonly clangCommonFlags=\
"-std=c++20 \
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
        clangCompilerFlags="${clangCommonFlags}"
    else
        clangCompilerFlags="${clangCompilerFlagsCommon} -stdlib=libc++"
    fi
}

function set_clang_release_flags() {
    clangCompilerFlags="${clangCompilerFlags} \
                        -O3"
}

function set_clang_debug_flags() {
    clangCompilerFlags="${clangCompilerFlags} \
                        -g"
}

function set_clang_address_flags() {
    clangCompilerFlags=\
"${clangCompilerFlags} \
-g \
-fsanitize=address \
-fsanitize-address-use-after-return=always \
-fsanitize-address-use-after-scope \
-fno-omit-frame-pointer \
-fno-optimize-sibling-calls"
}

function set_clang_memory_flags() {
    clangCompilerFlags=\
"${clangCommonFlags} \
-g \
-Wno-error=unused-command-line-argument
-fsanitize=memory \
-fno-omit-frame-pointer \
-fno-optimize-sibling-calls \
-fsanitize-memory-track-origins \
-fsanitize-memory-use-after-dtor \
-nostdinc++ \
-I${libCppMsanRoot}/include/c++/v1"
}

function set_clang_thread_flags() {
    clangCompilerFlags="${clangCompilerFlags} -g -O1 -fsanitize=thread"
}

function set_clang_undefined_flags() {
    clangCompilerFlags="${clangCompilerFlags} -fsanitize=undefined"
}

function set_clang_coverage_flags() {
    clangCompilerFlags="${clangCompilerFlags} -fprofile-instr-generate -fcoverage-mapping"
}

function set_clang_compiler_linker_flags() {
    if is_windows; then
        readonly clangCompilerLinkerFlags="${clangCompilerFlags}"
    elif [[ "${configuration}" == "memory" ]]; then
        readonly clangCompilerLinkerFlags=\
"${clangCompilerLinkerFlags} \
-L${libCppMsanRoot}/lib \
-lc++ \
-lc++abi \
-lpthread \
-Wl,-rpath,${libCppMsanRoot}/lib"
    else
        readonly clangCompilerLinkerFlags="${clangCompilerFlags} -lpthread"
    fi
}

function set_clang_flags() {
    set_clang_common_flags

    if [[ "${configuration}" == "release" ]]; then
        set_clang_release_flags
    elif [[ "${configuration}" == "debug" ]]; then
        set_clang_debug_flags
    elif [[ "${configuration}" == "address" ]]; then
        set_clang_address_flags
    elif [[ "${configuration}" == "coverage" ]]; then
        set_clang_coverage_flags
    elif [[ "${configuration}" == "memory" ]]; then
        set_clang_memory_flags
    elif [[ "${configuration}" == "thread" ]]; then
        set_clang_thread_flags
    elif [[ "${configuration}" == "undefined" ]]; then
        set_clang_undefined_flags
    fi

    set_clang_compiler_linker_flags
}

function set_gcc_flags() {
    readonly gccCompilerFlags=\
"-std=c++20 \
-Wall \
-Werror \
-pedantic-errors \
-fmodules-ts"
}

function set_msvc_flags() {
    readonly msvcCommonFlags=\
"/nologo \
/std:c++latest \
/EHsc \
/MT \
/W4 \
/WX \
/wd4005 \
/wd5105 \
/wd5106 \
/ifcSearchDir \"${buildRoot}/astl\" \
/headerUnit \"projects/astl/astl.hpp=${buildRoot}/astl/astl.hpp.ifc\" \
/headerUnit \"projects/awinapi/windows.hpp=${buildRoot}/awinapi/windows.hpp.ifc\" \
/headerUnit \"projects/yamlcpp/yamlcpp.hpp=${buildRoot}/yamlcpp/yamlcpp.hpp.ifc\""

    if [[ "${configuration}" == "release" ]]; then
        readonly msvcCompilerFlags="${msvcCommonFlags} /O2"
    elif [[ "${configuration}" == "debug" ]]; then
        readonly msvcCompilerFlags="${msvcCommonFlags} /Z7"
    fi
}

function set_build_flags() {
    if [[ "${toolchain}" == "clang" ]]; then
        set_clang_flags
    elif [[ "${toolchain}" == "gcc" ]]; then
        set_gcc_flags
    elif [[ "${toolchain}" == "msvc" ]]; then
        set_msvc_flags
    fi
}



function build_msvc() {
    echo "@echo off
call \"${msvcEnvScript}\" >nul
${buildMSVC}" > build.bat

    local log=""

    if log=$(cmd //c build.bat); then
        readonly status=0
    else
        readonly status=1
        echo "Failed: ${log}"
    fi

    rm build.bat
}

function detect_msvc_env_script() {
    if [[ "${toolchain}" != "msvc" ]] || [[ "${msvcEnvScript}" != "" ]]; then
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
