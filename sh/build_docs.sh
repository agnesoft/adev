[ -n "$BUILD_DOCS_SH" ] && return || readonly BUILD_DOCS_SH=1

source "sh/common.sh"

function build_docs () {
    if is_available "doxygen"; then
        local -r doxygen="doxygen"
    elif is_windows && [[ -f "C:/Program Files/doxygen/bin/doxygen.exe" ]]; then
        local -r doxygen="C:/Program Files/doxygen/bin/doxygen.exe"
    else
        print_error "ERROR: 'doxygen' is not available. Try installing it with './build.sh install doxygen'."
        exit 1
    fi

    mkdir -p "build"

    echo "Doxygen $("$doxygen" --version)"

    if $("${doxygen}" .doxyfile); then
        print_error "ERROR: Building documentation failed."
        exit 1
    fi
}
