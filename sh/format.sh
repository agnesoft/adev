source ./sh/common.sh

action=$1
clangFormat=
modified=0

function detect_clang_format() {
    if is_windows; then
        clangFormat="clang-format.exe"
    else
        clangFormat="clang-format-12"
    fi

    if ! is_available $clangFormat; then
        print_error "ERROR: ${clangFormat} is not available. Please install it with './adev.sh install clang-format'."
    else
        $clangFormat --version | head -n 1
    fi
}

function check_source_formatting() {
    local source=$1
    local replacements=$($clangFormat -output-replacements-xml $source | grep "<replacement ")

    if [[ "${replacements}" != "" ]]; then
        print_error "[ FAILED ] $source"
        modified=$(( $modified + 1 ))
    else
        print_ok "[ PASSED ] $source"
    fi
}

function check_formatting() {
    for file in projects/**/*.{cpp,hpp}; do
        check_source_formatting $file
    done

    if (( $modified != 0 )); then
        print_error "Formatting checks FAILED"
        print_error "Run './adev.sh format' and commit the result"
    else
        print_ok "Formatting checks PASSED"
    fi

    exit $modified
}

function format_source() {
    local source=$1
    $clangFormat -i $source
}

function format_sources() {
    for file in projects/**/*.{cpp,hpp}; do
        format_source $file
    done

    print_ok "done"
}

detect_clang_format

if [[ "${action}" == "check" ]]; then
    check_formatting
else
    format_sources
fi
