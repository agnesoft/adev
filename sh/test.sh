[ -n "$TEST_SH" ] && return || readonly TEST_SH=1

source "sh/build_common.sh"
source "sh/coverage.sh"

function test_() {
    set_build_properties "${1}" "${2}" "${3}"
    set_iterations "${1}" "${2}" "${3}" "${4}"

    if [[ "${configuration}" == "coverage" ]]; then
        coverage
    else
        run_tests
    fi
}

function set_iterations() {
    if [[ "${1}" != "" ]] && is_number $1; then
        iterations=$1
    elif [[ "${2}" != "" ]] && is_number $2; then
        iterations=$2
    elif [[ "${3}" != "" ]] && is_number $3; then
        iterations=$3
    elif [[ "${4}" != "" ]] && is_number $4; then
        iterations=$4
    else
        iterations=1
    fi
}

function run_test() {
    local -r test="${1}"
    local failures=0
    local i=0

    for (( i=0; i < $iterations; i++ )); do
        local log
        

        if ! log=$($test); then
            result=1
            failures=$(( $failures + 1 ))
            echo "${log}"
            print_error "ERROR: ${test} failed on run $(( $i + 1 )):"
        fi
    done

    if (( $failures != 0 )); then
        print_error "[ FAILED ] ${test} (${failures} of ${iterations} runs failed)"
    else
        print_ok "[ PASSED ] ${test} (${iterations} of ${iterations} runs passed)"
    fi

    result=$(( $result + $failures ))
}

function run_all_tests() {
    result=0
    echo "Running tests from '${binDir}'..."

    if [[ -d "${binDir}" ]]; then
        for test in ${binDir}/*.test${executableExtension}; do
            if [[ -f "${test}" ]]; then
                run_test "${test}"
            fi
        done
    fi
}

function run_tests() {
    if [[ "${project}" != "" ]]; then
        run_test "${binDir}/${project}"
    else
        run_all_tests
    fi

    if (( $result != 0 )); then
        print_error "Tests FAILED"
        exit 1
    else
        print_ok "All tests PASSED"
    fi
}
