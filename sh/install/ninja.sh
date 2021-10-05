source "sh/common.sh"

if is_windows; then
    choco install -y ninja
else
    sudo apt-get update -y
    sudo apt-get install -y ninja-build
fi
