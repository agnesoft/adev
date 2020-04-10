#####################
# ADev Build Script #
#####################
#Parameters
ACTION=$1

#############
# Functions #
#############
function printHelp () {
    echo "ADev Build Script"
    echo "-----------------"
    echo "Supported OS: Windows, Linux, macOS"
    echo "Usage:"
    echo "  ./build.sh [ACTION]"
    echo "Environment Variables:"
    echo "  CC: C compiler to use. [Default: 'gcc' on Linux, 'cl' on Windows, 'clang' on macOS, 'clang' if building 'coverage' everywhere]"
    echo "  CXX: C++ compiler to use. [Default: 'g++' on Linux, 'cl' on Windows, 'clang++' on macOS, 'clang++' if building 'coverage' everywhere]"
    echo "  BUILD_DIR: Relative directory to build in [Default: build_\${CC}_\${BUILD_Type}] (e.g. build_gcc_Release)"
    echo "  BUILD_TYPE: Type of build passed to CMake. [Default: Release]"
    echo "  CLANG_FORMAT: Binary used for formatting [Default: clang-format]"
    echo "  LLVM_COV: Binary used for generating code coverage [Default: llvm-cov]"
    echo "  LLVM_PROFDATA: Binary used for generating code coverage [Default: llvm-profdata]"
    echo "  MSVC_ENV_SCRIPT: [ONLY WINDOWS] Path to Visual Studio Environment Script (vcvarsall.bat) [Default: C:/Program Files (x86)/Microsoft Visual Studio/2019/[Enterprise|Professional|Community]/VC/Auxiliary/Build/vcvars64.bat]"
    echo ""
    echo "Available Actions:"
    echo "  build:"
    echo "    * Requires: CMake, Ninja, C++ Toolchain"
    echo "    * Environment Variables: CC, CXX, BUILD_DIR, BUILD_TYPE"
    echo "    * Builds with the given compilers and configuration (or with defaults if no options are provided)"
    echo "  check-files:"
    echo "    * Requires: git"
    echo "    * Options: None"
    echo "    * Determines if the files committed in the local branch that are different to the files in origin/master can be checked (i.e. if they are *.cpp, *.hpp). Returns 0 if there are such files."
    echo "  check-formatting"
    echo "    * Requires: clang-format"
    echo "    * Environment Variables: CLANG_FORMAT"
    echo "    * Checks formatting of files in commits that are not also in 'master'."
    echo "  coverage:"
    echo "    * Requires: llvm-cov, llvm-profdata, Clang"
    echo "    * Options: CC, CXX, BUILD_DIR, LLVM_COV, LLVM_PROFDATA"
    echo "    * Runs the tests from the code coverage enabled build and generate the code coverage report. Builds it first if \${BUILD_DIR} does not exist"
    echo "  test:"
    echo "    * Requires: None"
    echo "    * Environment Variables: BUILD_DIR"
    echo "    * Run previously built unit tests from \$BUILD_DIR/bin/test. If \$BUILD_DIR is not specified first found build_* directory is used."
}

function isWindows () { 
    test $OSTYPE == "msys" || test $OSTYPE == "cygwin" 
}

function isLinux () { 
    test $OSTYPE == "linux-gnu" 
}

function isAvailable () {
    local CMD=`command -v $1`
    test "$CMD"
}

function checkFiles () {
    local COMMITTED_FILES=`git diff --name-only HEAD origin/master | grep 'cpp\|hpp' || [[ $? == 1 ]]`

    if test "$COMMITTED_FILES"; then
        echo "Files to check: $COMMITTED_FILES"
        exit 0
    fi

    echo "Nothing to check."
    exit 1
}

