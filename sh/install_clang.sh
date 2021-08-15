source "sh/common.sh"

if isAvailable "clang++"; then
    echo "Clang is already installed."
elif isWindows; then
    choco install -y llvm
else
    sudo apt-get update -y
    sudo apt-get install -y clang++-12
    sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-12 1
fi
