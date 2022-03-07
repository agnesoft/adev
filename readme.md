# adev

[![adev](https://github.com/agnesoft/adev/actions/workflows/adev.yml/badge.svg)](https://github.com/agnesoft/adev/actions/workflows/adev.yml)

Agnesoft central development repository.

- [adev](#adev)
  - [Usage](#usage)
  - [Projects](#projects)
  - [Tools](#tools)
  - [Development](#development)
  - [Continuous Integration](#continuous-integration)
    - [pr](#pr)
    - [adev](#adev-1)
  - [Known Issues](#known-issues)

## Usage

Run (use Git Bash on Windows):

```
./adev.sh
```

This will show all available actions: building, running various checks, installation of prerequisites & tools etc. E.g.

To build simply run any of:

```
./adev.sh build
./adev.sh build <toolchain>
./adev.sh build <project> <toolchain>
```

Available toolchains are:

-   **clang**
-   **gcc\*** (default, Linux only)
-   **msvc** (default, Windows only)

\*see [Known Issues](#known-issues)

The binaries will be output to `build/<toolchain>`.

## Projects

| Name                                            | Path                                              | Description                                                                                              |
| ----------------------------------------------- | ------------------------------------------------- | -------------------------------------------------------------------------------------------------------- |
| [acommandline](projects/acommandline/readme.md) | [projects/acommandline/](/projects/acommandline/) | c++ command line parser                                                                                  |
| [aprocess](projects/aprocess/readme.md)         | [projects/aprocess/](/projects/aprocess/)         | c++ process management library                                                                           |
| [astl](projects/astl/readme.md)                 | [projects/astl/](/projects/astl/)                 | c++ standard template library module wrapper                                                             |
| [atest](projects/atest/readme.md)               | [projects/atest/](/projects/atest/)               | c++ testing framework                                                                                    |
| [awinapi](projects/awinapi/readme.md)           | [projects/awinapi/](/projects/awinapi/)           | c++ winapi module wrapper                                                                                |
| [yamlcpp](projects/yamlcpp/readme.md)           | [projects/yamlcpp/](/projects/yamlcpp/)           | c++ [yaml-cpp](https://github.com/jbeder/yaml-cpp/) module wrapper, [LICENSE](/projects/yamlcpp/license) |

## Tools

| Windows                                                                | Installation                | Note                                                                |
| ---------------------------------------------------------------------- | --------------------------- | ------------------------------------------------------------------- |
| [(Git) Bash](https://git-scm.com/download/win)                         | -                           |                                                                     |
| [Visual Studio 2022](https://visualstudio.microsoft.com/cs/downloads/) | manual                      |                                                                     |
| [clang 13](https://llvm.org/)                                          | `./adev.sh install llvm`    |                                                                     |
| [gcc 11\*](https://gcc.gnu.org/)                                       | `./adev.sh install gcc`     | See [Known Issues](#known-issues)                                   |
| [clang-tidy](https://clang.llvm.org/extra/clang-tidy/)                 | `./adev.sh install llvm`    |                                                                     |
| [llvm-cov](https://clang.llvm.org/docs/SourceBasedCodeCoverage.html)   | `./adev.sh install llvm`    |                                                                     |
| [Doxygen](https://www.doxygen.nl/index.html)                           | `./adev.sh install doxygen` |                                                                     |
| [clang-format](https://clang.llvm.org/docs/ClangFormat.html)           | `./adev.sh install llvm`    |                                                                     |
| [Docker Desktop](https://docs.docker.com/desktop/windows/install/)     | manual                      | Required only for [Continuous Integration](#continuous-integration) |

## Development

Please refer to [contribution.md](contribution.md) and [style_guide.md](style_guide.md) for general information.

To setup the development environment use `./adev.sh install <package>` actions or install the [tools](#tools) manually.

Workflow summary:

1. [Open and issue](https://github.com/agnesoft/adev/issues/new/choose)
2. Create a branch for that issue
    - observe the naming rules in the [style_guide.md](style_guide.md), i.e. use `[project tag]` and name the branch after your issue, including the issue #
3. Work on the branch
    - build & run checks via `./adev.sh`
4. Push the branch to GitHub & open PR from your branch to `main`
5. Merge to `main`
    - requires 1 approval & all [Continous Integration](#continuous-integration) checks to pass

## Continuous Integration

The `adev` is using GitHub Actions. The `.adev.sh` script actions are run as part of the continuous integration (CI) on every pull request and subsequent merge to `main`. Most actions are run on Ubuntu based custom docker image that comes with preinstalled prerequisites & tools listed above:

-   [`agnesoft/adev`](https://github.com/agnesoft/adev/pkgs/container/adev)

The docker image is rebuilt if it changes in a PR and the checks are run on such an image. If the PR with a changed `dockerfile` is merged to `main` the build will rebuild & push the `latest` tag of that image to the repository.

Every CI run uploads artifacts (binaries, documentation, coverage report) for usage or inspection even if it failed for easier debugging.

There are two workflows:

### pr

-   runs on every pull request (pr)
-   checks are run only on the diff to the latest `main`
-   skips irrelevant checks.

### adev

-   runs on every push (merge) to `main`
-   all checks are run fully

## Known Issues

| Platform | Action            | Tool           | Version | Description                                                                                                                                                           | Workaround                                                                                                                             | Date       |
| -------- | ----------------- | -------------- | ------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------- | ---------- |
| any      | coverage          | llvm           | 13      | LLVM instrumentation has difficulties with `if constexpr` and some other entities showing them "uncovered" even though they are executed.                             | **None.** Increase coverage thresholds as needed.                                                                                      | 29/09/2021 |
| any      | coverage          | llvm           | 13      | LLVM source code based coverage has difficulties with spawned processes. The resulting `profraw` file is reported as corrupted and cannot be used by `llvm-profdata`. | **None.** Exclude affected projects from coverage.                                                                                     | 25/11/2021 |
| any      | analysis          | llvm           | 13      | `clang-tidy` cannot use TemplateParameterCase style parameter along with other style checks citing "invalid style".                                                   | **None.** Do not use for now.                                                                                                          | 02/10/2021 |
| Linux    | build             | gcc            | 11      | Unable to use its own STL (`libstdc++`) as header units. Build fails on internal compiler error. The `libc++` does not work either.                                   | **None.** GCC is not usable with modules and STL.                                                                                      | 31/08/2021 |
| Linux    | analysis          | llvm           | 13      | `clang-tidy` has difficulties using Unix headers such as `wait.h` as header units even with correct `module.modulemap`. The build si ok.                              | Specify the headers in `module.modulemap` manually.                                                                                    | 25/11/2021 |
| Linux    | address-sanitizer | llvm           | 13.0.1  | Address sanitizer produces `alloc-dealloc-mismatch` false positives coming from `libc++`.                                                                             | Set `export ASAN_OPTIONS=alloc_dealloc_mismatch=0` before running the tests built with asan.                                           | 18/01/2022 |
| Linux    | memory-sanitizer  | llvm           | 13.0.1  | Memory sanitizer detects uninitialized variables in `libc++` in `std::filesystem` (/llvm/libcxx/src/filesystem/operations.cpp).                                       | Set `fun:*create_file_status*` in `ignorelist.txt` and use `-fsanitize-ignorelist=ignorelist.txt` during the memsan build of `libc++`. | 14/02/2022 |
| Windows  | docker            | Docker Desktop | 4.1     | It cannot be installed if there is an existing WSL 2 image. Docker Desktop must be installed when there are no other WSL 2 images.                                    | Remove all WSL2 images before installing Docker Desktop.                                                                               | 02/10/2021 |
| Windows  | build             | msvc           | 16.11.5 | Using `<Windows.h>` as a header unit produces warnings 4005, 5105 and 5106 transitively (i.e. all uses even indirect ones).                                           | Suppress `4005`, `5105` and `5106` for all direct and indirect usages of `windows.h`.                                                  | 03/11/2021 |
