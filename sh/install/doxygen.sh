source "sh/common.sh"

if is_windows; then
    choco install -y doxygen.install
else
    sudo apt-get update -y
    sudo apt-get install -y doxygen
fi
