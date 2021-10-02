FROM ubuntu:20.04
WORKDIR /home/adev
RUN apt-get -y update
#tzdata
ENV DEBIAN_FRONTEND="noninteractive"
RUN apt-get -y install tzdata
RUN ln -fs /usr/share/zoneinfo/Europe/Prague /etc/localtime
RUN dpkg-reconfigure --frontend noninteractive tzdata
#docker prerequisites
RUN apt-get -y install dos2unix git sudo
#install scripts
ADD adev.sh /home/adev/
ADD sh /home/adev/sh
RUN find . -type f -print0 | xargs -0 dos2unix
#installation
RUN bash ./adev.sh install llvm
RUN bash ./adev.sh install gcc
RUN bash ./adev.sh install doxygen
