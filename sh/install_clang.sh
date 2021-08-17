source "sh/common.sh"

if isWindows; then
    if isAvailable "clang++.exe"; then
        echo "clang is already installed."
    else
        choco install -y llvm
    fi
else
    if isAvailable "clang++-12"; then
        echo "clang is already installed."
    else
        sudo apt-get update -y
        sudo apt-get install -y clang++-12
    fi
fi
