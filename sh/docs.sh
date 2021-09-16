source "sh/common.sh"

function detect_doxygen () {
    if is_available "doxygen"; then
        doxygen="doxygen"
    elif is_windows && [[ -f "C:/Program Files/doxygen/bin/doxygen.exe" ]]; then
        doxygen="C:/Program Files/doxygen/bin/doxygen.exe"
    else
        print_error "ERROR: 'doxygen' is not available. Try installing it with './adev.sh install doxygen'."
        exit 1
    fi

    echo "Doxygen $("$doxygen" --version)"
}

function build_docs () {
    echo "Generating documentation..."
    mkdir -p "build"
    "${doxygen}" adev.doxyfile
    
    if (( $? != 0 )); then
        print_error "ERROR: Building documentation failed."
        exit 1
    else
        print_ok "done"
    fi
}

detectDoxygen
build_docs
