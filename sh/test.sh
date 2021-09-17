source "sh/common.sh"

toolchain=
times=
extension=
result=0

function set_properties() {
    if [[ "$1" == "clang" ]]; then
        toolchain="clang"
        times=$2
    elif [[ "$1" == "gcc" ]]; then
        toolchain="gcc"
        times=$2
    elif [[ "$1" == "msvc" ]]; then
        toolchain="msvc"
        times=$2
    elif [[ "$1" == "" ]]; then
        if is_windows; then
            toolchain="msvc"
        elif is_linux; then
            toolchain="gcc"
        else
            toolchain="clang"
        fi

        times=1
    fi

    if [[ "$times" == "" ]]; then
        times=1
    fi

    if is_windows; then
        extension=".exe"
    fi
}

function run_test() {
    local test=$1
    local failures=0
    local i=0

    for (( i=0; i < $times; i++ )); do
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
        print_error "[ FAILED ] ${test} (${failures} of ${times} runs failed)"
    else
        print_ok "[ PASSED ] ${test} (${times} of ${times} runs passed)"
    fi

    result=$(( $result + $failures ))
}

function run_tests() {
    local testDir="build/$toolchain/bin"
    echo "Running tests from '${testDir}'..."

    if [[ -d "$testDir" ]]; then
        for test in $testDir/*_test$extension; do
            run_test $test
        done
    fi

    if (( $result != 0 )); then
        print_error "Tests FAILED"
    else
        print_ok "All tests PASSED"
    fi
}

set_properties $1 $2
run_tests
exit $result
