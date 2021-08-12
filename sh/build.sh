PARAM=$1

function findProjectScripts () {
    PROJECTS_SCRIPTS=$(find . -name "build_*.sh" -type f)
}

function listProjects () {
    findProjectScripts

    local PATTERN=".*build_(.*)\.sh"

    for SCRIPT in $PROJECTS_SCRIPTS
    do
        if [[ $SCRIPT =~ $PATTERN ]]
        then
            echo "  ${BASH_REMATCH[1]}"
        fi
    done
}

function buildAll () {
    findProjectScripts

    for SCRIPT in $PROJECTS_SCRIPTS
    do
        eval $SCRIPT
    done
}

function buildProject {
    local BUILD_SH="./sh/build_$1.sh"

    if test -f "$BUILD_SH"; then
        eval $BUILD_SH
    else
        echo "
Project '$1' does not exist.

Available projects:"

        listProjects
        exit 1;
    fi
}

if test "$PARAM" == "list"; then
    listProjects
elif test "$PARAM" == ""; then
    buildAll
else
    buildProject $PARAM
fi
