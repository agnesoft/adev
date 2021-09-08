source ./sh/common.sh

ACTION=
PROJECT=
PROJECT_NAME_FROM_SCRIPT_PATTERN=".*build_(.*)\.sh"
TOOLCHAIN=

function detectProjects () {
    PROJECTS_SCRIPTS=$(find . -name "build_*.sh" -type f)
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
        printError "ERROR: Project '$PROJECT' does not exist."
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
        printError "ERROR: Building project '$2' ($1) failed: $STATUS"
        exit 1
    fi
}

function setProperties () {
    if test "$1" == "list"; then
        ACTION="list"
    elif test "$1" == "clang" || test "$1" == "msvc" || test "$1" == "gcc" || test "$1" == ""; then
        ACTION="buildAll"
        TOOLCHAIN=$1
    elif test "$2" == "clang" || test "$2" == "msvc" || test "$2" == "gcc" || test "$2" == ""; then
        ACTION="buildProject"
        PROJECT=$1
        TOOLCHAIN=$2
    fi

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

setProperties $1 $2
removeDoneFiles

if test "$ACTION" == "list"; then
    listProjects
elif test "$ACTION" == "buildAll"; then
    buildAll
elif test "$ACTION" == "buildProject"; then
    buildProject
else
    printError "ERROR: unknown action '$ACTION'"
    exit 1
fi
