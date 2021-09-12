source ./sh/common.sh

TOOLCHAIN=$1

function setProperties () {
    if test "$TOOLCHAIN" == ""; then
        if isWindows; then
            TOOLCHAIN="msvc"
        else
            TOOLCHAIN="gcc"
        fi
    fi

    if ! isWindows && test "$TOOLCHAIN" == "msvc"; then
        printError "ERROR: toolchain 'msvc' is only available on Windows"
        exit 1
    fi

    if ! isLinux && test "$TOOLCHAIN" == "gcc"; then
        printError "ERROR: toolchain 'gcc' is only available on Linux"
        exit 1
    fi
}

function run_tests () {
    echo "Running tests $1 times..."

    local TESTS=$(find build/$TOOLCHAIN/bin -name "*_test" -o -name "*_test.exe" -type f)
    local TEST_REPEAT=$1
    local TEST_RUN_RESULT=0
    local TEST_OK=0
    local i=0
    local RETURN=0

    for test in $TESTS;
    do
        TEST_OK=0
        i=0

        while test "$i" -lt "$TEST_REPEAT";
        do
            i=$(($i + 1))
            LOG=$($test)
            RETURN=$?

            if test $RETURN -ne 0; then
                echo ""
                echo ""
                printError "ERROR: $test failed on run $i:"
                echo "$LOG"
                TEST_RUN_RESULT=1
                TEST_OK=$(($TEST_OK + 1))
                continue;
            fi
        done

        if test $TEST_OK -ne 0; then
            printError "ERROR: $test failed $TEST_OK times out of $TEST_REPEAT (see above for errors)"
        else
            printOK "$test OK"
        fi
    done

    exit $TEST_RUN_RESULT
}

setProperties
run_tests 100
