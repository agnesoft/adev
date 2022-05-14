source "sh/common.sh"

if is_windows; then
    print_error "GCC is not available on Windows"
    exit 1
else
    sudo add-apt-repository -y "deb http://mirrors.kernel.org/ubuntu ${ubuntuRepository} main universe"
    sudo apt-get update -y
    sudo apt-get install -y g++-${gccVersion}
fi
