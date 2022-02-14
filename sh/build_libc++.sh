source "sh/common.sh"

function detect_cmake() {
    if ! is_available "cmake"; then
        print_error "ERROR: 'cmake' is not available. Try installing it with './adev.sh install cmake'."
        exit 1
    fi

    echo "$(cmake --version | head -n 1)"
}

function detect_ninja () {
    if ! is_available "ninja"; then
        print_error "ERROR: 'ninja' is not available. Try installing it with './adev.sh install ninja'."
        exit 1
    fi

    echo "Ninja $(ninja --version)"
}

function get_llvm_sources() {
    if [[ ! -d "${home}/llvm" ]]; then
        git clone --depth=1 -b llvmorg-13.0.1 https://github.com/llvm/llvm-project.git "${home}/llvm"
    fi

    mkdir -p "${home}/llvm/build_msan/"
    mkdir -p "${home}/libc++-msan/"
}

function build() {
    cd ${home}/llvm/build_msan/

    cmake -G Ninja \
          ../llvm \
          -D CMAKE_BUILD_TYPE=Release \
          -D LLVM_ENABLE_PROJECTS="libcxx;libcxxabi" \
          -D CMAKE_C_COMPILER=clang-${llvmVersion} \
          -D CMAKE_CXX_COMPILER=${clang} \
          -D LLVM_USE_SANITIZER=MemoryWithOrigins \
          -D CMAKE_INSTALL_PREFIX="${home}/libc++-msan/"

    cmake --build . -- cxx cxxabi
    cmake --build . -- install-cxx install-cxxabi
}

detect_cmake
detect_ninja
get_llvm_sources
build
