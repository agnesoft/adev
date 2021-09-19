source sh/common.sh

project="${1}"
clangTidy=
result=0

function detect_clang_tidy() {
    if is_windows; then
        clangTidy="clang-tidy.exe"
    else
        clangTidy="clang-tidy-12"
    fi

    if ! is_available $clangTidy; then
        print_error "ERROR: ${clangTidy} is not available. Please install it with './adev.sh install clang-tidy'."
    else
        $clangTidy --version | head -n 2 | tail -n +2
    fi
}

function analyse_source() {
    local source="${1}"
    local headerFilter="${2}"
    local log
    log=$(${clangTidy} --quiet ${headerFilter} ${source} 2>&1)

    if (( $? != 0 )); then
        echo "${log}"
        print_error "[ FAILED ] ${source}"
        exit 1
    else
        print_ok "[ PASSED ] ${source}"
        exit 0
    fi
}

function analyse_sources() {
    local path="${1}"

    for source in $path/*.cpp; do
        analyse_source $source &
    done
}

function analyse_project_test() {
    local project="${1}"
    local path="projects/${project}/test"

    if [[ -d "${path}" ]]; then
        analyse_sources $path
    fi
}

function analyse_project() {
    local project="${1}"
    local source="projects/${project}/${project}.cpp"

    if [[ "${project}" != "astl" ]]; then
        analyse_source $source "-header-filter=.*" &
    fi

    analyse_project_test $project
}

function analyse_projects() {
    for project in projects/*; do
        analyse_project ${project:9}
    done
}

function analyse() {
    if  [[ "${project}" == "" ]]; then
        analyse_projects
    else
        analyse_project $project
    fi

    for job in $(jobs -p)
    do
        wait $job || $((result += 1))
    done

    if (( $result != 0 )); then
        print_error "ERROR: analysis found issues (see above)"
    else
        print_ok "Analysis OK"
    fi

    exit $result
}

detect_clang_tidy
analyse
