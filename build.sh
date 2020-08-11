#####################
# ADev Build Script #
#####################
ACTION=$1
GCC_VERSION=10
LLVM_VERSION=10

function printHelp () {
    echo "ADev Build Script"
    echo "-----------------"
    echo "Supported OS: Windows, Linux, macOS"
    echo "Usage:"
    echo "  ./build.sh [ACTION]"
    echo ""
    echo "Environment Variables:"
    echo "  CC: C compiler to use. [Default: 'gcc' on Linux, 'cl' on Windows, 'clang' on macOS, 'clang' if building 'coverage' everywhere]"
    echo "  CXX: C++ compiler to use. [Default: 'g++' on Linux, 'cl' on Windows, 'clang++' on macOS, 'clang++' if building 'coverage' everywhere]"
    echo "  BUILD_DIR: Relative directory to build in. [Default: build_\${CC}_\${BUILD_Type}] (e.g. build_gcc_Release)"
    echo "  BUILD_TYPE: Type of build passed to CMake. [Default: Release]"
    echo "  MSVC_ENV_SCRIPT: [ONLY WINDOWS] Path to Visual Studio Environment Script (e.g. vcvars64.bat). [Default: C:/Program Files (x86)/Microsoft Visual Studio/2019/[Enterprise|Professional|Community]/VC/Auxiliary/Build/vcvars64.bat]"
    echo "  TEST_REPEAT: Runs each test this many times. [Default: 1]"
    echo ""
    echo "Available Actions:"
    echo "  analysis"
    echo "    * Requires: CMake, Ninja, C++ Toolchain, clang-tidy"
    echo "    * Environment Variables: CC, CXX, BUILD_DIR, BUILD_TYPE, [MSVC_ENV_SCRIPT]"
    echo "    * Builds and then performs static analysis using Clang-Tidy on all source files using the compile commands database."
    echo "  benchmarks"
    echo "    * Requires: None"
    echo "    * Environment Variables: BUILD_DIR"
    echo "    * Runs benchmark executables (.*[Bb]enchmark.*) found under \$BUILD_DIR/bin expecting first cases to be always the fastest."
    echo "  build[-analysis|-coverage|-sanitize-address|-sanitize-memory|-sanitize-ub]"
    echo "    * Requires: CMake, Ninja, C++ Toolchain"
    echo "    * Environment Variables: CC, CXX, BUILD_DIR, BUILD_TYPE, [MSVC_ENV_SCRIPT]"
    echo "    * Builds using the either the default compiler and configuration (see above) or values from the environment variables (see above) if set or predefined values based on the requested build flavor."
    echo "  build-vuejs"
    echo "    * Require: node.js, vue"
    echo "    * Environment Variables: None"
    echo "    * Build Vue.js applications."
    echo "  coverage"
    echo "    * Requires: llvm-cov, llvm-profdata, Clang"
    echo "    * Environment Variables: CC, CXX, BUILD_DIR, [MSVC_ENV_SCRIPT]"
    echo "    * Builds with Clang compiler and BUILD_TYPE=Coverage, runs the tests and generate the code coverage report."
    echo "  documentation"
    echo "    * Requires: Doxygen"
    echo "    * Environment Variables: None"
    echo "    * Builds documentation from the source files using Doxygen."
    echo "  formatting"
    echo "    * Requires: clang-format"
    echo "    * Environment Variables: None"
    echo "    * Checks formatting of the source files with Clang-Format."
    echo "  install-[clang|clang-format|clang-tidy|cmake|doxygen|llvm|msvc|ninja|vuejs]"
    echo "    * Requires: Chocolatey [Windows], apt-get [Linux], Homebrew [macOS]"
    echo "    * Environment Variables: None"
    echo "    * Installs one of the packages required by the other actions. Useful if you do not have them already. NOTE: 'msvc' can only be installed on Windows."
    echo "  sanitize-address"
    echo "    * Requires: Clang, llvm-symbolizer"
    echo "    * Environment Variables: CC, CXX, BUILD_DIR"
    echo "    * Builds with Clang/LLVM memory address sanitizer and run tests."
    echo "  sanitize-memory"
    echo "    * Requires: [Linux], Clang, llvm-symbolizer"
    echo "    * Environment Variables: CC, CXX, BUILD_DIR"
    echo "    * Builds with Clang/LLVM memory sanitizer and run tests."
    echo "  sanitize-ub"
    echo "    * Requires: Clang, llvm-symbolizer"
    echo "    * Environment Variables: CC, CXX, BUILD_DIR"
    echo "    * Builds with Clang/LLVM undefined behavior sanitizer and run tests."
    echo "  tests"
    echo "    * Requires: None"
    echo "    * Environment Variables: BUILD_DIR, TEST_REPEAT"
    echo "    * Run tests in \$BUILD_DIR/bin/test. If \$BUILD_DIR is not specified first found build_* directory is used. If \$REPEAT is specified each test will be run that many times/"
    echo "  tests-vuejjs"
    echo "    * Requires: node.js, Vue.js"
    echo "    * Environment Variables: None"
    echo "    * Run Vue.js tests."
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
    local CMD=$(command -v $1)
    test "$CMD"
}

