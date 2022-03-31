source sh/common.sh

pids=()

function analyse() {
    local parameter="${1}"

    if [[ "${parameter}" == "" ]] || [[ "${parameter}" == "diff" ]]; then
        analyse_projects "${parameter}"
    else
        analyse_project "${parameter}"
    fi
}

function analyse_source() {
    if [[ "${1}" == projects/yamlcpp/* ]]; then
        return;
    fi

    wait_for_free_job 8 # $(nproc)
    do_analyse_source $1 $2 &
    pids+=($!)
}

function analyse_sources() {
    local path="${1}"
    local diff="${2}"

    for source in $path/*.cpp; do
        if should_analyse "${diff}" "${source}"; then
            analyse_source "${source}"
        fi
    done
}

function analyse_project() {
    local project="${1}"
    local diff="${2}"

    if [[ "${project}" != "astl" ]]; then
        analyse_project_module "${project}" "${diff}"
        analyse_project_main "${project}" "${diff}"
        analyse_project_test "${project}" "${diff}"
    fi
}

function analyse_project_module() {
    local project="${1}"
    local diff="${2}"

    if [[ "${project}" == */* ]]; then
        local ar=(${project//\// })
        local fileName=${ar[1]}
    else
        local fileName=${project}
    fi

    local source="projects/${project}/${fileName}.cpp"

    if [[ -f "${source}" ]]; then
        analyse_project_module_source "${source}" "${project}" "${diff}"
    fi
}

function analyse_project_module_source() {
    local source="${1}"
    local project="${2}"
    local diff="${3}"
    local sources=(projects/${project}/*.cpp)
    sources="${sources[@]}"

    if should_analyse "${diff}" "${sources}"; then
        analyse_source "${source}" "-header-filter=.*\\.cpp"
    fi
}

function analyse_project_main() {
    local project="${1}"
    local diff="${2}"
    local source="projects/${project}/main.cpp"

    if [[ -f "${source}" ]] && should_analyse "${diff}" "${source}"; then
        analyse_source "${source}"
    fi
}

function analyse_project_test() {
    local project="${1}"
    local diff="${diff}"
    local path="projects/${project}/test"

    if [[ -d "${path}" ]]; then
        analyse_sources "${path}" "${diff}"
    fi
}

function analyse_projects() {
    local diff="${1}"

    for project in projects/*; do
        if [[ -d "${project}" ]]; then
            analyse_project "${project:9}" "${diff}"
            analyse_subprojects "${project}" "${diff}"
        fi
    done
}

function analyse_subprojects() {
    local project="${1}"
    local diff="${2}"
    local size=${#project}+1
    
    for subproject in "${project}"/*; do
        if [[ -d "${subproject}" ]] && ! [[ "${subproject}" == */test ]]; then
            analyse_project "${project:9}/${subproject:${size}}" "${diff}"
        fi
    done
}

function detect_clang_tidy() {
    if ! is_available "${clangTidy}"; then
        print_error "ERROR: ${clangTidy} is not available. Please install it with './adev.sh install llvm'."
    else
        if is_linux; then
            libCppModuleMap="--extra-arg=-fmodule-map-file=/usr/lib/llvm-${llvmVersion}/include/c++/v1/module.modulemap"
        fi

        "${clangTidy}" --version | head -n 2 | tail -n +2
    fi
}

function do_analyse_source() {
    local source="${1}"
    local headerFilter="${2}"
    local log
    log=$(${clangTidy} --quiet ${libCppModuleMap} ${headerFilter} ${source} 2>&1)

    if (( $? != 0 )); then
        echo "${log}"
        print_error "[ FAILED ] ${source}"
        exit 1
    else
        print_ok "[ PASSED ] ${source}"
        exit 0
    fi
}

function should_analyse() {
    local diff="${1}"
    local sources="${2}"

    ( ! [[ "${diff}" == "diff" ]] || is_changed "${sources}" )
}

function wait_for_free_job() {
    while test $(jobs -p | wc -w) -ge "$1"; do wait -n; done
}

function wait_for_jobs() {
    result=0

    for job in ${pids[@]}
    do
        wait $job || result=1
    done
}

detect_clang_tidy
analyse "${1}"
wait_for_jobs

if (( $result == 0 )); then
    print_ok "Analysis OK"
else
    print_error "ERROR: analysis found issues (see above)"
fi

exit $result
