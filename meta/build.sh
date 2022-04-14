#!/bin/sh

SCRIPT_DIR="$(dirname "${0}")"
PROJECT_DIR="$(dirname $SCRIPT_DIR)"
BUILD_DIR=$PROJECT_DIR/build

if [ ! -d $BUILD_DIR ]; then
    mkdir $BUILD_DIR
    cd $BUILD_DIR
    cmake .. -G Ninja
    ninja
else
    cd $BUILD_DIR
    ninja
fi
