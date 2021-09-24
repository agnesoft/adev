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

function run_script() {
    local script="${1}"
    local arg1="${2}"
    local arg2="${3}"
    local arg3="${4}"
    "${script[@]}" "${arg1}" "${arg2}" "${arg3}"
    local status=$?

    if (( $status != 0 )); then
        print_error "ERROR: ${script} failed: ${status}"
        exit 1
    fi
}

function set_toolchain() {
    if [[ "${1}" == "" ]]; then
        if is_linux; then
            toolchain="gcc"
        elif is_windows; then
            toolchain="msvc"
        else
            toolchain="clang"
        fi
    elif is_toolchain "${1}"; then
        toolchain="${1}"
    else
        print_error "ERROR: unknown toolchain '${1}'"
        exit 1
    fi

    echo "Toolchain: ${toolchain}"
    echo ""
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
