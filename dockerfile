FROM ubuntu:20.04
WORKDIR /home/adev
ADD adev.sh /home/adev/
ADD sh /home/adev/sh
RUN bash sh/docker.sh
