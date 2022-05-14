source "sh/common.sh"

if is_windows; then
    choco install -y llvm
else
    sudo apt-get update -y
    sudo apt-get install -y lsb-release wget software-properties-common apt-utils
    wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key|sudo apt-key add -
    sudo add-apt-repository -y "deb http://apt.llvm.org/${ubuntuRepository}/ llvm-toolchain-${ubuntuRepository}-${llvmVersion} main"
    sudo apt-get update -y
    sudo apt-get install -y ${clang} ${clangFormat} ${clangTidy} libc++-${llvmVersion}-dev libc++abi-${llvmVersion}-dev ${lld} llvm-${llvmVersion} libunwind-${llvmVersion}-dev
fi
