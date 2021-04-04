./build.sh "-fprofile-instr-generate -fcoverage-mapping"


function printError () {
    echo -e "\033[31m$1\033[0m"
}

function printOK () {
    echo -e "\033[32m$1\033[0m"
}

function coverage () {
    LLVM_COV=llvm-cov-11
    LLVM_PROFDATA=llvm-profdata-11

    #Collect all binary objects (libraries and executables)
    cd build/bin
    local OBJECTS=$(ls)
    local OBJECTS_ARR=(${OBJECTS})
    local OBJECTS_ARGS=${OBJECTS_ARR[0]}

    for object in ${OBJECTS_ARR[@]:1}
    do
        local OBJECTS_ARGS="$OBJECTS_ARGS -object=$object"
    done

    #Run the tests generating coverage data
    export LD_LIBRARY_PATH=.
    local TESTS=$(find . -name "*_test" -o -name "*_test.exe" -type f)
    for test in $TESTS
    do
        export LLVM_PROFILE_FILE="${test}.profraw"
        LOG=$(./${test})

        if test $? -ne 0; then
            echo ""
            echo ""
            printError "ERROR: $test failed:"
            echo "$LOG"
        fi
    done

    #Generate individual reports per test
    local DATA_RAW=$(find . -name "*.profraw" -type f)
    local DATA=""
    local EXCLUDE_REGEX=".*[Tt]est[\/\\]"

    for raw_data in $DATA_RAW
    do
        $LLVM_PROFDATA merge ${raw_data} -o ${raw_data}.profdata 
        $LLVM_COV show $OBJECTS_ARGS -output-dir=../coverage/${raw_data::-8} -format=html -instr-profile=${raw_data}.profdata
        DATA="$DATA ${raw_data}.profdata"
    done

    #Generate complete report
    $LLVM_PROFDATA merge $DATA -o coverage.profdata 
    $LLVM_COV show $OBJECTS_ARGS -output-dir=../coverage -format=html -instr-profile=coverage.profdata
    local TOTAL=$($LLVM_COV report $OBJECTS_ARGS -instr-profile=coverage.profdata | grep "TOTAL.*")
    local MATCH=$(echo $TOTAL | perl -nle'print $& while m{[\d\.]+\%}g')
    local MATCH_ARR=(${MATCH})
    local REGION=${MATCH_ARR[0]}
    local FUNCTION=${MATCH_ARR[1]}
    local LINE=${MATCH_ARR[2]}

    #cleanup
    rm -rf *.profraw
    rm -rf *.profdata

    printOK ""
    printOK "Summary:"
    printOK "  * Function: $FUNCTION"
    printOK "  * Line: $LINE"
    printOK "  * Region: $REGION"
    printOK "NOTE: See build/coverage for details."

    cd ../..
}

coverage
