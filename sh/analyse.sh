[ -n "$ANALYSE_SH" ] && return || readonly ANALYSE_SH=1

source sh/build_common.sh
source sh/analyse_checks.sh

function analyse() {
    detect_clang_tidy

    pids=()

    set_analyse_properties "${1}" "${2}"

    if [[ "${file}" != "" ]]; then
        analyse_source "${file}"
    else
        analyse_projects
    fi

    wait_for_jobs

    if (( $result == 0 )); then
        print_ok "Analysis OK"
    else
        print_error "ERROR: analysis found issues (see above)"
    fi

    exit $result
}

function analyse_project() {
    local -r project="${1}"

    analyse_project_module "${project}"
    analyse_project_main "${project}"
    analyse_project_test "${project}"
}

function analyse_project_module() {
    local -r project="${1}"

    if [[ -f "projects/${project}/${project}.cpp" ]]; then
        analyse_project_module_source "${project}"
    fi
}

function analyse_project_module_source() {
    local project="${1}"
    local sources=(projects/${project}/*.cpp)

    if should_analyse "${sources[@]}"; then
        analyse_source "projects/${project}/${project}.cpp" "-header-filter=.*\\.cpp"
    fi
}

function analyse_project_main() {
    local project="${1}"
    local source="projects/${project}/main.cpp"

    if [[ -f "${source}" ]] && should_analyse "${source}"; then
        analyse_source "${source}"
    fi
}

function analyse_project_test() {
    local project="${1}"
    local path="projects/${project}/test"

    if [[ -d "${path}" ]]; then
        analyse_sources "${path}"
    fi
}

function analyse_projects() {
    for project in projects/*; do
        analyse_project "${project:9}"
    done
}

function analyse_source() {
    if [[ "${1}" == projects/yamlcpp/* ]]; then
        return;
    fi

    wait_for_free_job 8 # $(nproc)
    do_analyse_source "${1}" "${2}" &
    pids+=($!)
}

function analyse_sources() {
    local path="${1}"

    for source in ${path}/*.cpp; do
        if should_analyse "${source}"; then
            analyse_source "${source}"
        fi
    done
}

function detect_clang_tidy() {
    if ! is_available "${clangTidy}"; then
        print_error "ERROR: ${clangTidy} is not available. Please install it with './build.sh install llvm'."
    else
        if is_linux; then
            libCppModuleMap="--extra-arg=-fmodule-map-file=/usr/lib/llvm-${llvmVersion}/include/c++/v1/module.modulemap"
            "${clangTidy}" --version | head -n 1
        else
            "${clangTidy}" --version | head -n 2 | tail -n +2
        fi
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

function set_analyse_properties() {
    if [[ "${1}" == "diff" ]]; then
        diff="diff"
        set_checks "${2}"
    elif [[ -f "${1}" ]]; then
        file="${1}"
        set_checks "${2}"
    else
        set_checks "${1}"
    fi

    if ! [[ -d "build/clang/release" ]]; then
        print_error "ERROR: 'clang | release' build missing. Please build it with './build.sh clang release'."
        exit 1
    fi
}

function set_checks() {
    local checkSets=("${1}")
    local checks=""

    if [[ "${checkSets[@]}" == "" ]]; then
        checkSets=("bugprone cert cppcoreguidelines google misc modernize performance readability")
    fi

    for checkSet in ${checkSets[@]}; do
        for check in ${allChecks[$checkSet]}; do
            # MSVC STL has throwing std::string / std::vector
            # default constructors (that should not throw) and
            # clang-tidy somewhat incorrectly leak that diagnostics
            # to user code via 'bugprone-exception-escape' so that
            # check cannot be used on Windows for now.
            #
            # 15. 5. 2022
            if ! is_windows || [[ "${check}" != "bugprone-exception-escape" ]]; then
                checks="${checks}
${check},"
            fi
        done
    done

    echo "Checks: '${checks}
'
WarningsAsErrors: '${checks}
'
FormatStyle: file
CheckOptions:
  - key:   hicpp-signed-bitwise.IgnorePositiveIntegerLiterals
    value: true
  - key:   readability-identifier-naming.ClassCase
    value: 'CamelCase'
  - key:   readability-identifier-naming.EnumCase
    value: 'CamelCase'
  - key:   readability-identifier-naming.EnumConstantCase
    value: 'CamelCase'
  - key:   readability-identifier-naming.FunctionCase
    value: 'lower_case'
  - key:   readability-identifier-naming.GlobalConstantCase
    value: 'UPPER_CASE'
  - key:   readability-identifier-naming.MacroDefinitionCase
    value: 'UPPER_CASE'
  - key:   readability-identifier-naming.NamespaceCase
    value: 'lower_case'
  - key:   readability-identifier-naming.StructCase
    value: 'CamelCase'
  #Broken since LLVM 13 (2nd October 2021) 
  #- key:   readability-identifier-naming.TemplateParameterCase
  #  value: 'CamelCase'
  - key:   readability-identifier-naming.ValueTemplateParameterCase
    value: 'camelBack'
  - key:   readability-identifier-naming.TypeAliasCase
    value: 'CamelCase'
  - key:   readability-identifier-naming.VariableCase
    value: 'camelBack'" > ./.clang-tidy
}

function should_analyse() {
    local -r sources="${1}"

    [[ "${diff}" == "" ]] || is_changed "${sources}"
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
