function print_error() {
    echo -e "\033[31m${1}\033[0m"
}

function print_ok() {
    echo -e "\033[32m${1}\033[0m"
}

function is_available() {
    local cmd=$(command -v $1)
    [[ "${cmd}" ]]
}

function is_changed() {
    local diff=$(git diff main --name-only -- ${1})
    [[ "${diff}" != "" ]]
}

function is_linux() { 
    [[ $OSTYPE == "linux-gnu" ]]
}

function is_number() {
    test "${1}" ~= ^[0-9]+$
}

function is_toolchain() {
    [[ "${1}" == "clang" ]] || [[ "${1}" == "msvc" ]] || [[ "${1}" == "gcc" ]]
}

function is_windows() { 
    [[ $OSTYPE == "msys" ]] || [[ $OSTYPE == "cygwin" ]]
}

function set_toolchain() {
    if is_toolchain "${1}"; then
        toolchain="${1}"
    elif is_linux; then
        toolchain="gcc"
    elif is_windows; then
        toolchain="msvc"
    else
        toolchain="clang"
    fi
}

function wait_for_jobs() {
    result=0

    for job in $(jobs -p)
    do
        wait $job || ((result=1))
    done
}

shopt -s globstar

if is_windows; then
    executableExtension=".exe"
fi