#############
# INSTALL-* #
#############
function installClang () {
    if isWindows; then
        choco install -y llvm
    elif isLinux; then
        sudo apt-get update -y
        sudo apt-get purge -y clang clang++ clang-6 clang++-6 clang-7 clang++-7 clang-8 clang++-8 clang-9 clang++-9
        sudo apt-get autoremove -y
        sudo apt-get install -y clang-${LLVM_VERSION} clang++-${LLVM_VERSION}
    else
        brew install llvm
    fi
}

function installClangFormat () {
    if isWindows; then
        choco install -y llvm
    elif isLinux; then
        sudo apt-get update -y
        sudo apt-get install -y clang-format-${LLVM_VERSION}
    else
        brew install clang-format
    fi
}

function installClangTidy () {
    if isWindows; then
        choco install -y llvm
    elif isLinux; then
        sudo apt-get update -y
        sudo apt-get install -y clang-tidy-${LLVM_VERSION}
    else
        brew install llvm
    fi
}

function installCMake () {
    if isWindows; then
        choco install -y cmake
    elif isLinux; then
        sudo apt-get update -y
        sudo apt-get remove --purge cmake -y
        sudo apt-get install snap -y
        sudo snap install cmake --classic
    else
        brew install cmake
    fi
}

function installDoxygen () {
    if isWindows; then
        choco install doxygen.install -y
    elif isLinux; then
        sudo apt-get update -y
        sudo apt-get install -y doxygen
    else
        brew install doxygen
    fi
}

function installGCC () {
    if isLinux; then
        sudo apt-get update -y
        sudo apt-get install -y gcc-${GCC_VERSION} g++-${GCC_VERSION}
    else
        echo "ERROR: 'GCC' can only be installed on Linux."
    fi
}

function installLLVM () {
    if isWindows; then
        choco install -y llvm
    elif isLinux; then
        sudo apt-get update -y
        sudo apt-get install -y llvm-10
    else
        brew install llvm
    fi
}

function installMSVC () {
    if isWindows; then
        choco install -y visualstudio2019buildtools --package-parameters "--add Microsoft.VisualStudio.Workload.VCTools --passive --norestart"
    else
        printError "ERROR: 'MSVC' can only be installed on Windows."
        exit 1
    fi
}

function installNinja () {
    if isWindows; then
        choco install -y ninja
    elif isLinux; then
        sudo apt-get update -y
        sudo apt-get install -y ninja-build
    else
        brew install ninja
    fi
}

function installNodeJS() {
    if isWindows; then
        choco install -y nodejs
    elif isLinux; then
        sudo apt-get update -y
        sudo apt-get install -y nodejs
        sudo apt-get install -y npm
    else
        brew install node
    fi
}

function installVuejs {
    if ! isAvailable "npm"; then
        installNodeJS
    fi

    sudo npm install -g @vue/cli
    sudo npm install -g @vue/cli-service
    sudo npm install -g serve
}

