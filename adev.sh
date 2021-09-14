ACTION=$1
PARAM=$2

function printHelp () {
    help="Usage: ./adev.sh [ACTION] [PARAMS]

./adev.sh analyse                        Run static analysis on all projects with clang-tidy.
./adev.sh analyse <project>              Run static analysis on <project> with clang-tidy.
./adev.sh build                          Build all projects with the default toolchain.
./adev.sh build <toolchain>              Build all projects with <toolchain>.
./adev.sh build <project>                Build <project>.
./adev.sh build <project> <toolchain>    Build <project> with <toolchain>.
./adev.sh build list                     List available projects.
./adev.sh build docs                     Build Doxygen documentation.
./adev.sh format                         Formats source code with clang-format.
./adev.sh install list                   List available packages.
./adev.sh install <package>              Install <package>.
./adev.sh test                           Run all tests for build of the default toolchain.
./adev.sh test <toolchain>               Run all tests for build of the <toolchain>.

Toolchains:
  clang
  gcc (Linux only, default)
  msvc (Windows only, default)"

    echo "$help"
}

if test "$ACTION" == "analyse"; then
    sh/clang_tidy.sh $PARAM
elif test "$ACTION" == "build"; then
    sh/build.sh $PARAM $3
elif test "$ACTION" == "format"; then
    sh/format.sh
elif test "$ACTION" == "install"; then
    sh/install.sh $PARAM
elif test "$ACTION" == "test"; then
    sh/test.sh $PARAM
else
    printHelp
fi
