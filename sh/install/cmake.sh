source "sh/common.sh"

if is_windows; then
    choco install -y cmake
else
    wget -O cmake.sh https://github.com/Kitware/CMake/releases/download/v3.21.3/cmake-3.21.3-linux-x86_64.sh
    chmod +x cmake.sh
    mkdir -p "${home}/cmake"
    ./cmake.sh --skip-license --prefix="${home}/cmake"
    chmod -R +x ${home}/cmake/bin/*

    for binaryPath in ${home}/cmake/bin/*; do
        binary=$(basename $binaryPath)
        sudo update-alternatives --install /usr/bin/$binary $binary $binaryPath 1
    done

    rm cmake.sh
fi
