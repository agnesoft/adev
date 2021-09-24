echo "Usage: ./adev.sh [ACTION] [PARAMS]

./adev.sh <|help|?>                        Prints this help.
./adev.sh analyse                          Run clang-tidy on all projects.
./adev.sh analyse diff                     Run clang-tidy on files/projects changed from main.
./adev.sh analyse <project>                Run clang-tidy on <project>.
./adev.sh build                            Build all projects with the default toolchain.
./adev.sh build list                       List available projects.
./adev.sh build docs                       Build Doxygen documentation.
./adev.sh build <toolchain>                Build all projects with <toolchain>.
./adev.sh build <project>                  Build <project> with default toolchain.
./adev.sh build <project> <toolchain>      Build <project> with <toolchain>.
./adev.sh format                           Formats source code with clang-format.
./adev.sh format check                     Checks formatting of the sources with clang-format.
./adev.sh format check diff                Checks formatting of changed sources with clang-format.
./adev.sh install list                     List available packages.
./adev.sh install <package>                Install <package>.
./adev.sh test                             Run all tests once for build of the default toolchain.
./adev.sh test <iterations>                Run all tests <iterations> for build of the default toolchain.
./adev.sh test <toolchain>                 Run all tests once for build of the <toolchain>.
./adev.sh test <toolchain> <iterations>    Run all tests <iterations> for build of the <toolchain>.

Toolchains:
  clang
  gcc (Linux only, default)
  msvc (Windows only, default)"
