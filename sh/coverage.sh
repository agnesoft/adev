[ -n "$COVERAGE_SH" ] && return || readonly COVERAGE_SH=1

source "sh/common.sh"

#function | line | region | branch
#e.g. thresholds["abuild.cache.test.exe"]="1 0 1 0"
declare -A thresholds

function coverage() {
    detect_llvm_cov
    detect_llvm_profdata
    set_coverage_properties

    result=0

    if [[ "${project}" != "" ]]; then
        coverage_project "${binDir}/${project}"
    else
        coverage_all
    fi

    cleanup

    exit $result
}

function set_coverage_properties() {
    if is_windows; then
        readonly ignoredSources="(\\\\|\\|\/)(test|test_utilities|yamlcpp)(\\\\|\\|\/)"

        thresholds["astl.test.exe"]="2 2 2 0"
        thresholds["abuild.cache.test.exe"]="3 4 4 0"
        thresholds["abuild.scanners.test.exe"]="0 0 0 1"
        thresholds["acommandline.test.exe"]="0 1 12 0"
        thresholds["aprocess.test.exe"]="1 9 5 5"
        thresholds["atest.test.exe"]="2 31 21 2"
        thresholds["awinapi.test.exe"]="0 3 1 1"
    else
        readonly ignoredSources="\/(test|test_utilities|yamlcpp)\/"
    fi
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
    for test in ${binDir}/*.test${executableExtension}; do
        coverage_project "${test}"
    done
}

function cleanup() {
    rm -f ${binDir}/*.profdata
    rm -f ${binDir}/*.profraw
}

function detect_llvm_cov() {
    if ! is_available "${llvmCov}"; then
        print_error "ERROR: 'llvm-cov' is not available. Try installing it with './adev.sh install llvm'"
        exit 1
    fi
}

function detect_llvm_profdata() {
    if ! is_available "${llvmProfdata}"; then
        print_error "ERROR: 'llvm-profdata' is not available. Try installing it with './adev.sh install llvm'"
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
  * region:   ${regionCoverage} uncovered (must be ${branchThreshold})
  * branch:   ${branchCoverage} uncovered (must be ${regionThreshold})"
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
