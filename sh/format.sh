source ./sh/common.sh

clangFormat=

function detect_clang_format() {
    if is_windows; then
        clangFormat="clang-format.exe"
    else
        clangFormat="clang-format-12"
    fi

    if ! is_available $clangFormat; then
        print_error "ERROR: ${clangFormat} is not available. Please install it with './adev.sh install clang-format'."
    fi
}

function format_source() {
    local source=$1
    $clangFormat -i $source
}

function format_sources() {
    for file in projects/**/*.{cpp,hpp}; do
        format_source $file
    done

    if [[ "$(git status --short)" != "" ]]; then
        exit 1
    else
        exit 0
    fi
}

detect_clang_format
format_sources
