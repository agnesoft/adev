#!/usr/bin/env bash

function install_prerequisites() {
    export DEBIAN_FRONTEND="noninteractive"
    apt-get update -y
    apt-get install -y --no-install-recommends tzdata git sudo
    ln -fs /usr/share/zoneinfo/Europe/Prague /etc/localtime
    dpkg-reconfigure --frontend noninteractive tzdata
}

function install() {
    bash ./adev.sh install llvm
    bash ./adev.sh install gcc
    bash ./adev.sh install doxygen
    bash ./adev.sh install ninja
    bash ./adev.sh install cmake
    bash ./adev.sh build libc++-msan
}

function cleanup() {
    rm -rf /home/adev
    rm -rf /home/llvm
    apt-get autoremove -y
}

install_prerequisites
install
cleanup
