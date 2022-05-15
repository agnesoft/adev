[ -n "$BUILD_SH" ] && return || readonly BUILD_SH=1

source "sh/build_docs.sh"
source "sh/build_libc++_msan.sh"

function build_() {
    if [[ "${1}" == "docs" ]]; then
        build_docs
        return
    fi

    if [[ "${1}" == "libc++-msan" ]]; then
        build_libcpp_msan
        return
    fi

    if [[ "${1}" == "list" ]]; then
        list_projects
        return
    fi

    set_build_properties "${1}" "${2}" "${3}"

    if [[ "${project}" != "" ]]; then
        rm -f "build/${toolchain}/${configuration}/${project}.done"
        build_project "${project}"
    else
        build_projects
    fi
}

function build_projects() {
    remove_done_files "${1}" "${2}"

    for script in sh/build/*.sh; do
        if [[ "${script}" =~ sh/build/(.*)\.sh ]]; then
            build_project "${BASH_REMATCH[1]}"
        fi
    done
}

function list_projects() {
    for script in sh/build/*.sh; do
        if [[ "${script}" =~ sh/build/(.*)\.sh ]]; then
            echo "  ${BASH_REMATCH[1]}"
        fi
    done
}

function remove_done_files() {
    if [[ -d "build/${toolchain}/${configuration}" ]]; then
        find "build/${toolchain}/${configuration}" -name "*.done" -type f -delete
    fi
}
