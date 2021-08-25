source "sh/common.sh"

if isWindows; then
    choco install -y llvm
else
    sudo apt-get update -y
    sudo apt-get install -y clang-format-12
fi
