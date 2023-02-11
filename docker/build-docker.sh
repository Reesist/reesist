#!/usr/bin/env bash

export LC_ALL=C

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR/.. || exit

DOCKER_IMAGE=${DOCKER_IMAGE:-reesist/reesistd-develop}
DOCKER_TAG=${DOCKER_TAG:-latest}

BUILD_DIR=${BUILD_DIR:-.}

rm docker/bin/*
mkdir docker/bin
cp $BUILD_DIR/src/reesistd docker/bin/
cp $BUILD_DIR/src/reesist-cli docker/bin/
cp $BUILD_DIR/src/reesist-tx docker/bin/
strip docker/bin/reesistd
strip docker/bin/reesist-cli
strip docker/bin/reesist-tx

docker build --pull -t $DOCKER_IMAGE:$DOCKER_TAG -f docker/Dockerfile docker
