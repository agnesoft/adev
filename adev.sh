ACTION=$1
PARAM=$2

function printHelp () {
    help="Usage: ./adev.sh [ACTION] [PARAM]

./adev.sh build                Build all projects.
./adev.sh build list           List available projects.
./adev.sh build <project>      Build project.
./adev.sh install list         List available packages.
./adev.sh install <package>    Install package."
    echo "$help"
}

if test "$ACTION" == "build"; then
    sh/build.sh $PARAM
elif test "$ACTION" == "install"; then
    sh/install.sh $PARAM
else
    printHelp
fi
