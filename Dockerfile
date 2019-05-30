FROM ubuntu:18.04
MAINTAINER "Cocoa Oikawa <0xbbc@0xbbc.com>"

USER root
RUN apt-get update && apt-get install -y gcc-aarch64-linux-gnu g++-aarch64-linux-gnu build-essential
WORKDIR /src

