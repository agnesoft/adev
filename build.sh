if [[ $1 == "install-prerequisites" ]] ; then
    sudo apt update
    sudo apt install -y cmake ninja lsb-release software-properties-common wget
    sudo bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"
fi

export CC=clang-10
export CXX=clang++-10

mkdir -p build
cd build
cmake .. -D CMAKE_BUILD_TYPE=Release
make
