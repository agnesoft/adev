FROM ubuntu:20.04
ADD adev.sh /root
ADD sh /root/sh
WORKDIR /root
RUN bash sh/docker.sh
