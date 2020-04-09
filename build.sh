#####################
# ADev Build Script #
#####################
#Parameters
ACTION=$1
BUILD_DIR=$2
CC=$3
CXX=$4
BUILD_TYPE=$5

#############
# Functions #
#############
function printHelp () {
    echo "ADev Build Script"
    echo "-----------------"
    echo "Supported OS: Windows, Linux, macOS"
    echo "Usage:"
    echo "  ./build.sh [OPTIONS]"
    echo "Options:"
    echo "  NOTE: All of the options are optional."
    echo "  #1: Action (see below) [Default: printHelp]"
    echo "  #2: Build dir [Default: build_{C compiler}_{Build Type}] (e.g. build_gcc_Release)"
    echo "  #3: C Compiler [Default: 'gcc' on Linux, 'cl' on Windows, 'clang' on macOS, 'clang' if building 'coverage' everywhere]"
    echo "  #4: C++ Compiler [Default: 'g++' on Linux, 'cl' on Windows, 'clang++' on macOS, 'clang++' if building 'coverage' everywhere]"
    echo "  #5: Build type [Default: Release]"
    echo ""
    echo "Available Actions:"
    echo "  build:"
    echo "    * Requires: CMake, Ninja, C++ Toolchain"
    echo "    * Options: #2, #3, #4, #5 (see above)"
    echo "    * Builds with the given compilers and configuration (or with defaults if no options are provided)"
    echo "  build-coverage:"
    echo "    * Requires: CMake, Ninja, Clang"
    echo "    * Options: #2"
    echo "    * Builds instrumented binaries with Clang with enabled code coverage."
    echo "  check-formatting"
    echo "    * Requires: clang-format"
    echo "    * Options: None"
    echo "    * Checks formatting of files in commits that are not also in 'master'."
    echo "  coverage:"
    echo "    * Requires: llvm-cov, llvm-profdata"
    echo "    * Options: #2"
    echo "    * Runs the tests from code coverage enabled build and generate the code coverage report."
    echo "  files-to-check:"
    echo "    * Requires: git"
    echo "    * Options: None"
    echo "    * Determines if the files committed in the local branch that are different to the files in origin/master can be checked (i.e. *.cpp, *.hpp). Returns 0 if there are such files."
    echo "  test:"
    echo "    * Requires: None"
    echo "    * Options: #2"
    echo "    * Run previously built unit tests from #2/bin/test."
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

function filesToCheck () {
    local COMMITTED_FILES=`git diff --name-only HEAD origin/master | grep 'cpp\|hpp' || [[ $? == 1 ]]`

    if test "$COMMITTED_FILES"; then
        exit 0
    fi

    exit 1
}

function checkFormatting () {
    if ! isAvailable "clang-format"; then
        echo "ERROR: 'clang-format' is not available"
        exit 1
    fi

    local COMMITTED_FILES=`git diff --name-only HEAD origin/master | grep 'cpp\|hpp' || [[ $? == 1 ]]`

    if test "$COMMITTED_FILES"; then
        clang-format -i $COMMITTED_FILES
        local UNFORMATTED_FILES=`git ls-files -m | grep 'cpp/|hpp'` || [[ $? == 1 ]]

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
    if test -f "C:/Program Files (x86)/Microsoft Visual Studio/2019/Enterprise/VC/Auxiliary/Build/vcvars64.bat"; then
        ENV_SCRIPT="C:/Program Files (x86)/Microsoft Visual Studio/2019/Enterprise/VC/Auxiliary/Build/vcvars64.bat"
    elif test -f "C:/Program Files (x86)/Microsoft Visual Studio/2019/Professional/VC/Auxiliary/Build/vcvars64.bat"; then
        ENV_SCRIPT="C:/Program Files (x86)/Microsoft Visual Studio/2019/Professional/VC/Auxiliary/Build/vcvars64.bat"
    elif test -f "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build/vcvars64.bat" ; then
        ENV_SCRIPT="C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build/vcvars64.bat"
    else
        echo "ERROR: Visual Studio 2019 environemnt script not found"
        exit 1
    fi

    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
    local BUILD_SCRIPT="call \"${ENV_SCRIPT}\"
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
    CC="clang"
    CXX="clang++"
    BUILD_TYPE="Coverage"
    build
}

function coverage () {
    if ! isAvailable "llvm-cov"; then   
        echo "ERROR: 'llvm-cov' is not available"
        exit 1
    fi

    if ! isAvailable "llvm-profdata"; then
        echo "ERROR: 'llvm-profdata' is not available"
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
        llvm-profdata merge ${raw_data} -o ${raw_data}.profdata 
        llvm-cov show $OBJECTS_ARGS -output-dir=../../coverage/${raw_data::-8} -ignore-filename-regex=.*[Tt]est[\/\\].* -format=html -instr-profile=${raw_data}.profdata
        local DATA="$DATA ${raw_data}.profdata"
    done

    #Generate complete report
    llvm-profdata merge $DATA -o coverage.profdata 
    llvm-cov show $OBJECTS_ARGS -output-dir=../../coverage -ignore-filename-regex=.*[Tt]est[\/\\].* -format=html -instr-profile=coverage.profdata
    local FULL_COVERAGE=$(llvm-cov report $OBJECTS_ARGS -ignore-filename-regex=.*[Tt]est[\/\\].* -instr-profile=coverage.profdata | grep "TOTAL.*100\.00\%.*100\.00\%")
   
    #cleanup
    rm -rf *.profraw
    rm -rf *.profdata

    if ! test "$FULL_COVERAGE"; then
        echo "ERROR: Test coverage is not full. See coverage results for details."
        exit 1
    else
        echo "Test coverage is 100.00 %"
    fi
}

function runTests () {
    if ! test "$BUILD_DIR"; then
        BUILD_DIR=$(find . -name "build_*" -type d | head -n 1)
    fi
    echo $BUILD_DIR
    
    cd $BUILD_DIR/bin/test
    export LD_LIBRARY_PATH=.
    find . -name "*Test" -o -name "*Test.exe" -type f -exec {} ";"
}

###############
# Main Script #
###############
if ! test "$ACTION"; then
    printHelp
elif test "$ACTION" == "build"; then
    build
elif test $ACTION == "build-coverage"; then
    buildCoverage
elif test "$ACTION" == "check-formatting"; then
    checkFormatting
elif test "$ACTION" == "coverage"; then
    if ! test -e "build_clang_Coverage"; then
        buildCoverage
    fi

    if ! test "$BUILD_DIR"; then
        BUILD_DIR="build_clang_Coverage"
    fi

    coverage
elif test "$ACTION" == "files-to-check"; then
    filesToCheck
elif test "$ACTION" == "test"; then
    runTests
fi