##########
# DETECT #
##########
function detectBuildProperties () {
    if ! test "$BUILD_TYPE"; then
        BUILD_TYPE="Release"
    fi

    if ! test "$BUILD_DIR"; then
        BUILD_DIR="build_${CC}_${BUILD_TYPE}"
    fi
}

function detectClang () {
    if isWindows && isAvailable "clang-cl"; then
        CC="clang-cl"
        CXX="clang-cl"
    elif isAvailable "clang++-${LLVM_VERSION}"; then
        CC="clang-${LLVM_VERSION}"
        CXX="clang++-${LLVM_VERSION}"
    elif isAvailable "clang++"; then
        CC="clang"
        CXX="clang++"
    else
        printError "ERROR: 'clang' is not available. Try installing it with './build.sh install-clang'."
        exit 1
    fi

    $CXX --version | head -n 1
}

function detectClangFormat () {
    if isAvailable "clang-format-${LLVM_VERSION}"; then
        CLANG_FORMAT="clang-format-${LLVM_VERSION}"
    elif isAvailable "clang-format"; then
        CLANG_FORMAT="clang-format"
    else
        printError "ERROR: 'clang-format' is not available. Try installing it with './build.sh install-clang-format'."
        exit 1
    fi

    $CLANG_FORMAT --version | head -n 1
}

function detectClangTidy () {
    if isAvailable "clang-tidy-${LLVM_VERSION}"; then
        CLANG_TIDY="clang-tidy-${LLVM_VERSION}"
    elif isAvailable "clang-tidy"; then
        CLANG_TIDY="clang-tidy"
    else
        printError "ERROR: 'clang-tidy' is not available. Try installing it with './build.sh install-clang-tidy'."
        exit 1
    fi

    echo "clang-tidy from $($CLANG_TIDY --version | head -n 2 | tail -n 1)"
}

function detectCMake () {
    if isAvailable "cmake"; then
        CMAKE="cmake"
    else
        printError "ERROR: 'cmake' is not available. Try installing it with './build.sh install-cmake'."
    fi

    $CMAKE --version | head -n 1
}

function detectCompiler () {
    if ! test "$CXX"; then
        if isWindows; then
            CC=cl
            CXX=cl
        elif isLinux; then
            detectGCC
        else
            detectClang
        fi
    fi
}

function detectDoxygen () {
    if isAvailable "doxygen"; then
        DOXYGEN="doxygen"
    else
        printError "ERROR: 'doxygen' is not available. Try installing it with './build.sh install-doxygen'."
        exit 1
    fi

    echo "Doxygen $(doxygen --version)"
}

function detectGCC () {
    if isAvailable "g++-${GCC_VERSION}"; then
        CC="gcc-${GCC_VERSION}"
        CXX="g++-${GCC_VERSION}"
    elif isAvailable "g++"; then
        CC="gcc"
        CXX="g++"
    else
        printError "ERROR: 'GCC' is not available. Try installing it with './build.sh install-gcc'"
        exit 1
    fi

    $CXX --version | head -n 1
}

function detectLLVMCov () {
    if isAvailable "llvm-cov-${LLVM_VERSION}"; then
        LLVM_COV="llvm-cov-${LLVM_VERSION}"
    elif isAvailable "llvm-cov"; then
        LLVM_COV="llvm-cov"
    else
        printError "ERROR: 'llvm-cov' is not available. Try installing it with './build.sh install-llvm'."
        exit 1
    fi

    echo "llvm-cov from $($LLVM_COV --version | head -n 2 | tail -n 1)"
}

function detectLLVMProfdata () {
    if isAvailable "llvm-profdata-${LLVM_VERSION}"; then
        LLVM_PROFDATA="llvm-profdata-${LLVM_VERSION}"
    elif isAvailable "llvm-profdata"; then
        LLVM_PROFDATA="llvm-profdata"
    else
        printError "ERROR: 'llvm-profdata' is not available. Try installing it with './build.sh install-llvm'."
        exit 1
    fi
}

