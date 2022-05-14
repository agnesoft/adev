FROM ubuntu:22.04

WORKDIR /adev
ADD adev.sh /adev
ADD sh /adev/sh
RUN bash sh/docker.sh
