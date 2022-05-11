echo "Usage: ./build.sh [ACTION] [PARAMS]

./build.sh <|help|?>                        Prints this help.
./build.sh analyse                          Run clang-tidy on all projects.
./build.sh analyse diff                     Run clang-tidy on files/projects changed from main.
./build.sh analyse <project>                Run clang-tidy on <project>.
./build.sh build                            Build all projects with the default toolchain.
./build.sh build list                       List available projects.
./build.sh build docs                       Build Doxygen documentation.
./build.sh build coverage                   Build all projects with clang and code coverage instrumentation.
./build.sh build libc++-msan                Build libc++ with memory sanitizer flags.
./build.sh build address-sanitizer          Build all projects with clang and address sanitizer flags.
./build.sh build memory-sanitizer           Build all projects with cland and memory sanitizer flags.
./build.sh build thread-sanitizer           Build all projects with cland and thread sanitizer flags.
./build.sh build undefined-sanitizer        Build all projects with cland and undefined behavior sanitizer flags.
./build.sh build <toolchain>                Build all projects with <toolchain>.
./build.sh build <project>                  Build <project> with default toolchain.
./build.sh build <project> <toolchain>      Build <project> with <toolchain>.
./build.sh coverage                         Run tests and collect code coverage stats (requires 'build coverage').
./build.sh format                           Formats source code with clang-format.
./build.sh format check                     Checks formatting of the sources with clang-format.
./build.sh format check diff                Checks formatting of changed sources with clang-format.
./build.sh install list                     List available packages.
./build.sh install <package>                Install <package>.
./build.sh test                             Run all tests once for build of the default toolchain.
./build.sh test <iterations>                Run all tests <iterations> for build of the default toolchain.
./build.sh test <toolchain>                 Run all tests once for build of the <toolchain>.
./build.sh test <toolchain> <iterations>    Run all tests <iterations> for build of the <toolchain>.

Toolchains:
  clang
  gcc (Linux only, default)
  msvc (Windows only, default)"