function detectLLVMSymbolizer () {
    if isAvailable "llvm-symbolizer-${LLVM_VERSION}"; then
        LLVM_SYMBOLIZER="llvm-symbolizer-${LLVM_VERSION}"
    elif isAvailable "llvm-symbolizer"; then
        LLVM_SYMBOLIZER="llvm-symbolizer"
    else
        printError "ERROR: 'llvm-symbolizer' is not available. Try insdtalling it with './build.sh install-llvm'."
        exit 1
    fi
}

function detectMSVCEnvScript () {
    if ! test "$MSVC_ENV_SCRIPT"; then
        MSVC_ENV_SCRIPT="C:/Program Files (x86)/Microsoft Visual Studio/2019/Enterprise/VC/Auxiliary/Build/vcvars64.bat"
        if test -f "$MSVC_ENV_SCRIPT"; then
            return
        fi
        
        MSVC_ENV_SCRIPT="C:/Program Files (x86)/Microsoft Visual Studio/2019/Professional/VC/Auxiliary/Build/vcvars64.bat"
        if test -f "$MSVC_ENV_SCRIPT"; then
            return
        fi

        MSVC_ENV_SCRIPT="C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build/vcvars64.bat"
        if test -f "$MSVC_ENV_SCRIPT"; then
            return
        fi

        printError "ERROR: Visual Studio environemnt script not found."
        exit 1
    fi
}

function detectNinja () {
    if isAvailable "ninja"; then
        NINJA="ninja"
    else
        printError "ERROR: 'ninja' is not available. Try installing it with './build.sh install-ninja'."
        exit 1
    fi

    echo "ninja $(ninja --version)"
}

function detectTestProperties () {
    if ! test "$BUILD_DIR"; then
        BUILD_DIR=$(find . -name "build_*" -type d | head -n 1)
    fi

    if ! test "$TEST_REPEAT"; then
        TEST_REPEAT=1
    fi
}

###########
# ACTIONS #
###########
function analysis() {
    detectClangTidy
    buildAnalysis
    cd $BUILD_DIR

    if test -f "clang_tidy_error"; then
        rm "clang_tidy_error"
    fi
    
    if ! test -f "compile_commands.json"; then
        printError "ERROR: 'compile_commands.json' cannot be found. Are you missing 'set(CMAKE_EXPORT_COMPILE_COMMANDS true)' in CMakeLists.txt?"
        exit 1
    fi

    while IFS= read -r line
    do
        local file=$(echo $line | perl -nle 'print "$1" while /"file": "(.+)"/g;')
        if test "$file"; then
            if [[ $file != *"cmake_pch"* ]] && [[ $file != *"tiny-process-library"* ]]; then
                SOURCE_FILES="$file $SOURCE_FILES"
            fi
        fi
    done < "compile_commands.json"

    echo "Analysing..."

    for source in $SOURCE_FILES
    do
        analyseFile "$source" &
    done
    wait

    if test -f "clang_tidy_error"; then
        printError "ERROR: Static analysis found issues. See the log above for details."
        exit 1
    else
        printOK "Analysis OK"
    fi
}

function analyseFile () {
    LINT_RESULT=$($CLANG_TIDY "$1" -p "$(pwd)" 2>&1)

    if test $? -ne 0; then
        echo ""
        echo "$LINT_RESULT"
        echo ""
        printError "Run 'clang-tidy --fix \"$1\" -p \"\$(pwd)\"' (adjust the paths to your system) or resolve the issues manually and commit the result."
        echo ""
        touch "clang_tidy_error"
    else
        printOK "$1 (OK)"
    fi
}

function benchmarks () {
    detectTestProperties

    cd $BUILD_DIR/bin
    local EXECUTABLES=$(find . -name "*benchmark*" -o -name "*Benchmark*" -o -name "*benchmark*.exe" -o -name "*Benchmark*" -type f)
    local RETURN=0
    local BENCHMARKS_RESULT=0

    for benchmark in $EXECUTABLES
    do
        $benchmark
        RETURN=$?

        if test $RETURN -ne 0; then
            echo ""
            echo ""
            printError "ERROR: $benchmark failed. See the log above for details."
            BENCHMARKS_RESULT=1
        else
            printOK "$benchmark OK"
        fi
    done

    exit $BENCHMARKS_RESULT
}

