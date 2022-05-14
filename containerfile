FROM ubuntu:22.04

WORKDIR /adev
ADD build.sh /adev
ADD sh /adev/sh
RUN bash sh/docker.sh
