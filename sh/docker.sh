#!/usr/bin/env bash

source "sh/common.sh"

function install_prerequisites() {
    export DEBIAN_FRONTEND="noninteractive"
    apt-get update -y
    apt-get install -y --no-install-recommends tzdata git sudo
    ln -fs /usr/share/zoneinfo/Europe/Prague /etc/localtime
    dpkg-reconfigure --frontend noninteractive tzdata
}

function install() {
    bash ./build.sh install llvm
    bash ./build.sh install gcc
    bash ./build.sh install doxygen
    bash ./build.sh install ninja
    bash ./build.sh install cmake
    bash ./build.sh libc++-msan
}

function cleanup() {
    rm "${home}/build.sh"
    rm -rf "${home}/sh"
    rm -rf "${home}/llvm"
}

install_prerequisites
install
cleanup
