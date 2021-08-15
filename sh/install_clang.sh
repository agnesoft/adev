source "sh/common.sh"

if isAvailable "clang++"; then
    echo "Clang is already installed."
elif isWindows; then
    choco install -y llvm
else
    sudo apt-get update -y
    sudo apt-get install -y clang++-12
fi
