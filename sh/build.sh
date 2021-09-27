source "sh/common.sh"

function build_project() {
    local project="${1}"
    local buildScript="sh/build/${project}.sh"

    if [[ -f "${buildScript}" ]]; then
        run_script "${buildScript}" "${toolchain}"
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

    remove_done_files

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
    if [[ -d "build/${toolchain}/" ]]; then
        find build/${toolchain}/ -name "*.done" -type f -delete
    fi
}

if [[ "${1}" == "coverage" ]]; then
    export CODE_COVERAGE="true"
    build_projects "clang"
elif [[ "${1}" == "docs" ]]; then
    sh/build_docs.sh
elif [[ "${1}" == "list" ]]; then
    list_projects
elif is_toolchain "${1}"; then
    build_projects "${1}"
elif [[ "${1}" != "" ]]; then
    set_toolchain "${2}"
    remove_done_files
    build_project "${1}"
else
    build_projects
fi
