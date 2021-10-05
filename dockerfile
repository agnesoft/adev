FROM ubuntu:20.04
WORKDIR /home/adev
#prerequisites
ENV DEBIAN_FRONTEND="noninteractive"
RUN apt-get update -y && apt-get upgrade -y && apt-get install -y tzdata dos2unix git sudo
RUN ln -fs /usr/share/zoneinfo/Europe/Prague /etc/localtime
RUN dpkg-reconfigure --frontend noninteractive tzdata
#install scripts
ADD adev.sh /home/adev/
ADD sh /home/adev/sh
RUN find . -type f -print0 | xargs -0 dos2unix
#installation
RUN bash ./adev.sh install llvm
RUN bash ./adev.sh install gcc
RUN bash ./adev.sh install doxygen
RUN bash ./adev.sh install ninja
RUN bash ./adev.sh install cmake
RUN bash ./adev.sh build libc++-msan
#cleanup
RUN rm -rf /home/adev
RUN rm -rf /home/llvm
RUN apt-get autoremove -y
