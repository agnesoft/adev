IF [%1] == [install-prerequisites] (
    choco feature enable -n allowGlobalConfirmation
    choco install visualstudio2019buildtools
    choco install visualstudio2019-workload-vctools
    choco install llvm
    choco install cmake
    choco install ninja
) ELSE (
    set CC="C:/Program Files/LLVM/bin/clang-cl.exe"
    set CXX="C:/Program Files/LLVM/bin/clang-cl.exe"

    mkdir build
    cd build
    "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
    "C:\Program Files\CMake\bin\cmake" .. -GNinja -D CMAKE_BUILD_TYPE=Release
    ninja
)