ACTION=$1
PARAM=$2

function printHelp () {
    help="Usage: ./adev.sh [ACTION] [PARAMS]

./adev.sh build                          Build all projects.
./adev.sh build <toolchain>              Build all projects with toolchain.
./adev.sh build <project>                Build project.
./adev.sh build <project> <toolchain>    Build project with toolchain.
./adev.sh build list                     List available projects.
./adev.sh install list                   List available packages.
./adev.sh install <package>              Install package.

Toolchains:
  clang
  msvc (Windows only)"

    echo "$help"
}

if test "$ACTION" == "build"; then
    sh/build.sh $PARAM $3
elif test "$ACTION" == "install"; then
    sh/install.sh $PARAM
else
    printHelp
fi
