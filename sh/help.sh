[ -n "$HELP_SH" ] && return || readonly HELP_SH=1

function help_() {
    echo \
"Usage: ./build.sh [OPTIONS]

./build.sh                                                            Builds sources with default toolchain and release configuration.
./build.sh list                                                       List available projects.
./build.sh <configuration>                                            Builds sources with default toolchain and <configuration>.
./build.sh <toolchain>                                                Builds sources with <toolchain> and release configuration.
./build.sh <toolchain> <configuration>                                Builds sources with <toolchain> and <configuration>.
./build.sh <project>                                                  Builds <project> with default toolchain and release configuration.
./build.sh <project> <toolchain>                                      Builds <project> with <toolchain> and release configuration.
./build.sh <project> <configuration>                                  Builds <project> with default toolchain and <configuration>.
./build.sh <project> <toolchain> <configuration>                      Builds <project> with <toolchain> and release configuration.
./build.sh docs                                                       Builds documentation with Doxygen.
./build.sh libc++-msan                                                Builds libc++ with memory sanitizer.
./build.sh analyse                                                    Runs clang-tidy on all sources with all checks.
./build.sh analyse <checks>                                           Runs clang-tidy on all sources with <checks> sets.
./build.sh analyse diff                                               Runs clang-tidy on sources changed from main with all checks.
./build.sh analyse diff <checks>                                      Runs clang-tidy on sources changed from main with <checks> sets.
./build.sh analyse <file>                                             Runs clang-tidy on <file> with all checks.
./build.sh analyse <file> <checks>                                    Runs clang-tidy on <file> with <checks> sets.
./build.sh format                                                     Runs clang-format on all sources.
./build.sh format check                                               Runs clang-format on all sources to detect unformatted files.
./build.sh format check diff                                          Runs clang-format on sources changed from main to detect unformatted files.
./build.sh install list                                               Lists available packages.
./build.sh install <package>                                          Installs <package> for current platform.
./build.sh test                                                       Runs all tests once for default toolchain and release configuration.
./build.sh test <iterations>                                          Runs all tests <iterations> times for default toolchain and release configuration.
./build.sh test <configuration>                                       Runs all tests once for default toolchain and <configuration>.
./build.sh test <configuration> <iterations>                          Runs all tests <iterations> times for default toolchain and <configuration>.
./build.sh test coverage                                              Runs all tests once for clang and coverage configuration. Reports coverage.
./build.sh test <toolchain>                                           Runs all tests once for <toolchain> and release configuration.
./build.sh test <toolchain> <iterations>                              Runs all tests <iterations> times for <toolchain> and release configuration.
./build.sh test <toolchain> <configuration>                           Runs all tests once for <toolchain> and <configuration>.
./build.sh test <toolchain> <configuration> <iterations>              Runs all tests <iterations> times for <toolchain> and <configuration>.
./build.sh test <project> <toolchain>                                 Runs <project> test once for <toolchain> and release configuration.
./build.sh test <project> <toolchain> <iterations>                    Runs <project> test <iterations> times for <toolchain> and release configuration.
./build.sh test <project> <toolchain> <configuration>                 Runs <project> test once for <toolchain> and <configuration>.
./build.sh test <project> <toolchain> <configuration> <iterations>    Runs <project> test <iterations> times for <toolchain> and <configuration>.
./build.sh test <project> coverage                                    Runs <project> test once for clang and coverage configuration. Reports <project> coverage.


Toolchain         | Platforms             | Default Platform
------------------|-----------------------|-----------------
clang             | Windows, macOS, Linux | macOS
gcc               | Linux                 | Linux
msvc              | Windows               | Windows

Configuration     | Description
------------------|-----------------------------------------
release           | Optimized build
debug             | Unoptimized build with debug symbols
coverage          | Instrumented build (clang only)
memory            | Memory sanitized build (clang only)
address           | Address sanitized build (gcc and clang)
thread            | Thread sanitized build (gcc and clang)
undefined         | UB sanitized build (gcc and clang)

Check Set         | Decsription
------------------|-----------------------------------------
bugprone          | Bugprone checks
cert              | CERT Security Guidelines
cppcoreguidelines | C++ Core Guidelines
fuchsia           | Fuchsia OS (Android Successor)
google            | Google checks
hicpp             | High Performance C++
misc              | Miscellaneous checks
modernize         | Modernize checks
performance       | Performance checks
readability       | Readability checks"
}
