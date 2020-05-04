#####################
# ADev Build Script #
#####################
ACTION=$1
INSTALL=$2

function printHelp () {
    echo "ADev Build Script"
    echo "-----------------"
    echo "Supported OS: Windows, Linux, macOS"
    echo "Usage:"
    echo "  ./build.sh [ACTION] [install]"
    echo ""
    echo "NOTE: The 'install' argument is optional and will enable installing of the prerequisites for the given action if they are not available."
    echo ""
    echo "Environment Variables:"
    echo "  CC: C compiler to use. [Default: 'gcc' on Linux, 'cl' on Windows, 'clang' on macOS, 'clang' if building 'coverage' everywhere]"
    echo "  CXX: C++ compiler to use. [Default: 'g++' on Linux, 'cl' on Windows, 'clang++' on macOS, 'clang++' if building 'coverage' everywhere]"
    echo "  BUILD_DIR: Relative directory to build in. [Default: build_\${CC}_\${BUILD_Type}] (e.g. build_gcc_Release)"
    echo "  BUILD_TYPE: Type of build passed to CMake. [Default: Release]"
    echo "  CLANG_FORMAT: Binary used for formatting. [Default: clang-format]"
    echo "  CLANG_TIDY: Binary used for static analysis. [Default: clang-tidy]"
    echo "  DOXYGEN: Binary used for building documentation. [Default: doxygen]"
    echo "  LLVM_COV: Binary used for generating code coverage. [Default: llvm-cov]"
    echo "  LLVM_PROFDATA: Binary used for generating code coverage. [Default: llvm-profdata]"
    echo "  MSVC_ENV_SCRIPT: [ONLY WINDOWS] Path to Visual Studio Environment Script (e.g. vcvars64.bat). [Default: C:/Program Files (x86)/Microsoft Visual Studio/2019/[Enterprise|Professional|Community]/VC/Auxiliary/Build/vcvars64.bat]"
    echo "  TEST_REPEAT: Runs each test this many times. [Default: 1]"
    echo ""
    echo "Available Actions:"
    echo "  build:"
    echo "    * Requires: CMake, Ninja, C++ Toolchain"
    echo "    * Environment Variables: CC, CXX, BUILD_DIR, BUILD_TYPE"
    echo "    * Builds with the given compilers and configuration (or with defaults if no options are provided)"
    echo "  check-files:"
    echo "    * Requires: git"
    echo "    * Environment Variables: None"
    echo "    * Determines if the files committed in the local branch that are different to the files in origin/master can be checked (i.e. if they are *.cpp, *.hpp). Returns 0 if there are such files."
    echo "  check-formatting"
    echo "    * Requires: clang-format"
    echo "    * Environment Variables: CLANG_FORMAT"
    echo "    * Checks formatting of files in commits that are not also in 'master'."
    echo "   analyse"
    echo "    * Requires: CMake, Ninja, C++ Toolchain, clang-tidy"
    echo "    * Environment Variables: CC, CXX, BUILD_DIR, BUILD_TYPE, CLANG_TIDY"
    echo "    * Builds and then performs clang-tidy static analysis on all source files using the compile commands database."
    echo "  coverage:"
    echo "    * Requires: llvm-cov, llvm-profdata, Clang"
    echo "    * Environment Variables: CC, CXX, BUILD_DIR, LLVM_COV, LLVM_PROFDATA"
    echo "    * Builds with code coverage settings, runs the tests and generate the code coverage report."
    echo "  documentation:"
    echo "    * Requires: Doxygen"
    echo "    * Environment Variables: DOXYGEN"
    echo "    * Builds documentation from the sources."
    echo "  test:"
    echo "    * Requires: None"
    echo "    * Environment Variables: BUILD_DIR, TEST_REPEAT"
    echo "    * Run tests in \$BUILD_DIR/bin/test. If \$BUILD_DIR is not specified first found build_* directory is used. If \$REPEAT is specified each test will be run that many times/"
}

function printError () {
    echo -e "\033[31m$1\033[0m"
}

