[ -n "$FORMAT_SH" ] && return || readonly FORMAT_SH=1

source "sh/common.sh"

function format() {
    detect_clang_format

    if [[ "${1}" == "check" ]]; then
        result=0
        
        if [[ "${2}" == "diff" ]]; then
            check_formatting_diff
        else
            check_formatting
        fi

        if (( $result != 0 )); then
            print_error "Formatting checks FAILED"
            print_error "Run './build.sh format' and commit the result"
            exit 1
        else
            print_ok "Formatting checks PASSED"
        fi
    else
        format_sources
    fi
}

function check_formatting() {
    for file in projects/**/*.{cpp,hpp}; do
        if ! is_ignored "${file}"; then
            check_formatting_file "${file}"
        fi
    done
}

function check_formatting_diff() {
    for file in projects/**/*.{cpp,hpp}; do
        if ! is_ignored "${file}" && is_changed "${file}"; then
            check_formatting_file "${file}"
        fi
    done
}

function format_sources() {
    for file in projects/**/*.{cpp,hpp}; do
        if ! is_ignored "${file}"; then
            "${clangFormat}" -i "${file}"
            print_ok "${file}"
        fi
    done
}

function detect_clang_format() {
    if ! is_available "${clangFormat}"; then
        print_error "ERROR: ${clangFormat} is not available. Please install it with './adev.sh install llvm'."
    else
        ${clangFormat} --version | head -n 1
    fi
}

function check_formatting_file() {
    local -r file="${1}"
    local -r replacements=$("${clangFormat}" -output-replacements-xml "${file}" | grep "<replacement ")

    if [[ "${replacements}" != "" ]]; then
        print_error "[ FAILED ] ${file}"
        result=1
    else
        print_ok "[ PASSED ] ${file}"
    fi
}
