source "sh/common.sh"

function set_iterations() {
    if [[ "${1}" != "" ]] && is_number $1; then
        iterations=$1
    elif [[ "${2}" != "" ]] && is_number $2; then
        iterations=$2
    else
        iterations=1
    fi
}

function run_test() {
    local test="${1}"
    local failures=0
    local i=0

    for (( i=0; i < $iterations; i++ )); do
        local log
        log=$($test)

        if (( $? != 0 )); then
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

function run_tests() {
    local testDir="build/${toolchain}/bin"
    result=0
    echo "Running tests from '${testDir}'..."

    if [[ -d "${testDir}" ]]; then
        for test in $testDir/*_test$executableExtension; do
            if [[ -f "${test}" ]]; then
                run_test "${test}"
            fi
        done
    fi

    if (( $result != 0 )); then
        print_error "Tests FAILED"
        exit 1
    else
        print_ok "All tests PASSED"
        exit 0
    fi
}

set_toolchain "${1}"
set_iterations $1 $2
run_tests
