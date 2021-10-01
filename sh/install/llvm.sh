source "sh/common.sh"

if is_windows; then
    choco install -y llvm
else
    bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"
fi
