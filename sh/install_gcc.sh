source "sh/common.sh"

if isWindows; then
    printError "GCC is not available on Windows"
    exit 1
else
    sudo add-apt-repository -y 'deb http://mirrors.kernel.org/ubuntu hirsute main universe'
    sudo apt-get update -y
    sudo apt-get install -y g++-11
fi
