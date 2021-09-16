source ./sh/common.sh

ACTION=
PROJECT=
PROJECT_NAME_FROM_SCRIPT_PATTERN=".*build_(.*)\.sh"
TOOLCHAIN=

function detectProjects () {
    PROJECTS_SCRIPTS=$(find . -name "build_*.sh" -type f)
}

function detectDoxygen () {
    if is_available "doxygen"; then
        DOXYGEN="doxygen"
    elif is_windows && test -f "C:/Program Files/doxygen/bin/doxygen.exe"; then
        DOXYGEN="C:/Program Files/doxygen/bin/doxygen.exe"
    else
        print_error "ERROR: 'doxygen' is not available. Try installing it with './adev.sh install doxygen'."
        exit 1
    fi

    echo "Doxygen $("$DOXYGEN" --version)"
}

function listProjects () {
    detectProjects

    for SCRIPT in $PROJECTS_SCRIPTS
    do
        if [[ $SCRIPT =~ $PROJECT_NAME_FROM_SCRIPT_PATTERN ]]
        then
            echo "  ${BASH_REMATCH[1]}"
        fi
    done
}

function buildDocs () {
    detectDoxygen
    echo "Generating documentation..."
    mkdir -p "build/"
    "$DOXYGEN" adev.doxyfile
    if test $? -ne 0; then
        print_error "ERROR: Building documentation failed."
        exit 1
    else
        print_ok "Documentation OK"
    fi
}

function buildAll () {
    echo "Projects: all"
    echo "Toolchain: $TOOLCHAIN"
    echo ""

    detectProjects

    for SCRIPT in $PROJECTS_SCRIPTS
    do
        if [[ $SCRIPT =~ $PROJECT_NAME_FROM_SCRIPT_PATTERN ]]
        then
            runBuildScript $SCRIPT ${BASH_REMATCH[1]}
        fi
    done
}

function buildProject () {
    echo "Projects: $PROJECT"
    echo "Toolchain: $TOOLCHAIN"
    echo ""

    local BUILD_SH="./sh/build_$PROJECT.sh"

    if test -f "$BUILD_SH"; then
        runBuildScript $BUILD_SH $PROJECT
    else
        echo ""
        print_error "ERROR: Project '$PROJECT' does not exist."
        echo "
Available projects:"
        listProjects
        exit 1;
    fi
}

function removeDoneFiles () {
    if test -d "build/$TOOLCHAIN/"; then
        find build/$TOOLCHAIN/ -name "*.done" -type f -delete
    fi
}

function runBuildScript () {
    eval "$1 $TOOLCHAIN"
    STATUS=$?

    if test $STATUS -ne 0; then
        print_error "ERROR: Building project '$2' ($1) failed: $STATUS"
        exit 1
    fi
}

function setProperties () {
    if test "$1" == "list"; then
        ACTION="list"
    elif test "$1" == "docs"; then
        ACTION="docs"
    elif test "$1" == "clang" || test "$1" == "msvc" || test "$1" == "gcc" || test "$1" == ""; then
        ACTION="buildAll"
        TOOLCHAIN=$1
    elif test "$2" == "clang" || test "$2" == "msvc" || test "$2" == "gcc" || test "$2" == ""; then
        ACTION="buildProject"
        PROJECT=$1
        TOOLCHAIN=$2
    fi

    if test "$TOOLCHAIN" == ""; then
        if is_windows; then
            TOOLCHAIN="msvc"
        else
            TOOLCHAIN="gcc"
        fi
    fi

    if ! is_windows && test "$TOOLCHAIN" == "msvc"; then
        print_error "ERROR: toolchain 'msvc' is only available on Windows"
        exit 1
    fi

    if ! is_linux && test "$TOOLCHAIN" == "gcc"; then
        print_error "ERROR: toolchain 'gcc' is only available on Linux"
        exit 1
    fi
}

setProperties $1 $2
removeDoneFiles

if test "$ACTION" == "list"; then
    listProjects
elif test "$ACTION" == "docs"; then
    buildDocs
elif test "$ACTION" == "buildAll"; then
    buildAll
elif test "$ACTION" == "buildProject"; then
    buildProject
else
    print_error "ERROR: unknown action '$ACTION'"
    exit 1
fi