function printOK () {
    echo -e "\033[32m$1\033[0m"
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

function installClangFormat () {
    if [ "$INSTALL" = "install" ]; then
        if isWindows; then
            choco install llvm
            export PATH="C:/Program Files/LLVM/bin:$PATH"
        elif isLinux; then
            sudo apt-get install clang-format-10
        else
            brew install clang-format
        fi

        clang-format --version
    else
        printError "ERROR: 'clang-format' is not available"
        exit 1
    fi
}

function installClangTidy () {
    if [ "$INSTALL" = "install" ]; then
        if isWindows; then
            choco install llvm
            export PATH="C:/Program Files/LLVM/bin:$PATH"
        elif isLinux; then
            sudo apt-get install clang-tidy
        else
            brew install llvm
        fi

        clang-tidy --version
    else
        printError "ERROR: 'clang-tidy' is not available"
        exit 1
    fi
}

function installLLVM () {
    if [ "$INSTALL" = "install" ]; then
        if isWindows; then
            choco install llvm
            export PATH="C:/Program Files/LLVM/bin:$PATH"
        elif isLinux; then
            sudo apt-get install llvm
        fi

        $1 --version
    else
        printError "ERROR: '$1' is not available"
        exit 1
    fi
}

function installClang () {
    if [ "$INSTALL" = "install" ]; then
        if isWindows; then
            choco install llvm
            export PATH="C:/Program Files/LLVM/bin:$PATH"
        elif isLinux; then
            sudo apt-get install clang clang++
        fi

        $CC --version
        $CXX --version
    else
        printError "ERROR: 'clang' is not available"
        exit 1
    fi
}

function installDoxygen () {
    if [ "$INSTALL" = "install" ]; then
        if isWindows; then
            choco install doxygen.install
            export PATH="C:/Program Files/doxygen/bin:$PATH"
        elif isLinux; then
            sudo apt-get install doxygen
        else
            brew install doxygen
        fi

        doxygen --version
    else
        printError "ERROR: 'doxygen' is not available"
        exit 1
    fi
}

function installNinja () {
    if [ "$INSTALL" = "install" ]; then
        if isWindows; then
            NinjaOS=win
        elif isLinux; then
            NinjaOS=linux
        else
            NinjaOS=mac
        fi

        curl -s -S -f -L -o ninja.zip https://github.com/ninja-build/ninja/releases/download/v1.10.0/ninja-${NinjaOS}.zip
        unzip -q ninja.zip
        export PATH=$(pwd):$PATH

        ninja --version
    else
        printError "ERROR: 'ninja' is not available"
        exit 1
    fi
}

function analyse() {
    if ! test "$CLANG_TIDY"; then
        CLANG_TIDY="clang-tidy"
    fi

    if ! isAvailable "$CLANG_TIDY"; then
        installLLVM $CLANG_TIDY
    fi

    build
    cd $BUILD_DIR
    
    if ! test -f "compile_commands.json"; then
        printError "ERROR: 'compile_commands.json' not generated. Are you missing 'set(CMAKE_EXPORT_COMPILE_COMMANDS true)' in CMakeLists.txt?"
        exit 1
    fi

    while IFS= read -r line
    do
        local file=$(echo $line | perl -nle 'print "$1" while /"file": "(.+)"/g;')
        if test "$file"; then
            SOURCE_FILES="$file $SOURCE_FILES"
        fi
    done < "compile_commands.json"

    echo "Analysing..."

    for source in $SOURCE_FILES
    do
        LINT_RESULT=$($CLANG_TIDY "$source" -p "$(pwd)" 2>&1)

        if test $? -ne 0; then
            echo ""
            echo $LINT_RESULT
            echo ""
            printError "Run 'clang-tidy --fix \"$source\" -p \"\$(pwd)\"' (adjust the paths to your system) or resolve the issues manually and commit the result."
            echo ""
            CLANG_TIDY_ERROR=1
        else
            printOK "$source (OK)"
        fi
    done

    if test $CLANG_TIDY_ERROR; then
        printError "ERROR: Static analysis found issues. See the log above for details."
        exit 1
    else
        printOK "Analysis OK"
    fi

    cd ..
}

function checkFormatting () {
    if ! test "$CLANG_FORMAT"; then
        CLANG_FORMAT="clang-format"
    fi

    if ! isAvailable "$CLANG_FORMAT"; then
        installLLVM $CLANG_FORMAT
    fi

    local SOURCE_FILES=`find ./projects -name "*.cpp" -o -name "*.hpp" -type f`

    echo "Formatting..."

    for file in $SOURCE_FILES
    do
        local REPLACEMENTS=`$CLANG_FORMAT $file -output-replacements-xml | grep "<replacement "`

        if test "$REPLACEMENTS"; then
            printError "$file (ERROR: Incorrectly formatted file.)"
            UNFORMATTED_FILES="$UNFORMATTED_FILES $file"
        else
            printOK "$file (OK)"
        fi
    done
    
    if test "$UNFORMATTED_FILES"; then
        printError "ERROR: Incorrectly formatted files."
        printError "Run 'clang-format -i $UNFORMATTED_FILES' and commit the result."
        exit 1
    else
        printOK "Formatting OK"
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
            printError "ERROR: Visual Studio environemnt script not found."
            exit 1
        fi
    fi

    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
    local BUILD_SCRIPT="call \"${MSVC_ENV_SCRIPT}\"
                        set CC=${CC}
                        set CXX=${CXX}
                        cmake .. -G Ninja -D CMAKE_BUILD_TYPE=${BUILD_TYPE} -D CMAKE_INSTALL_PREFIX=.
                        ninja
                        ninja install"
    echo "$BUILD_SCRIPT" > build.bat
    cmd //c build.bat
}

function buildUnix () {
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
    export CC=$CC
    export CXX=$CXX
    cmake .. -G Ninja -D CMAKE_BUILD_TYPE=$BUILD_TYPE -D CMAKE_INSTALL_PREFIX=.
    ninja
    ninja install
}

function build () {
    if ! isAvailable "cmake"; then
        printError "ERROR: 'cmake' is not available."
        exit 1
    fi
    
    if ! isAvailable "ninja"; then
        installNinja
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
        printError "ERROR: '$CC' C compiler is not available."
        exit 1
    fi

    if ! isAvailable "$CXX" && ! isWindows; then
        printError "ERROR: '$CXX' C++ compiler is not available."
        exit 1
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

    if test $? -ne 0; then
        exit 1
    else
        cd ..
    fi
}

function buildCoverage () {
    BUILD_TYPE="Coverage"

    if ! test "$BUILD_DIR"; then
        BUILD_DIR="build_clang_Coverage"
    fi

    if ! test "$CC"; then
        if isWindows; then
            CC="clang-cl"
        else
            CC="clang"
        fi
    fi

    if ! test "$CXX"; then
        if isWindows; then
            CXX="clang-cl"
        else
            CXX="clang++"
        fi
    fi

    if ! isAvailable $CC || ! isAvailable $CXX; then
        installClang
    fi

    build
}

function coverage () {
    if ! test "$LLVM_COV"; then
        LLVM_COV="llvm-cov"
    fi

    if ! isAvailable "$LLVM_COV"; then
        installLLVM $LLVM_COV
    fi

    if ! test "$LLVM_PROFDATA"; then
        LLVM_PROFDATA="llvm-profdata"
    fi

    if ! isAvailable "$LLVM_PROFDATA"; then
        installLLVM $LLVM_PROFDATA
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
        printError "ERROR: Test code coverage is not sufficient:"
        printError "  * Region: $REGION (can by any)"
        printError "  * Function: $FUNCTION (must be 100.00 %)"
        printError "  * Line: $LINE (must be 100.00 %)"
        printError "NOTE: See code coverage report in build artifacts for details."
        exit 1
    else
        printOK "Test code coverage is OK:"
        printOK "  * Region: $REGION"
        printOK "  * Function: $FUNCTION"
        printOK "  * Line: $LINE"
        printOK "NOTE: See code coverage report in build artifacts for details."
    fi
}

function documentation () {
    if ! test "$DOXYGEN"; then
        DOXYGEN="doxygen"
    fi

    if ! isAvailable "$DOXYGEN"; then
        installDoxygen $DOXYGEN
    fi

    $DOXYGEN ADev.doxyfile

    if test $?; then
        printOK "Documentation OK"
    fi
}

function runTests () {
    if ! test "$BUILD_DIR"; then
        BUILD_DIR=$(find . -name "build_*" -type d | head -n 1)
    fi

    if ! test "$TEST_REPEAT"; then
        TEST_REPEAT=1
    fi
    
    cd $BUILD_DIR/bin/test
    local TESTS=`find . -name "*Test" -o -name "*Test.exe" -type f`
    export LD_LIBRARY_PATH=.
    local TEST_RUN_RESULT=0
    local TEST_OK=0
    local i=0
    local RETURN=0

    echo "Running tests $TEST_REPEAT times..."

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
elif test "$ACTION" == "analyse"; then
    analyse
elif test "$ACTION" == "documentation"; then
    documentation
elif test "$ACTION" == "test"; then
    runTests
else
    printHelp
fi