function checkFormatting () {
    if ! test "$CLANG_FORMAT"; then
        CLANG_FORMAT="clang-format"
    fi

    if ! isAvailable "$CLANG_FORMAT"; then
        echo "ERROR: '$CLANG_FORMAT' is not available"
        exit 1
    fi

    local COMMITTED_FILES=`git diff --name-only HEAD origin/master | grep 'cpp\|hpp' || [[ $? == 1 ]]`

    if test "$COMMITTED_FILES"; then
        $CLANG_FORMAT -i $COMMITTED_FILES
        local UNFORMATTED_FILES=`git ls-files -m | grep 'cpp\|hpp'` || [[ $? == 1 ]]
        echo $UNFORMATTED_FILES
        if test "$UNFORMATTED_FILES"; then
            echo "Incorrectly formatted files: $UNFORMATTED_FILES"
            echo "Run 'clang-format -i $UNFORMATTED_FILES' and commit the result"
            exit 1
        else
            echo "Formatting is OK"
        fi
    else
        echo "Nothing to check."
    fi 
}

function buildWindows () {
    if ! test -f "$MSVC_ENV_SCRIPT"; then
        if test -f "C:/Program Files (x86)/Microsoft Visual Studio/2019/Enterprise/VC/Auxiliary/Build/vcvars64.bat"; then
            MSVC_ENV_SCRIPT="C:/Program Files (x86)/Microsoft Visual Studio/2019/Enterprise/VC/Auxiliary/Build/vcvars64.bat"
        elif test -f "C:/Program Files (x86)/Microsoft Visual Studio/2019/Professional/VC/Auxiliary/Build/vcvars64.bat"; then
            MSVC_ENV_SCRIPT="C:/Program Files (x86)/Microsoft Visual Studio/2019/Professional/VC/Auxiliary/Build/vcvars64.bat"
        elif test -f "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build/vcvars64.bat" ; then
            MSVC_ENV_SCRIPT="C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build/vcvars64.bat"
        else
            echo "ERROR: Visual Studio environemnt script not found"
            exit 1
        fi
    fi

    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
    local BUILD_SCRIPT="call \"${MSVC_ENV_SCRIPT}\"
                  set CC=${CC}
                  set CXX=${CXX}
                  cmake .. -GNinja -D CMAKE_BUILD_TYPE=${BUILD_TYPE} -D CMAKE_INSTALL_PREFIX=.
                  ninja
                  ninja install"

    echo "$BUILD_SCRIPT" > build.bat
    cmd //c build.bat
    cd ..
}

function buildUnix () {
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
    export CC=$CC
    export CXX=$CXX
    cmake .. -GNinja -D CMAKE_BUILD_TYPE=$BUILD_TYPE -D CMAKE_INSTALL_PREFIX=.
    ninja
    ninja install
    cd ..
}

function build () {
    if ! isAvailable "cmake"; then
        echo "ERROR: 'cmake' is not available"
        exit 1
    fi
    
    if ! isAvailable "ninja"; then
        echo "ERROR: 'ninja' build system is not available"
        exit 1
    fi

    if ! test "$CC"; then
        if isWindows; then
            CC="cl"
        elif isLinux; then
            CC="gcc"
        else
            CC="clang"
        fi
    fi

    if ! test "$CXX"; then
        if isWindows; then
            CXX="cl"
        elif isLinux; then
            CXX="g++"
        else
            CXX="clang"
        fi
    fi

    if ! isAvailable "$CC" && ! isWindows; then
        echo "ERROR: '$CC' C compiler is not available"
    fi

    if ! isAvailable "$CXX" && ! isWindows; then
        echo "ERROR: '$CXX' C++ compiler is not available"
    fi

    if ! test "$BUILD_TYPE"; then
        BUILD_TYPE="Release"
    fi

    if ! test "$BUILD_DIR"; then
        BUILD_DIR="build_${CC}_${BUILD_TYPE}"
    fi

    if isWindows; then
        buildWindows
    else
        buildUnix
    fi
}

function buildCoverage () {
    BUILD_TYPE="Coverage"

    if ! test "$BUILD_DIR"; then
        BUILD_DIR="build_clang_Coverage"
    fi

    if ! test -e "$BUILD_DIR"; then
        if ! test "$CC"; then
            CC="clang"
        fi

        if ! test "$CXX"; then
            CXX="clang++"
        fi

        build
    fi
}

