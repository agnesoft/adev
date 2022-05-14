[ -n "$COVERAGE_SH" ] && return || readonly COVERAGE_SH=1

source "sh/common.sh"
source "sh/coverage_thresholds.sh"

function coverage() {
    detect_llvm_cov
    detect_llvm_profdata

    readonly ignoredSources="(\\\\|\\|\/)(test|test_utilities|yamlcpp)(\\\\|\\|\/)"
    result=0

    if [[ "${project}" != "" ]]; then
        coverage_project "${binDir}/${project}"
    else
        coverage_all
    fi

    cleanup

    exit $result
}

function generate_project_report() {
    local -r test="${1}"

    "${llvmCov}" show -output-dir="${buildRoot}/reports/$(basename ${test})"  \
                      -format=html                                \
                      -instr-profile="${test}.profdata"           \
                      -ignore-filename-regex="${ignoredSources}"  \
                      -show-instantiations=false                  \
                      -show-branches=count                        \
                      -show-expansions                            \
                      -show-line-counts-or-regions                \
                      "${test}"                                   \
                      ${sources}
}

function set_data_points() {
    local -r total="${1}"

    local dataPoints=()

    for point in ${total}; do
        dataPoints+=($point)
    done

    regionCoverage=${dataPoints[2]}
    functionCoverage=${dataPoints[5]}
    lineCoverage=${dataPoints[8]}
    branchCoverage=${dataPoints[11]}
}

function set_thresholds() {
    local -r test="${1}"
    local -r testThresholds=(${thresholds["${test}"]})

    if [[ "${testThresholds}" == "" ]]; then
        functionThreshold="0"
        lineThreshold="0"
        branchThreshold="0"
        regionThreshold="0"
    else
        functionThreshold="${testThresholds[0]}"
        lineThreshold="${testThresholds[1]}"
        regionThreshold="${testThresholds[2]}"
        branchThreshold="${testThresholds[3]}"
    fi
}

function match_thresholds() {
    if [[ "${functionCoverage}" == "${functionThreshold}" ]] && [[ "${lineCoverage}" == "${lineThreshold}" ]] && [[ "${regionCoverage}" == "${regionThreshold}" ]] && [[ "${branchCoverage}" == "${branchThreshold}" ]]; then
        result=0
    else
        echo "HERE"
        result=1
    fi
}

function generate_project_summary() {
    local -r test="${1}"
    local -r basenameTest=$(basename "${test}")
    local -r total=$(${llvmCov} report ${test} -instr-profile=${test}.profdata -ignore-filename-regex="${ignoredSources}" ${sources} | grep "TOTAL.*")

    set_data_points "${total}"
    set_thresholds "${basenameTest}"
    match_thresholds
    print_project_summary "${test}"
}

function coverage_project() {
    local -r test="${1}"

    log=$(LLVM_PROFILE_FILE="${test}.profraw" ${test})

    if (( $? != 0 )); then
        echo "${log}"
        print_error "[ FAILED ] ${test}"
        exit 1
    else
        "${llvmProfdata}" merge "${test}.profraw" -o "${test}.profdata"
        set_sources "${test}"
        generate_project_report "${test}"
        generate_project_summary "${test}"
    fi
}

function coverage_all() {
    if ! [[ -d "${binDir}" ]]; then
        print_error "ERROR: 'clang | coverage' build missing. Please build it with './build.sh clang coverage'."
        exit 1
    fi

    for test in ${binDir}/*.test${executableExtension}; do
        if [[ "${test}" != *awinapi.test ]]; then
            coverage_project "${test}"
        fi
    done
}

function cleanup() {
    rm -f ${binDir}/*.profdata
    rm -f ${binDir}/*.profraw
}

function detect_llvm_cov() {
    if ! is_available "${llvmCov}"; then
        print_error "ERROR: 'llvm-cov' is not available. Try installing it with './build.sh install llvm'"
        exit 1
    fi
}

function detect_llvm_profdata() {
    if ! is_available "${llvmProfdata}"; then
        print_error "ERROR: 'llvm-profdata' is not available. Try installing it with './build.sh install llvm'"
        exit 1
    fi
}

function print_project_summary() {
    local -r test="${1}"

    if (( $result == 1 )); then
        print_error "[ FAILED ] ${test}
Insufficient coverage:
  * function: ${functionCoverage} uncovered (must be ${functionThreshold})
  * line:     ${lineCoverage} uncovered (must be ${lineThreshold})
  * region:   ${regionCoverage} uncovered (must be ${regionThreshold})
  * branch:   ${branchCoverage} uncovered (must be ${branchThreshold})"
    else
        print_ok "[ PASSED ] ${test}"
    fi
}

function set_sources() {
    local -r test="${1}"
    local -r baseName=$(basename ${test})
    sources=""

    if [[ "${baseName}" == *.exe ]]; then
        local -r projectName="${baseName:: -9}"
    else
        local -r projectName="${baseName:: -5}"
    fi

    sources="$(find projects/${projectName} -maxdepth 1 -name *.cpp) $(find projects/${projectName} -maxdepth 1 -name *.hpp)"
}
