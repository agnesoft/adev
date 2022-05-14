[ -n "$BUILD_LIBCPP_MSAN_SH" ] && return || readonly BUILD_LIBCPP_MSAN_SH=1

source "sh/build_common.sh"

function build_libcpp_msan() {
    detect_cmake
    detect_ninja
    get_llvm_sources
    set_build_properties "clang" "memory"

    cd ${home}/llvm/build_msan/

    #echo "src:/adev/llvm/libcxx/src/filesystem/operations.cpp" > ignorelist.txt
    local dir=$(pwd)

    cmake -G Ninja \
          ../llvm \
          -D CMAKE_CXX_FLAGS="-fsanitize-ignorelist=${dir}/ignorelist.txt" \
          -D CMAKE_BUILD_TYPE=Release \
          -D LLVM_ENABLE_PROJECTS="libcxx;libcxxabi" \
          -D CMAKE_C_COMPILER=clang-${llvmVersion} \
          -D CMAKE_CXX_COMPILER=${clang} \
          -D LLVM_USE_SANITIZER=MemoryWithOrigins \
          -D CMAKE_INSTALL_PREFIX="${home}/libc++-msan/"

    cmake --build . -- cxx cxxabi
    cmake --build . -- install-cxx install-cxxabi
}

function detect_cmake() {
    if ! is_available "cmake"; then
        print_error "ERROR: 'cmake' is not available. Try installing it with './build.sh install cmake'."
        exit 1
    fi

    echo "$(cmake --version | head -n 1)"
}

function detect_ninja () {
    if ! is_available "ninja"; then
        print_error "ERROR: 'ninja' is not available. Try installing it with './build.sh install ninja'."
        exit 1
    fi

    echo "Ninja $(ninja --version)"
}

function get_llvm_sources() {
    if [[ ! -d "${home}/llvm" ]]; then
        git clone --depth=1 -b llvmorg-14.0.3 https://github.com/llvm/llvm-project.git "${home}/llvm"
    fi

    mkdir -p "${home}/llvm/build_msan/"
    mkdir -p "${home}/libc++-msan/"
}