function coverage () {
    if ! test "$LLVM_COV"; then
        LLVM_COV="llvm-cov"
    fi

    if ! isAvailable "$LLVM_COV"; then   
        echo "ERROR: '$LLVM_COV' is not available"
        exit 1
    fi

    if ! test "$LLVM_PROFDATA"; then
        LLVM_PROFDATA="llvm-profdata"
    fi

    if ! isAvailable "$LLVM_PROFDATA"; then
        echo "ERROR: '$LLVM_PROFDATA' is not available"
        exit 1
    fi

    cd $BUILD_DIR/bin/test
    
    #Collect all binary objects (libraries and executables)
    local OBJECTS=$(ls)
    local OBJECTS_ARR=(${OBJECTS})
    local OBJECTS_ARGS=${OBJECTS_ARR[0]}
    
    for object in ${OBJECTS_ARR[@]:1}
    do
        local OBJECTS_ARGS="$OBJECTS_ARGS -object=$object"
    done

    #Run the tests generating coverage data
    export LD_LIBRARY_PATH=.
    local TESTS=$(find . -name "*Test" -o -name "*Test.exe" -type f)
    for test in $TESTS
    do
        LLVM_PROFILE_FILE="${test}.profraw" ./${test}
    done

    #Generate individual reports per test
    local DATA_RAW=$(find . -name "*.profraw" -type f)
    local DATA=""

    for raw_data in $DATA_RAW
    do
        $LLVM_PROFDATA merge ${raw_data} -o ${raw_data}.profdata 
        $LLVM_COV show $OBJECTS_ARGS -output-dir=../../coverage/${raw_data::-8} -ignore-filename-regex=.*[Tt]est[\/\\].* -format=html -instr-profile=${raw_data}.profdata
        local DATA="$DATA ${raw_data}.profdata"
    done

    #Generate complete report
    $LLVM_PROFDATA merge $DATA -o coverage.profdata 
    $LLVM_COV show $OBJECTS_ARGS -output-dir=../../coverage -ignore-filename-regex=.*[Tt]est[\/\\].* -format=html -instr-profile=coverage.profdata
    local TOTAL=$($LLVM_COV report $OBJECTS_ARGS -ignore-filename-regex=.*[Tt]est[\/\\].* -instr-profile=coverage.profdata | grep "TOTAL.*")
    local MATCH=$(echo $TOTAL | perl -nle'print $& while m{[\d\.]+\%}g')
    local MATCH_ARR=(${MATCH})
    local REGION=${MATCH_ARR[0]}
    local FUNCTION=${MATCH_ARR[1]}
    local LINE=${MATCH_ARR[2]}
   
    #cleanup
    rm -rf *.profraw
    rm -rf *.profdata

    if ! test "${FUNCTION}" == "100.00%" || ! test "${LINE}" == "100.00%"; then
        echo "ERROR: Test coverage is not sufficient:"
        echo "  * Region: $REGION (can by any)"
        echo "  * Function: $FUNCTION (must be 100.00 %)"
        echo "  * Line: $LINE (must be 100.00 %)"
        exit 1
    else
        echo "Test coverage is 100.00 %"
    fi
}

function runTests () {
    if ! test "$BUILD_DIR"; then
        BUILD_DIR=$(find . -name "build_*" -type d | head -n 1)
    fi
    
    cd $BUILD_DIR/bin/test
    export LD_LIBRARY_PATH=.
    find . -name "*Test" -o -name "*Test.exe" -type f -exec {} ";"
}

###############
# Main Script #
###############

if test "$ACTION" == "build"; then
    build
elif test "$ACTION" == "check-files"; then
    checkFiles
elif test "$ACTION" == "check-formatting"; then
    checkFormatting
elif test "$ACTION" == "coverage"; then
    buildCoverage
    coverage
elif test "$ACTION" == "test"; then
    runTests
else
    printHelp
fi
