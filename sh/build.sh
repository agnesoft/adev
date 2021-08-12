source ./sh/common.sh

PARAM=$1
PROJECT_NAME_FROM_SCRIPT_PATTERN=".*build_(.*)\.sh"

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
    local BUILD_SH="./sh/build_$1.sh"

    if test -f "$BUILD_SH"; then
        runBuildScript $BUILD_SH $1
    else
        prrintError "ERROR: Project '$1' does not exist."
        echo "
Available projects:"
        listProjects
        exit 1;
    fi
}

function runBuildScript () {
    echo "Building project '$2'..."
    eval $1
    STATUS=$?

    if test $STATUS -ne 0; then
        printError "ERROR: Building project '$2' ($1) failed: $STATUS"
        exit 1
    else
        printOK "done"
    fi
}

if test "$PARAM" == "list"; then
    listProjects
elif test "$PARAM" == ""; then
    buildAll
else
    buildProject $PARAM
fi
