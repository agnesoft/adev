source "sh/common.sh"

function detect_clang_format() {
    if is_windows; then
        clangFormat="clang-format.exe"
    else
        clangFormat="clang-format-12"
    fi

    if ! is_available "${clangFormat}"; then
        print_error "ERROR: ${clangFormat} is not available. Please install it with './adev.sh install clang-format'."
    else
        $clangFormat --version | head -n 1
    fi
}

function format_check_source() {
    local source="${1}"
    local replacements=$($clangFormat -output-replacements-xml $source | grep "<replacement ")

    if [[ "${replacements}" != "" ]]; then
        print_error "[ FAILED ] $source"
        result=$(( $result + 1 ))
    else
        print_ok "[ PASSED ] $source"
    fi
}

function format_check_diff() {
    local sources=$(git diff main --name-only -- *.cpp *.hpp)

    for file in ${sources[@]}; do
        format_check_source "${file}"
    done
}

function format_check_all() {
    for file in projects/**/*.{cpp,hpp}; do
        format_check_source "${file}"
    done
}

function format_check() {
    local diff="${1}"
    result=0

    if [[ "${diff}" == "diff" ]]; then
        format_check_diff
    else
        format_check_all
    fi

    if (( $result != 0 )); then
        print_error "Formatting checks FAILED"
        print_error "Run './adev.sh format' and commit the result"
    else
        print_ok "Formatting checks PASSED"
    fi

    exit 1
}

function format() {
    for file in projects/**/*.{cpp,hpp}; do
        $clangFormat -i "${file}"
        print_ok "${file}"
    done
}

detect_clang_format

if [[ "${1}" == "check" ]]; then
    format_check "${2}"
else
    format
fi
