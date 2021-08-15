source ./sh/common.sh

PARAM=$1
PACKAGE_NAME_FROM_SCRIPT_PATTERN=".*install_(.*)\.sh"

function detectPackages () {
    PACKAGE_SCRIPTS=$(find . -name "install_*.sh" -type f)
}

function installPackage () {
    local INSTALL_SH="./sh/install_$1.sh"

    if test -f "$INSTALL_SH"; then
        runInstallScript $INSTALL_SH $1
    else
        echo ""
        printError "ERROR: Package '$1' does not exist."
        echo "
Available packages:"
        listPackages
        exit 1;
    fi
}

function listPackages () {
    detectPackages

    for SCRIPT in $PACKAGE_SCRIPTS
    do
        if [[ $SCRIPT =~ $PACKAGE_NAME_FROM_SCRIPT_PATTERN ]]
        then
            echo "  ${BASH_REMATCH[1]}"
        fi
    done
}

function runInstallScript () {
    echo "Installing package '$2'..."
    eval $1
    STATUS=$?

    if test $STATUS -ne 0; then
        printError "ERROR: Installing package '$2' ($1) failed: $STATUS"
        exit 1
    fi
}

if test "$PARAM" == "list"; then
    listPackages
else
    installPackage $PARAM
fi
