source "sh/common.sh"

if is_windows; then
    ignoredSources="(\\\\|\\|\/)test(\\\\|\\|\/)"
    uncoveredFunctions="4"
    uncoveredLines="43"
    uncoveredRegions="35"
    uncoveredBranches="9"
else
    ignoredSources="\/test\/"
    uncoveredFunctions="3"
    uncoveredLines="26"
    uncoveredRegions="28"
    uncoveredBranches="2"
fi

function coverage() {
    detect_llvm_cov
    detect_llvm_profdata

    result=0

    local objectArgs=""
    local profData=""
    local dir=$(pwd)

    for test in build/clang/bin/*_test${executableExtension}; do
        objectArgs="${objectArgs} -object=${test}"
        
        LLVM_PROFILE_FILE="${dir}/${test}.profraw" ${test}>/dev/null
        if (( $? != 0 )); then
            result=2
        fi
        "${llvmProfdata}" merge "${test}.profraw" -o "${test}.profdata"
        profData="${profData} ${test}.profdata"
    done

    generate_report "${objectArgs}" "${profData}"
    generate_summary "${objectArgs}"

    rm build/clang/bin/*.profdata
    rm build/clang/bin/*.profraw

    exit $result
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

function generate_report() {
    local objectArgs=$1
    local profData=$2

    "${llvmProfdata}" merge ${profData} -o build/clang/bin/coverage.profdata
    "${llvmCov}" show ${objectArgs} -output-dir=build/coverage -format=html -instr-profile=build/clang/bin/coverage.profdata -ignore-filename-regex="${ignoredSources}" -show-instantiations=false -show-branches=count -show-expansions -show-line-counts-or-regions
}

function generate_summary() {
    local objectArgs=$1
    local total=$($llvmCov report $objectArgs -instr-profile=build/clang/bin/coverage.profdata -ignore-filename-regex="${ignoredSources}" | grep "TOTAL.*")
    local dataPoints=()
    for point in ${total}; do
        dataPoints+=($point)
    done

    local regionDiff=${dataPoints[2]}
    local functionDiff=${dataPoints[5]}
    local lineDiff=${dataPoints[8]}
    local branchDiff=${dataPoints[11]}

    if [[ "${functionDiff}" != "${uncoveredFunctions}" ]] || [[ "${lineDiff}" != "${uncoveredLines}" ]] || [[ "${regionDiff}" != "${uncoveredRegions}" ]] || [[ "${branchDiff}" != "${uncoveredBranches}" ]]; then
        result=1
    fi

    print_summary "${functionDiff}" "${lineDiff}" "${regionDiff}" "${branchDiff}"
}

function print_summary() {
    local function="${1}"
    local line="${2}"
    local region="${3}"
    local branch="${4}"

    if (( $result == 1 )); then
        print_error "ERROR: insufficient code coverage:"
    elif (( $result == 2 )); then
        print_error "ERROR: tests failed:"
    else
        print_ok "Code coverage OK"
    fi

    if [[ "${function}" != "${uncoveredFunctions}" ]]; then
        print_error "  * function: ${functionDiff} uncovered (${uncoveredFunctions} expected)"
    else
        print_ok "  * function: ${functionDiff} uncovered"
    fi

    if [[ "${line}" != "${uncoveredLines}" ]]; then
        print_error "  * line: ${line} uncovered (${uncoveredLines} expected)"
    else
        print_ok "  * line: ${line} uncovered"
    fi

    if [[ "${region}" != "${uncoveredRegions}" ]]; then
        print_error "  * region: ${region} uncovered (${uncoveredRegions} expected)"
    else
        print_ok "  * region: ${region} uncovered"
    fi

    if [[ "${branch}" != "${uncoveredBranches}" ]]; then
        print_error "  * branch: ${branch} uncovered (${uncoveredBranches} expected)"
    else
        print_ok "  * branch: ${branch} uncovered"
    fi
}

coverage
