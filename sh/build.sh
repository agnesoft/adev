source "sh/common.sh"

readonly PROJECT_NAME_FROM_SCRIPT_PATTERN=".*build_(.*)\.sh"

action=
project=
toolchain=

function list_projects() {
    for script in sh/build_*.sh; do
        if [[ "${script}" =~ sh/build_(.*)\.sh ]]; then
            echo "  ${BASH_REMATCH[1]}"
        fi
    done
}

function build_all() {
    echo "Toolchain: $toolchain"
    echo ""

    for script in sh/build_*.sh; do
        if [[ "${script}" =~ sh/build_(.*)\.sh ]]; then
            run_build_script $script ${BASH_REMATCH[1]}
        fi
    done
}

function build_project() {
    echo "Toolchain: $toolchain"
    echo ""

    local buildScript="sh/build_$project.sh"

    if [[ -f "${buildScript}" ]]; then
        run_build_script $buildScript $project
    else
        echo ""
        print_error "ERROR: Project '$project' does not exist."
        echo "
Available projects:"
        listProjects
        exit 1;
    fi
}

function remove_done_files() {
    if [[ -d "build/$toolchain/" ]]; then
        find build/$toolchain/ -name "*.done" -type f -delete
    fi
}

function run_build_script() {
    local buildScript="${1}"
    local project="${2}"
    local status=0

    $buildScript $toolchain
    status=$?

    if (( $status != 0 )); then
        print_error "ERROR: Building project '${project}' (${buildScript}) failed: ${status}"
        exit 1
    fi
}

function set_properties() {
    if [[ "${1}" == "list" ]]; then
        action="list"
    elif [[ "${1}" == "docs" ]]; then
        action="docs"
    elif [[ "${1}" == "clang" ]] || [[ "${1}" == "msvc" ]] || [[ "${1}" == "gcc" ]] || [[ "${1}" == "" ]]; then
        action="buildAll"
        toolchain=$1
    elif [[ "${2}" == "clang" ]] || [[ "${2}" == "msvc" ]] || [[ "${2}" == "gcc" ]] || [[ "${2}" == "" ]]; then
        action="buildProject"
        project=$1
        toolchain=$2
    fi

    if [[ "${toolchain}" == "" ]]; then
        if is_windows; then
            toolchain="msvc"
        else
            toolchain="gcc"
        fi
    fi

    if ! is_windows && [[ "${toolchain}" == "msvc" ]]; then
        print_error "ERROR: toolchain 'msvc' is only available on Windows"
        exit 1
    fi

    if ! is_linux && [[ "$toolchain" == "gcc" ]]; then
        print_error "ERROR: toolchain 'gcc' is only available on Linux"
        exit 1
    fi
}

set_properties $1 $2
remove_done_files

if test "$action" == "list"; then
    list_projects
elif test "$action" == "docs"; then
    sh/docs.sh
elif test "$action" == "buildAll"; then
    build_all
elif test "$action" == "buildProject"; then
    build_project
else
    print_error "ERROR: unknown action '${action}'"
    exit 1
fi