function build () {
    detectCMake
    detectNinja
    detectCompiler
    detectBuildProperties

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

function buildAnalysis () {
    detectClang
    build
}

function buildCoverage () {
    detectClang
    BUILD_TYPE="Coverage"
    build
}

function buildSanitizeAddress () {
    detectClang
    BUILD_TYPE="SanitizeAddress"
    build
}

function buildSanitizeUB () {
    detectClang
    BUILD_TYPE="SanitizeUB"
    build
}

function buildSanitizeMemory () {
    buildLibCppWithMemorySanitizer
    BUILD_TYPE="SanitizeMemory"
    build
}

function buildLibCppWithMemorySanitizer () {
    detectCMake
    detectNinja
    detectClang

    if [ ! -d "llvm" ]; then
        git clone --depth=1 -b llvmorg-10.0.1 https://github.com/llvm/llvm-project.git llvm
    fi
    
    cd llvm
    
    if [ ! -d "build_msan" ]; then 
        mkdir "build_msan"
    fi

    cd build_msan

    export CC=$CC
    export CXX=$CXX
    $CMAKE -G Ninja ../llvm -D CMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_PROJECTS="libcxx;libcxxabi" -DLLVM_USE_SANITIZER=MemoryWithOrigins
    $CMAKE --build . -- cxx cxxabi
    cd ../..
}

function buildUnix () {
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
    export CC=$CC
    export CXX=$CXX
    $CMAKE .. -G Ninja -D CMAKE_BUILD_TYPE=$BUILD_TYPE -D CMAKE_INSTALL_PREFIX=.
    $NINJA
    $NINJA install
}

function buildVuejs () {
    cd projects/ADbStudio
    npm install
    npm run build
}

function buildWindows () {
    detectMSVCEnvScript
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
    local BUILD_SCRIPT="call \"${MSVC_ENV_SCRIPT}\"
                        set CC=${CC}
                        set CXX=${CXX}
                        ${CMAKE} .. -G Ninja -D CMAKE_BUILD_TYPE=${BUILD_TYPE} -D CMAKE_INSTALL_PREFIX=.
                        ${NINJA}
                        ${NINJA} install"

    echo "$BUILD_SCRIPT" > build.bat
    cmd //c build.bat
}

function coverage () {
    detectLLVMCov
    detectLLVMProfdata
    buildCoverage

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
    local EXCLUDE_REGEX=".*[Tt]est[\/\\]|tiny-process-library.*"

    for raw_data in $DATA_RAW
    do
        $LLVM_PROFDATA merge ${raw_data} -o ${raw_data}.profdata 
        $LLVM_COV show $OBJECTS_ARGS -output-dir=../../coverage/${raw_data::-8} -format=html -ignore-filename-regex=$EXCLUDE_REGEX -instr-profile=${raw_data}.profdata
        DATA="$DATA ${raw_data}.profdata"
    done

    #Generate complete report
    $LLVM_PROFDATA merge $DATA -o coverage.profdata 
    $LLVM_COV show $OBJECTS_ARGS -output-dir=../../coverage -format=html -ignore-filename-regex=$EXCLUDE_REGEX -instr-profile=coverage.profdata
    local TOTAL=$($LLVM_COV report $OBJECTS_ARGS -ignore-filename-regex=$EXCLUDE_REGEX -instr-profile=coverage.profdata | grep "TOTAL.*")
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
        printError "  * Function: $FUNCTION (must be 100.00 %)"
        printError "  * Line: $LINE (must be 100.00 %)"
        printError "  * Region: $REGION (can by any)"
        printError "NOTE: See code coverage report in build artifacts for details."
        exit 1
    else
        printOK "Test code coverage is OK:"
        printOK "  * Function: $FUNCTION"
        printOK "  * Line: $LINE"
        printOK "  * Region: $REGION"
        printOK "NOTE: See code coverage report in build artifacts for details."
    fi
}

function documentation () {
    detectDoxygen
    echo "Generating documentation..."
    $DOXYGEN ADev.doxyfile
    if test $? -ne 0; then
        printError "ERROR: Building documentation failed."
        exit 1
    else
        printOK "Documentation OK"
    fi
}

function formatting () {
    detectClangFormat

    echo "Formatting..."

    local SOURCE_FILES=$(find ./projects -name "*.cpp" -o -name "*.hpp" -type f)
    for file in $SOURCE_FILES
    do
        if [[ $file != *"tiny-process-library"* ]] && [[ $file != *"Catch2"* ]]; then
            local REPLACEMENTS=$($CLANG_FORMAT $file -output-replacements-xml | grep "<replacement ")

            if test "$REPLACEMENTS"; then
                printError "$file (ERROR: Incorrectly formatted file.)"
                UNFORMATTED_FILES="$UNFORMATTED_FILES $file"
            else
                printOK "$file (OK)"
            fi
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

function sanitizeAddress () {
    if isWindows; then
        printError "ERROR: Address sanitizer is not supported on Windows."
        exit 1
    fi

    detectLLVMSymbolizer
    buildSanitizeAddress
    tests
}

function sanitizeMemory () {
    if ! isLinux; then
        printError "ERROR: Memory sanitizer is supported only on Linux."
        exit 1
    fi

    detectLLVMSymbolizer
    buildSanitizeMemory
    tests
}

function sanitizeUB () {
    if isWindows; then
        printError "ERROR: Undefined behavior sanitizer is not supported on Windows."
        exit 1
    fi

    detectLLVMSymbolizer
    buildSanitizeUB
    tests
}

function tests () {
    detectTestProperties

    cd $BUILD_DIR/bin/test
    local TESTS=$(find . -name "*Test" -o -name "*Test.exe" -type f)
    local TEST_RUN_RESULT=0
    local TEST_OK=0
    local i=0
    local RETURN=0

    echo "Running tests $TEST_REPEAT times..."

    export LD_LIBRARY_PATH=.

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

function testsVuejs () {
    cd projects/ADbStudio
    npm run test:unit
}

###############
# Main Script #
###############

if test "$ACTION" == "analysis"; then
    analysis
elif test "$ACTION" == "benchmarks"; then
    benchmarks
elif test "$ACTION" == "build"; then
    build
elif test "$ACTION" == "build-analysis"; then
    buildAnalysis
elif test "$ACTION" == "build-coverage"; then
    buildCoverage
elif test "$ACTION" == "build-sanitize-address"; then
    buildSanitizeAddress
elif test "$ACTION" == "build-sanitize-memory"; then
    buildSanitizeMemory
elif test "$ACTION" == "build-sanitize-ub"; then
    buildSanitizeUB
elif test "$ACTION" == "build-vuejs"; then
    buildVuejs
elif test "$ACTION" == "coverage"; then
    coverage
elif test "$ACTION" == "documentation"; then
    documentation
elif test "$ACTION" == "formatting"; then
    formatting
elif test "$ACTION" == "install-clang"; then
    installClang
elif test "$ACTION" == "install-clang-format"; then
    installClangFormat
elif test "$ACTION" == "install-clang-tidy"; then
    installClangTidy
elif test "$ACTION" == "install-cmake"; then
    installCMake
elif test "$ACTION" == "install-doxygen"; then
    installDoxygen
elif test "$ACTION" == "install-llvm"; then
    installLLVM
elif test "$ACTION" == "install-msvc"; then
    installMSVC
elif test "$ACTION" == "install-ninja"; then
    installNinja
elif test "$ACTION" == "install-vuejs"; then
    installVuejs
elif test "$ACTION" == "sanitize-address"; then
    sanitizeAddress
elif test "$ACTION" == "sanitize-memory"; then
    sanitizeMemory
elif test "$ACTION" == "sanitize-ub"; then
    sanitizeUB
elif test "$ACTION" == "tests"; then
    tests
elif test "$ACTION" == "tests-vuejs"; then
    testsVuejs
else
    printHelp
fi
