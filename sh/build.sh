source "sh/common.sh"

function build_project() {
    local project="${1}"
    local buildScript="sh/build_${project}.sh"

    if [[ -f "${buildScript}" ]]; then
        run_script "${buildScript}" "${toolcahin}"
    else
        echo ""
        print_error "ERROR: Project '${project}' does not exist."
        echo "
Available projects:"
        listProjects
        exit 1;
    fi
}

function build_projects() {
    set_toolchain "${1}"

    for script in sh/build/*.sh; do
        if [[ "${script}" =~ sh/build/(.*)\.sh ]]; then
            build_project "${BASH_REMATCH[1]}"
        fi
    done
}

function build_projects_diff() {
    local toolchain="${1}"

    if ! is_changed "*.cpp *hpp"; then
        print_ok "No sources changed from 'main'. Nothing to build."
        exit 0
    fi

    build_projects "${toolchain}"
}

function list_projects() {
    remove_done_files

    for script in sh/build/*.sh; do
        if [[ "${script}" =~ sh/build/(.*)\.sh ]]; then
            echo "  ${BASH_REMATCH[1]}"
        fi
    done
}

function remove_done_files() {
    if [[ -d "build/${toolchain}/" ]]; then
        find build/${toolchain}/ -name "*.done" -type f -delete
    fi
}

if [[ "${1}" == "list" ]]; then
    list_projects
elif [[ "${1}" == "docs" ]]; then
    sh/build_docs.sh "${2}"
elif [[ "${1}" == "diff" ]]; then
    build_projects_diff "${2}"
elif is_toolchain "${1}"; then
    build_projects "${1}"
elif [[ "${1}" != "" ]]; then
    set_toolchain "${2}"
    remove_done_files
    build_project "${1}"
else
    build_projects
fi
