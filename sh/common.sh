[ -n "$COMMON_SH" ] && return || readonly COMMON_SH=1

function is_available() {
    local cmd=$(command -v $1)
    [[ "${cmd}" ]]
}

function is_changed() {
    local diff=$(git diff origin/main --name-only -- ${1})
    [[ "${diff}" != "" ]]
}

function is_configuration() {
    [[ "${1}" == "release" ]] \
    || [[ "${1}" == "debug" ]] \
    || [[ "${1}" == "coverage" ]] \
    || [[ "${1}" == "address" ]] \
    || [[ "${1}" == "memory" ]] \
    || [[ "${1}" == "thread" ]] \
    || [[ "${1}" == "undefined" ]]
}

function is_linux() { 
    [[ $OSTYPE == "linux-gnu" ]]
}

function is_number() {
    [[ "${1}" =~ ^[0-9]+$ ]]
}

function is_toolchain() {
    [[ "${1}" == "clang" ]] \
    || [[ "${1}" == "msvc" ]] \
    || [[ "${1}" == "gcc" ]]
}

function is_windows() { 
    [[ $OSTYPE == "msys" ]] || [[ $OSTYPE == "cygwin" ]]
}

function print_error() {
    echo -e "\033[31m${1}\033[0m"
}

function print_ok() {
    echo -e "\033[32m${1}\033[0m"
}

function run_script() {
    local -r script="${1}"
    local -r arg1="${2}"
    local -r arg2="${3}"
    local -r arg3="${4}"

    "${script}" "${arg1}" "${arg2}" "${arg3}"

    if (( $? != 0 )); then
        exit 1
    fi
}

function set_host_properties() {
    if is_windows; then
        set_windows_properties
    elif is_linux; then
        set_linux_properties
    else
        set_unix_properties
    fi
}

function set_linux_properties() {
    readonly home="/adev"
    readonly llvmVersion=14
    readonly gccVersion=12
    readonly clang="clang++-${llvmVersion}"
    readonly clangFormat="clang-format-${llvmVersion}"
    readonly clangTidy="clang-tidy-${llvmVersion}"
    readonly lld="lld-${llvmVersion}"
    readonly llvmCov="llvm-cov-${llvmVersion}"
    readonly llvmProfdata="llvm-profdata-${llvmVersion}"
    readonly gcc="g++-${gccVersion}"
    readonly ar="ar"
    readonly libCppMsanRoot="${home}/libc++-msan"
}

function set_unix_properties() {
    readonly clang="clang++"
    readonly clangFormat="clang-format"
    readonly clangTidy="clang-tidy"
    readonly llvmCov="llvm-cov"
    readonly llvmProfdata="llvm-profdata"
    readonly ar="ar"
}

function set_windows_properties() {
    readonly executableExtension=".exe"
    readonly clang="clang++${executableExtension}"
    readonly clangFormat="clang-format${executableExtension}"
    readonly clangTidy="clang-tidy${executableExtension}"
    readonly llvmCov="llvm-cov${executableExtension}"
    readonly llvmProfdata="llvm-profdata${executableExtension}"
    readonly ar="llvm-ar${executableExtension}"
}

shopt -s globstar

set_host_properties
