source "sh/common.sh"

if is_windows; then
    choco install -y llvm
else
    sudo apt-get update -y
    sudo apt-get install -y lsb-release wget software-properties-common apt-utils
    wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key|sudo apt-key add -
    sudo add-apt-repository -y "deb http://apt.llvm.org/focal/ llvm-toolchain-focal-${llvmVersion} main"
    sudo apt-get update -y
    sudo apt-get install -y ${clang}
    sudo apt-get install -y ${clangFormat}
    sudo apt-get install -y ${clangTidy}
    sudo apt-get install -y libc++-${llvmVersion}-dev
    sudo apt-get install -y libc++abi-${llvmVersion}-dev
    sudo apt-get install -y ${lld}
    sudo apt-get install -y llvm-${llvmVersion}
    sudo apt-get install -y libunwind-${llvmVersion}-dev
fi
