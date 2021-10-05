FROM ubuntu:20.04
ADD adev.sh /adev
ADD sh /adev/sh
WORKDIR /adev
RUN bash sh/docker.sh
