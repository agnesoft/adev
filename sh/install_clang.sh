source "sh/common.sh"

if isWindows; then
    choco install -y llvm
else
    sudo apt-get update -y
    sudo apt-get install -y clang++-12 libc++-12-dev libc++abi-12-dev libc++abi1-12
fi
