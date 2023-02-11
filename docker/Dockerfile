FROM debian:stretch
LABEL maintainer="Reesist Developers <dev@reesist.org>"
LABEL description="Dockerised ReesistCore, built from Travis"

RUN apt-get update && apt-get -y upgrade && apt-get clean && rm -fr /var/cache/apt/*

COPY bin/* /usr/bin/
