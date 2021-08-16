source "sh/common.sh"

if isWindows; then
    if isAvailable "clang-format.exe"; then
        echo "Clang is already installed."
    else
        choco install -y llvm
    fi
else
    if isAvailable "clang-format-12"; then
        echo "Clang is already installed."
    else
        sudo apt-get update -y
        sudo apt-get install -y clang-format-12
    fi
fi
